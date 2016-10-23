

////////////////////////////////////////////////////DECLARATIONS//////////////////////////////////////////////

//  **************************** SMS**************************************************

//#include "SIM900.h"
//#include "SoftwareSerial.h"
//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"
//If you want to use the Arduino functions to manage SMS, uncomment the lines below.

#include "sms.h"
SMSGSM sms;

//To change pins for Software Serial, use the two lines in GSM.cpp.
//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.
//Simple sketch to send and receive SMS.

boolean started = false;

char sms_position;
char phone_number[12]  ; // array for the phone number string
char  phoneToCall[12]= "0687972790" ;
char sms_text[8];
char tmp[15];
char cons[3];
char U[3];
int consigne = 2;
int i;
int p;             //position du message
int c;             //compteur de messages pour alarme temperature
int d;             //compteur de messages pour alarme batterie

unsigned long oldTime = 0 ;

//*****************************DS18B20************************************************
#include <OneWire.h>
#include <DallasTemperature.h>

// On utilise la Pin #7 pour connecter le DS18B20
#define ONE_WIRE_BUS 7

// Initialisation de la librairie OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Lien avec la librairie Dallas Temperature Control
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

const int  resolution = 10;

#include <Stdio.h>


//*****************************Horloge DS3231************************************************
/*   #include <Wire.h>
#include <RTClib.h>

RTC_DS1307 RTC; // Avec cette librairie on déclare le DS3231 comme un DS1307   */

//*****************************mesure tension************************************************
int Vin = A0 ;
const int nbreLect = 5;              // Nombre de lectures du tableau de mesure  ( SMOOTHING )
int lectures[nbreLect];                     // Tableau de lectures de l'entrée analogique ( SMOOTHING )// tension d'entree ramenée à 5V par resistances (Vin = Vbat/3)
int index = 0;                      // Index de la lecture en cours ( SMOOTHING )
int total = 0;                          // Total des lectures ( SMOOTHING )
int moyenne = 0;                        // Moyenne des lectures 
float Vbat;
int V;

/////////////////////////////////////////////////////////SETUP////////////////////////////////////////////////////////////////

void setup()
{
  c=0;
  d=0 ;
  
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH );
 pinMode(13, OUTPUT);
  digitalWrite(13, LOW );
  
  
  //Serial connection.
  Serial.begin(9600);


delay(20000);
  
  //  **************************** SMS**************************************************
  //Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.


  if (gsm.begin(4800))
  {
    Serial.println("\nstatus=READY");
    started = true;
  }
  else
  Serial.println("\nstatus=IDLE");


  delay(20000);

  if (started)
  {

   
    //Enable this two lines if you want to send an SMS.
    sms.SendSMS(phoneToCall, "SMS ok");
      // Serial.println("\nSMS sent OK");
      
      //if NO SPACE ,you need delete SMS from position 1 to position 50
      for (i = 1; i <= 50; i++)
      {
        sms.DeleteSMS(i);
        delay(5);
      }

  }
  //*****************************DS18B20************************************************

  //Serial.println("Prise de temperature avec  DS18B20");

  // On initialise la librairie DTC
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, resolution);
  sensors.getAddress(tempDeviceAddress, 1);
  sensors.setResolution(tempDeviceAddress, resolution);

  //*****************************Horloge DS3231************************************************

/*  Wire.begin();
  RTC.begin();
  //reglage de l'heure: décocher les 2 lignes suivantes pour effectuer le réglage(charger , recocher ,charger )
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  //DateTime now = RTC.now();    */
}
/////////////////////////////////////////////////////////LOOP////////////////////////////////////////////////////////////////
void loop()

