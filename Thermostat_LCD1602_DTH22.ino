
// EXEMPLE 1602 i2c SUR http://blog.f8asb.com/2014/03/01/mise-en-oeuvre-i2c-vers-lcd-carte-chinoise-sur-arduino/
// EXEMPLE clavier SUR http://www.mon-club-elec.fr/pmwiki_mon_club_elec/pmwiki.php?n=MAIN.ArduinoExpertLCDClavierAppuiTouche

/*
** Example Arduino sketch for SainSmart I2C LCD Screen 16x2
** based on https://bitbucket.org/celem/sainsmart-i2c-lcd/src/3adf8e0d2443/sainlcdtest.ino
** This example uses F Malpartida's NewLiquidCrystal library. Obtain from:
** https://bitbucket.org/fmalpartida/new-liquidcrystal
** NOTE: Tested on Arduino Uno whose I2C pins are A4==SDA, A5==SCL

*/
//***************************** Ecran 1602 i2c **************************************************

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>


#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

//***************************** Caracteres spéciaux  ************************************************

byte myDegre[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

//**************************** Clavier ************************************************

#include <Keypad.h> // inclusion de la librairie pour clavier matriciel 

//--- Constantes utilisées avec le clavier 4x4
const byte LIGNES = 4; // 4 lignes
const byte COLONNES = 4; //4 colonnes

int n = 1;

// clavier

const int C4 = 9; //declaration constante de broche
const int C3 = 8; //declaration constante de broche
const int C2 = 7; //declaration constante de broche
const int C1 = 6; //declaration constante de broche

const int L4 = 5; //declaration constante de broche
const int L3 = 4; //declaration constante de broche
const int L2 = 3; //declaration constante de broche
const int L1 = 2; //declaration constante de broche

//--- Définition des touches
char touches[LIGNES][COLONNES] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// tableaux de lignes et colonnes
byte BrochesLignes[LIGNES] = {L1, L2, L3, L4}; //connexions utilisées pour les broches de lignes du clavier
byte BrochesColonnes[COLONNES] = {C1, C2, C3, C4}; //connexions utilisées pour les broches de colonnes du clavier

char touche; // variable de stockage valeur touche appuyée

// création d'un objet keypad = initialisation clavier
Keypad clavier = Keypad( makeKeymap(touches), BrochesLignes, BrochesColonnes, LIGNES, COLONNES );
// les broches de lignes sont automatiquement configurées en ENTREE avec pullup interne activé
// les broches de colonnes sont automatiquement configurées en SORTIE

//*****************************DS18B20************************************************
/*#include <OneWire.h>
  #include <DallasTemperature.h>

  // On utilise la Pin #7 pour connecter le DS18B20
  #define ONE_WIRE_BUS 10

  // Initialisation de la librairie OneWire
  OneWire oneWire(ONE_WIRE_BUS);

  // Lien avec la librairie Dallas Temperature Control
  DallasTemperature sensors(&oneWire);
  DeviceAddress tempDeviceAddress;

  const int  resolution = 12;

  #include <Stdio.h>*/

//*****************************DTH22**************************************************

#include "DHT.h"

DHT dht;
//*****************************Thermostat**********************************************
int relais = 12 ;
float consigne = 19;
float ecart = 0.5;
float consPlus;
float consMoins;

//*****************************DS3231**********************************************
//#include <Wire.h>     // existe déjà plus haut
#include <RTClib.h>

RTC_DS1307 RTC; // Avec cette librairie on déclare le DS3231 comme un DS1307

//********************************AUTRE ***********************************************
int ligne = 0;
int colonne = 11;
int saisie = 0;
int page = 0 ;
int front ;  // memoire de front montant
int front2 ;
char derniereTouche = 0;
unsigned long tempoArret = 5000;
unsigned long tempsArret = 0;             // temps écoulé tempo arret

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup()
{

  //***************************** Ecran 1602 i2c *************************************

  lcd.begin (16, 2); //  <<----- Mon LCD est un 16x2

  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);

  lcd.home (); // go home -ramène le curseur à l'origine

  //lcd.cursor() ;
  lcd.print(" Mon thermostat ");
  delay(1000);


  //***************************** Caracteres spéciaux  *********************************

  lcd.createChar(0, myDegre); //apprend le caractère à l'écran LCD

  //*****************************DS18B20************************************************

  /*//Serial.println("Prise de temperature avec  DS18B20");

    // On initialise la librairie DTC
    sensors.begin();
    sensors.getAddress(tempDeviceAddress, 0);
    sensors.setResolution(tempDeviceAddress, resolution);
  */

  //*****************************DTH22************************************************

  dht.setup(10); // data pin 10

  //*****************************Thermostat************************************************

  pinMode(relais, OUTPUT);

  Serial.begin(9600);

  //*****************************DS3231**********************************************

  Wire.begin();
  RTC.begin();
  //reglage de l'heure: décocher les 2 lignes suivantes pour effectuer le réglage(charger , recocher ,charger )
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  //DateTime now = RTC.now();

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void loop()
{
  //delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  /* sensors.requestTemperatures(); // Envoi de la demande de température DS18B20
    float temp0 = (sensors.getTempCByIndex(0));
    int t0 = (temp0 - (int)temp0) * 100 ;
    //Serial.println(temp0); // Le 0 corresponant au premier esclave trouvé .*/

  consPlus = (consigne + ecart);
  consMoins = (consigne - ecart);

  if (temperature < consMoins)digitalWrite (relais, HIGH);
  if (temperature > consPlus) digitalWrite (relais, LOW);

  touche = clavier.getKey(); // lecture de la touche appuyée

  if (touche == '*')  { //on efface le dernier chiffre

    Serial.print ("colonne: ");
    Serial.println (colonne);
    if (colonne > 9) {
      lcd.setCursor(colonne, 1);
      lcd.print(' ');
      colonne = colonne - 1;
      if (saisie > 0)   saisie = ((saisie - (int(derniereTouche) - 48)) / 10);
      tempsArret = millis();

    }
  }

  else if (touche == 'A') { //affichage humidité et  temperature actuelle
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidite:  ");
    lcd.print(humidity, 1);
    lcd.print("%");
    tempsArret = millis();
    page=2;
  }

  else if   ((touche == '0') || (touche == '1') || (touche == '2') || (touche == '3')
             || (touche == '4') || (touche == '5') || (touche == '6') || (touche == '7')
             || (touche == '8') || (touche == '9')) {               //  on a tapé un chiffre
    if (colonne < 11 )    {
      colonne = colonne + 1;
      lcd.setCursor(colonne, 1);
      lcd.print(touche);
      derniereTouche = touche;
      saisie = saisie * 10 + (int (touche) - 48);
      tempsArret = millis();

    }
  }

  else if   (touche == 'C') //Affichage de l'heure

  {
    lcd.clear();
    page=3;
   tempsArret = millis();
  } 
   
    

   
    
  

  else if ((saisie < 26) && (touche == '#'))
  { consigne = saisie ;
  }

  else if  ((millis() - tempsArret > tempoArret) /* || (front == 0) */|| (touche == 'B') ) //affichage consigne de temperature
  {
    lcd.clear();
    lcd.setCursor(0, 0) ;
    lcd.print("Temp:     ");
    lcd.print(temperature, 1);
    lcd.write ((uint8_t) 0);//affiche le caractère de l'adresse 0
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Cons:");
    lcd.setCursor(10, 1);
    if (consigne < 10) lcd.print("0");
    lcd.print(consigne, 1);
    lcd.write ((uint8_t) 0);//affiche le caractère de l'adresse 0
    lcd.print("C");
    tempsArret = millis();
    front = 1;
    page=1;
    
  }

while (page==3){
 DateTime now = RTC.now(); // Lit l'heure du module
    lcd.clear();
    lcd.setCursor(0, 0);
    if (now.day() < 10)lcd.print("0");
    lcd.print(now.day(), DEC); // Affiche le jour
    lcd.print('/');
    if (now.month() < 10)lcd.print("0");
    lcd.print(now.month(), DEC); // Affiche le mois
    lcd.print('/');
    lcd.print(now.year(), DEC);  //Affiche l'année
    lcd.setCursor(0, 1);
    if (now.hour() == 0)lcd.print("0");
    if (now.hour() < 10)lcd.print("0");
    lcd.print(now.hour(), DEC); //Affiche les heures
    lcd.print(':');
    //if (now.minute() == 0)lcd.print("0");
    if (now.minute() < 10)lcd.print("0");
    lcd.print(now.minute(), DEC); // Affiche les minutes
    lcd.print(':');
    if (now.minute() == 0)lcd.print("0");
    if (now.second() < 10)lcd.print("0");
    lcd.print(now.second(), DEC); // Affiche les secondes
    lcd.print("  /j= ");
    lcd.print(now.dayOfWeek(),DEC);
    //tempsArret = millis();
    delay(1000);
    break;
}
}

// --- Fin programme ---

//000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

// --- Mémo instructions ---

// var_int = digitalRead(Pin); //lit état de la broche indiquée et renvoie valeur HIGH ou LOW
// digitalWrite(pin, value) ; // met la valeur HIGH ou LOW sur la broche indiquée
// digitalRead(pin) ; // lit l'état d'une broche numérique en entrée et renvoie la valeur HIGH ou LOW présente sur la broche
// --- Impulsions ---
// analogWrite(broche,largeur); // génère une impulsion PWM sur la broche de largeur 0 (0%) à 255 (100%)

// tone(broche, frequence, +/-duree); // produit son de fréquence indiquée sur la broche
// noTone(broche); // stoppe la production de son sur la broche
// à noter : une seule broche peut générer un son à la fois...

// --- mémo boucle --
//for (int i=0; i <= 10; i++); // boucle simple
//delay(300); // pause en millisecondes

// ----- memo LCD ---
// LiquidCrystal(rs, enable, d4, d5, d6, d7) ; // initialisation 4 bits
// lcd.begin(cols, rows); // initialisation nombre colonne/ligne
//
// lcd.clear(); // efface écran et met le curseur en haut à gauche
// lcd.home(); // repositionne le curseur en haut et à gauche SANS effacer écran
//
// lcd.setCursor(col, row) ; // positionne le curseur à l'endroit voulu (colonne, ligne) (1ère=0 !)
// lcd.print("texte") ; // affiche la chaîne texte
//
// lcd.cursor() ; // affiche la ligne de base du curseur
// lcd.noCursor() ; // cache le curseur
// lcd.blink() ; // fait clignoter le curseur
// lcd.noBlink() ;// stoppe le clignotement du curseur
// lcd.noDisplay() ; // éteint le LCD sans modifier affichage
// lcd.display() ; // rallume le LCD sans modif affichage
//
// lcd.scrollDisplayLeft(); // décale l'affichage d'une colonne vers la gauche
// lcd.scrollDisplayRight(); // décale l'affichage d'une colonne vers la droite
// lcd.autoscroll() ; // les nouveaux caractères poussent les caractères déjà affichés
// noAutoscroll(); // stoppe le mode autoscroll

//---------- Mémo Clavier - librairie Keypad ---------
// var=char getKey(); // renvoie la valeur de la touche
// touche = clavier.getKey(); // lecture de la touche appuyée
//
// //---------------- fonction pour tester l'utilisation de la RAM --------
// this function will return the number of bytes currently free in RAM
// written by David A. Mellis
// based on code by Rob Faludi http://www.faludi.com
// Source : http://www.arduino.cc/playground/Code/AvailableMemory
//
/*int ramDispo() {
  int size = 2048; // Use 2048 with ATmega328, 8192 avec 2560
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL);

  free(buf);
  return size; // renvoie la taille de la RAM disponible
  }
*/
//
//
//
