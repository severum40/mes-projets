

//***************************** Ecran 1602 i2c ************************************************
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// EXEMPLE 1602 i2c SUR http://blog.f8asb.com/2014/03/01/mise-en-oeuvre-i2c-vers-lcd-carte-chinoise-sur-arduino/
// EXEMPLE clavier SUR http://www.mon-club-elec.fr/pmwiki_mon_club_elec/pmwiki.php?n=MAIN.ArduinoExpertLCDClavierAppuiTouche

/*
** Example Arduino sketch for SainSmart I2C LCD Screen 16x2
** based on https://bitbucket.org/celem/sainsmart-i2c-lcd/src/3adf8e0d2443/sainlcdtest.ino
** This example uses F Malpartida's NewLiquidCrystal library. Obtain from:
** https://bitbucket.org/fmalpartida/new-liquidcrystal

** NOTE: Tested on Arduino Uno whose I2C pins are A4==SDA, A5==SCL

*/

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

const int C4 = 7; //declaration constante de broche
const int C3 = 6; //declaration constante de broche
const int C2 = 5; //declaration constante de broche
const int C1 = 4; //declaration constante de broche

const int L4 = 3; //declaration constante de broche
const int L3 = 2; //declaration constante de broche
const int L2 = 1; //declaration constante de broche
const int L1 = 0; //declaration constante de broche

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


//*****************************DTH22************************************************

#include "DHT.h"

DHT dht;
//*****************************Thermostat************************************************
int relais = 10 ;
float consigne = 21 ;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void setup()
{

  //***************************** Ecran 1602 i2c *************************************
 

  
  lcd.begin (16, 2); //  <<----- Mon LCD est un 16x2

  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
 

 
  lcd.home (); // go home -ramène le curseur à l'origine


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

dht.setup(8); // data pin 8


//*****************************Thermostat************************************************

pinMode(relais, OUTPUT);


}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void loop()
{
    delay(dht.getMinimumSamplingPeriod());
    
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    
 /* sensors.requestTemperatures(); // Envoi de la demande de température
  float temp0 = (sensors.getTempCByIndex(0));
  int t0 = (temp0 - (int)temp0) * 100 ;
  //Serial.println(temp0); // Le 0 corresponant au premier esclave trouvé .*/


  touche = clavier.getKey(); // lecture de la touche appuyée

  if (consigne >= temperature )digitalWrite (relais,HIGH);
  else digitalWrite (relais,LOW);


  switch (touche)
 
case "A" :
  
    lcd.clear(); // efface écran si appui # sinon  affiche touche
  

case "C" :
 
{
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Consigne :");
    lcd.print(consigne);
    lcd.write ((uint8_t) 0);//affiche le caractère de l'adresse 0
    lcd.print("C");
} 
   
/*  else if ((touche != NO_KEY) && (touche != '#')) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("touche: ");
    lcd.setCursor(9, 0);
    lcd.print(touche);

    delay(100); // pause entre 2 appuis
  } */
  
 
 case "B" :
  {
    lcd.clear();

    lcd.setCursor(0, 1);
    lcd.print("Humidite:  ");
    lcd.print(humidity, 1);
    lcd.print("%");
    delay(1000);
 
    lcd.setCursor(0, 1);
    lcd.print("temperat.:");
    lcd.print(temperature, 1);
    lcd.write ((uint8_t) 0);//affiche le caractère de l'adresse 0
    lcd.print("C");
     delay(1000);
 
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

int ramDispo() {
  int size = 2048; // Use 2048 with ATmega328, 8192 avec 2560
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL);

  free(buf);
  return size; // renvoie la taille de la RAM disponible
}
//
//
//