{
      //Serial.println(c);
      //Serial.println(d);
      //Serial.println(Vbat);
      
      //Serial.println( );

      
     sensors.requestTemperatures(); // Envoi de la demande de température

     
     mesureU()  ;
 
/*  //Lecture de la date et de l'heure
  DateTime now = RTC.now(); // Lit l'heure du module  */

  // Effacement des SMS quand 10 reçus
  if (p == 10)
  { 
    for (i = 1; i <= 11; i++)
    {
      sms.DeleteSMS(i);
      delay(10);
    }
    p = 0;
  }

       float temp0 = (sensors.getTempCByIndex(0));
        int t0 = (temp0 - (int)temp0) * 100 ;
        //Serial.print("La temperature pour le DS18B20 N_1 est de: ");
        //Serial.println(temp0); // Le 0 corresponant au premier esclave trouvé .


        
        float temp1 = (sensors.getTempCByIndex(1));
        int t1 = (temp1 - (int)temp1) * 100 ;
        //Serial.print("La temperature pour le DS18B20 N_2 est de: ");
       //Serial.println(temp1); // Le 1 corresponant au 2nd esclave trouvé ).

       

  if (started)
  {
     
    

    //Serial.println(p);
    //  vérification entrée de SMS
    sms_position = sms.IsSMSPresent(SMS_UNREAD);

    delay(5000) ;

    if (sms_position)
    {
      // récupération du nouveau SMS
     // Serial.print("SMS postion:");
     // Serial.println(sms_position, DEC);
      sms.GetSMS(sms_position, phone_number, sms_text, 10);
      // now we have phone number string in phone_num
      //Serial.println(phone_number);
      // and SMS text in sms_text
      //Serial.println(sms_text);


      int compar1 = (strcmp(sms_text, "Tmp")); // comparaison des mots
      /*int compar2 = (strcmp(sms_text, "On")); // comparaison des mots
      int compar3 = (strcmp(sms_text, "Off")); // comparaison des mots*/
      int compar4 = (strcmp(sms_text, "-")); // comparaison des mots
      int compar5 = (strcmp(sms_text, "+")); // comparaison des mots
      int compar6 = (strcmp(sms_text, "Al")); // comparaison des mots
      int compar7 = (strcmp(sms_text, "Bat")); // comparaison des mots

      
      if (compar1 == 0)
      {
       
        //sprintf(tmp,"%0d.%d\r%0d.%d ",(int)temp0,t0,(int)temp1,t1); // tmp contient le texte et les valeurs sous forme de chaine.
        sprintf(tmp,"Capteur 1: %0d.%02d\rCapteur 2: %0d.%02d",(int)temp0,t0,(int)temp1,t1); // tmp contient le texte et les valeurs sous forme de chaine.

        (sms.SendSMS(phoneToCall, tmp));                           // si le message est "Tmp"
        //Serial.println("\nSMS sent OK");
      }
      /*
      else if (compar2 == 0)
      {
        digitalWrite(8, LOW);                                     // si le message est "On"
        sms.SendSMS(phoneToCall, "On ok");
        //Serial.println("\nSMS sent OK");

      }
      else if (compar3 == 0)                                    // si le message est "Off"
      {

        digitalWrite(8, HIGH );
        sms.SendSMS(phoneToCall, "Off ok");
        //Serial.println("\nSMS sent OK");
      }
      */
      else if (compar4 == 0)                            // si le message est "Moins"
      {
        if (consigne >0 ) 
        {   
        consigne--;
        }
        sprintf(cons,"Alarme: %d ",consigne);
        sms.SendSMS(phoneToCall, cons);   
        //Serial.println("\nSMS sent OK");
      }

      else if (compar5 == 0)                                      // si le message est "Plus"
      {
        if (consigne <10 )  
        {
        consigne++;
        }
        sprintf(cons,"Alarme: %d ",consigne);
        sms.SendSMS(phoneToCall, cons);
        //Serial.println("\nSMS sent OK");
      }
       else if (compar6 == 0)                                      // si le message est "Cons"
      {
        sprintf(cons,"Alarme: %d ",consigne);
        sms.SendSMS(phoneToCall, cons);
        //Serial.println("\nSMS sent OK");
        
      }
       else if (compar7 == 0)                                      // si le message est "Bat"
      {
        
        sprintf(U,"U= %0d.%02d V ",(int)Vbat,V);
        sms.SendSMS(phoneToCall, U);
        //Serial.println("\nSMS sent OK");
        
      }
      else if ((compar1=!0) && /*(compar2=!0)&& (compar3=!0) && */(compar4=!0)&& (compar5=!0) && (compar6=!0)&& (compar7=!0)) // si les messages ne correspondent pas aux textes de la liste
      {
        sms.SendSMS(phoneToCall, "Commande inconnue");
        //Serial.println("\nSMS sent OK");
      }
     
      p = p + 1 ;

    }
      if ((temp0 < consigne ) || (temp1 < consigne))                                  // si la température descend en dessous de ....
      {
        c++ ;
        //Serial.print("alarme");
        if (c==1)
        {
        sms.SendSMS(phoneToCall, "Alarme temp");
        //Serial.println("\nSMS sent OK");
        }
      }
      else if ((temp0 >= (consigne+1)) && (temp1 >= (consigne+1)) && ( c>1))                              // si la température revientà la normale
      {
        c=0 ;
        //Serial.print("Fin d'alarme");
        sms.SendSMS(phoneToCall, "Fin alarme");
        //Serial.println("\nSMS sent OK");
      }


       unsigned long Time = millis();
       if( ((Time - oldTime) > 60000) && (Vbat < 11) && ( d<1))    // si la batterie est trop faible
       {  
        d=1 ;
        oldTime = Time;   // enregistre quand a eu lieu le dernier changement d'état de la lampe 
        sms.SendSMS(phoneToCall, "Alarme Bat");     
        }



       else if ((Vbat>=11)&&(d==1))                                 // si la batterie revient à la normale
      {
        
        //Serial.print("Fin d'alarme");
        sms.SendSMS(phoneToCall, "Fin alarme");
        //Serial.println("\nSMS sent OK");
        d=0 ;
      }
 
 } 
}

void mesureU() 
{

  total = total - lectures[index];            // soustarction de la dernière lecture
  lectures[index] = analogRead(Vin);    // Lecture de la valeur analogique du capteur de courant                  
  total = total + lectures[index];              // ajout de la dernière lecture
  index = index + 1;                        // avance à la prochaine position du tableau       
  if (index >= nbreLect)                     // si fin du tableau ...   
  index = 0;                                    // ...retour au début  
  moyenne = total / nbreLect;                    // calcul de la moyenne
  delay(1);

 Vbat = (moyenne*15.13/1024);     //15v pour 1024 pts corrigé à 15.13v
V = ((Vbat-(int)Vbat)*100) ;
//Serial.println(Vin);
//Serial.println(Vbat);
}

