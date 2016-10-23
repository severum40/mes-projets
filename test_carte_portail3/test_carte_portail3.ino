
// 
// simul portail 6_V2
// 
// Réalisé par Patrick OUKACI 



// Déclaration des broches  :___________________________________________________________________

int relais2 = 2 ; //relais 2
int relais1 = 3 ;  //relais 1
int lampe = 4 ; // Mosfet lampe flah
int moteur = 5 ; // Mosfet Moteur
int fdcD= 6 ; //fin de course droit
int fdcG = 7; //fin de course gauche
int cell = 8 ; // cellule 
int boutA = 9 ; //telecommade bouton A
int boutB = 10 ; //telecommade bouton B
int boutC = 11 ; //telecommade bouton C
int boutD = 12 ; //telecommade bouton D
int reed = A0 ;  // Capteur reed pour ralentissement moteur
int courant = A6 ; // Capteur de courant du moteur  ( 2,5V à 0 A )
int potar = A7 ; // potar pour réglage vitesse moteur ( gain )



// Constantes  : _________________________________________________________________________________

const unsigned long cycleClignot = 500;           // cycle de clignotement (milliseconds)
const unsigned long tempoArret = 5000;             // tempo d'arrêt entre ouvre et ferme
const unsigned long tempoAlarme = 2000;            // tempo d'alarme mesure courant 
const int coupMoteur = 200;                     // Nbre de points + ou - par rapport à 2.5 v (100 = 6.5A )pour déclencher alarmee courant 
const int nbreLect = 10;                   // Nombre de lecture du tableau de mesure courant ( SMOOTHING )

// Variables  :__________________________________________________________________________________


int etatlampe = LOW;                    // Etat initial dela lampe 
int gain = 0;                           // Gain pour PWM moteur
int maxgain = 0 ;                         // valeur maxi du gain réglée au potar


int alarme = 0 ;                         // alarme surveillance moteur
int refCourant ;                         // valeur de départ du capteur de courant 
int ecartCourant ;                       // écart entre valeur lue  et valeur départ

int lectures[nbreLect];              // Tableau de lectures de l'entrée analogique ( SMOOTHING )
int index = 0;                      // Index de la lecture en cours ( SMOOTHING )
int total = 0;                          // Total des lectures ( SMOOTHING )
int moyenne = 0;                        // Moyenne des lectures 

int unit = 0;                                 //variable comptage

unsigned long derniereLecturelampe = 0;       // Dernière lecture du temps  pour la lampe
unsigned long tempsArret = 0;             // temps écoulé tempo arret
unsigned long tempsAlarme = 0;            // temps écoulé tempo alarme
unsigned long derniereLecture = 0 ;







