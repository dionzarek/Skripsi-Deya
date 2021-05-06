#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); 

#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
long duration, cm; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
//const int relayPin=10;


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode (10, OUTPUT);
 pinMode (13, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600);
}
void loop() {
  
  ////////////////RELAY/////////////////////////////////////////
  
  digitalWrite(13,HIGH);
  Serial.println ("HIGH");
  delay (5000);
  digitalWrite (13, LOW);
  Serial.println ("LOW");
  delay (5000);


  //////////////////////ULTRASONIC/////////////////////////////////
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;
  ///////////////////////////////////////////////////////////////

 lcd.setCursor(0,0);
 lcd.print(cm);
 lcd.print("cm");

  
 delay(250);
}