void setup() ////////////////////////////////////////////////////////////  SETUP  //////////////////////////////////////////////////////////////////
{
// TCCR0B = TCCR0B & 0b11111000 | 001 ;
 Serial.begin(9600);
 
 pinMode(2, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 
 pinMode(6, INPUT_PULLUP);
 pinMode(7, INPUT_PULLUP);
 pinMode(8, INPUT_PULLUP);
 
 pinMode(9, INPUT);
 pinMode(10, INPUT);
 pinMode(11, INPUT);
 pinMode(12, INPUT);
 
 pinMode(A0, INPUT_PULLUP);
 
 digitalWrite (2 , HIGH );
 digitalWrite (3 , HIGH );
 digitalWrite (4 , LOW ); 

  
  // test smoothing :
  
  // initialisation des lectures à 0:
  for (int cetteLecture = 0; cetteLecture < nbreLect; cetteLecture++)
    lectures[cetteLecture] = 0;
}

////////////////////////////////////////////////////////////  LOOP  /////////////////////////////////////////////////////////////////////

void loop() 
 {   

  maxgain = (1024-analogRead(potar))/4;               // 4 = 1024/255  pour commande moteur
  refCourant = analogRead(courant);       //Lecture de la valeur de référence du capteur de courant
 int etatboutA = digitalRead(boutA);      // Lecture de l'état du bouton A
 int etatboutB = digitalRead(boutB);    // Lecture de l'état du bouton B
 int etatboutC = digitalRead(boutC);      // Lecture de l'état du bouton C
 int etatboutD = digitalRead(boutD);    // Lecture de l'état du bouton D
 alarme=0;
 

	  
 if (etatboutA==HIGH) {
   retour :                                // retour si coupure de la cellule durant la fermeture 
    tempsAlarme = millis();

    while((digitalRead(fdcD)==LOW) && (alarme != 1) && (digitalRead(boutC)==LOW)&& (digitalRead(boutD)==LOW)) {
      
       // Serial.println (maxgain);      
       
/*Serial.print ("FDC G :");
Serial.println (digitalRead( fdcG ));       
Serial.print ("FDC D :");
Serial.println (digitalRead( fdcD ));
Serial.print ("boutA :");
Serial.println (digitalRead( boutA ));
Serial.print ("boutB :");
Serial.println (digitalRead( boutB ));
Serial.print ("boutC :");
Serial.println (digitalRead( boutC ));
Serial.print ("boutD :");
Serial.println (digitalRead( boutD ));
Serial.print ("cell :");
Serial.println (digitalRead( cell));
Serial.print ("reed :");
Serial.println (digitalRead( reed ));*/
Serial.print ("gain :");
Serial.println (gain);
Serial.print ("courant :");
Serial.println (analogRead( courant ));

Serial.println ("  ");

delay (100);
      
       ouvre() ;flash(); testCourant() ;


       /*Serial.println("  ");
       Serial.print("  Reference ...... ");
       Serial.println(refCourant);
       Serial.print("  Moyenne ........ ");
       Serial.println(moyenne);
       Serial.print("  alarmee  .........");
       Serial.println(alarme);
       Serial.print("  Ecart  ..........");
       Serial.println(ecartCourant);  */
    }
 
    tempsArret = millis();  
   
    while ((millis()-tempsArret <= tempoArret)&& (digitalRead(boutC)==LOW) && (digitalRead(boutD)==LOW)){ 
      arret() ; flash();
    }   
    
    tempsAlarme = millis();
    
    
    while((digitalRead(fdcG)==LOW) && (digitalRead(cell)==LOW)&& (alarme != 2)&& (digitalRead(boutC)==LOW)&& (digitalRead(boutD)==LOW)) { 
      
                // Serial.println (maxgain);      
       
/*Serial.print ("FDC G :");
Serial.println (digitalRead( fdcG ));       
Serial.print ("FDC D :");
Serial.println (digitalRead( fdcD ));
Serial.print ("boutA :");
Serial.println (digitalRead( boutA ));
Serial.print ("boutB :");
Serial.println (digitalRead( boutB ));
Serial.print ("boutC :");
Serial.println (digitalRead( boutC ));
Serial.print ("boutD :");
Serial.println (digitalRead( boutD ));
Serial.print ("cell :");
Serial.println (digitalRead( cell));
Serial.print ("reed :");
Serial.println (digitalRead( reed ));*/
Serial.print ("gain :");
Serial.println (gain);
Serial.print ("courant :");
Serial.println (analogRead( courant ));

Serial.println ("  ");

delay (100); 
       ferme() ; flash() ; testCourant() ; 

     /*Serial.println("  ");
       Serial.print("  Reference ...... ");
       Serial.println(refCourant);
       Serial.print("  Moyenne ........ ");
       Serial.println(moyenne);
       Serial.print("  alarmee  .........");
       Serial.println(alarme);
       Serial.print("  Ecart  ..........");
       Serial.println(ecartCourant);   */
    }    
 }
    
    
 else if ((etatboutB==HIGH) ) { 
   
       while((digitalRead(fdcG)==LOW) && (digitalRead(cell)==LOW)&& (alarme != 2)&& (digitalRead(boutC)==LOW)&& (digitalRead(boutD)==LOW)) { 
      ferme() ; flash() ; testCourant() ;
      }
 }
 
 else if  ((digitalRead(cell)==HIGH) && (digitalRead(fdcD)==LOW)&& (digitalRead(boutC)==LOW)&&  (digitalRead(boutD)==LOW)){
   arret () ;
   delay (1000);
 goto retour ;
 }
 
 else {
   	arret() ; digitalWrite(lampe, LOW);
 }
  
}
//////////////////////////////////////////////////////////////////////////  FONCTIONS /////////////////////////////////////////////////////////////////////////

void ouvre(){ 
  startMotor(); digitalWrite(relais1,HIGH); digitalWrite(relais2,LOW);}

//............................................................................................................................................................ 

void ferme(){
  startMotor(); digitalWrite(relais1,LOW); digitalWrite(relais2,HIGH);}

//............................................................................................................................................................ 

void arret(){
  analogWrite(moteur,0); digitalWrite(relais1,HIGH); digitalWrite(relais2,HIGH); moyenne = 0 ; alarme = 0 ; gain =0 ;} 

//............................................................................................................................................................ 

void  startMotor()  {   //   démarrage et arrêt progressif 

    unsigned long t1 = millis();
    
 if (t1 - derniereLecture >= 50){
      derniereLecture = t1;
     if (gain < 80){
        gain = 80 ; }
    if( gain <= maxgain ) {
      gain ++ ;
   
    analogWrite (moteur, gain);
  }
  }
 unsigned long t2 = millis();
    
 if (t2 - derniereLecture >= 50){
      derniereLecture = t2;
     if (gain > maxgain){
        gain = maxgain ; }
    if( gain <= maxgain ) {
      gain -- ;
   
    // if ( gain <= maxgain ) 
    analogWrite (moteur, gain);
  }
  }   
}

//............................................................................................................................................................

void flash() {            // clignotement de la lampe
 

  unsigned long currentMillis = millis();
  
  if(currentMillis - derniereLecturelampe >= cycleClignot) {
    derniereLecturelampe = currentMillis;   // enregistre quand a eu lieu le dernier changement d'état de la lampe 
    // si la lampe est allumée , on l'éteint et vice-versa :
    if (etatlampe == LOW)
      etatlampe = HIGH;
    else
      etatlampe = LOW;
    digitalWrite(lampe, etatlampe);  // Ecriture  la sortie de la lampe en fonction  de l'état etatlampe
  } 
 }  
 
 //............................................................................................................................................................
 
void testCourant() {

  total = total - lectures[index];            // soustarction de la dernière lecture
  lectures[index] = analogRead(courant);    // Lecture de la valeur analogique du capteur de courant                  
  total = total + lectures[index];              // ajout de la dernière lecture
  index = index + 1;                        // avance à la prochaine position du tableau       
  if (index >= nbreLect)                     // si fin du tableau ...   
  index = 0;                                    // ...retour au début  
  moyenne = total / nbreLect;                    // calcul de la moyenne
  delay(1);

  ecartCourant = ( moyenne - refCourant );
     
  int delai = millis() - tempsAlarme ;
     
        if ( delai >= tempoAlarme ) {

 if (ecartCourant >= coupMoteur)
 alarme = 1 ;
 
 else if (-ecartCourant >= coupMoteur)
 alarme = 2 ;  

 }   
}
// .........................................Comptage.......................................................................................... 
int compt(){
  if(unit < 2)
  {
  boolean mes = digitalRead(reed); // mesure du capteur
  delay(5);                        // Temps d'attente de 5ms
  boolean etat = mes;              // Retranscription de la mesure par HIGH (état haut (1) ) ou LOW (état bas(0) )
  if (reed == LOW)                 // Si l'état est bas
  {unit ++;                        // On rajoute +1 au nombre de passage unit++ == unit=unit+1
   delay(400);}                    // temps d'attente de 400ms pour ne pas créer de fausse mesures
  }
  else
  {unit = 0;}                      // Si il n'y a pas d'état bas alors on ne fait rien
}         
   
     


   
    
    
