#include <ezButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); 
ezButton button (12); // Flush Button
#define echoPin 8 //ultrasonic echo pin
#define trigPin 9 //ultrasonic trigger pin

long duration, cm; //Ultrasonic duration variable
int distance; //distance variable
int inPin = 12; // pin Flush Button
int buttonVal = 0; // flush button value

int state=1;

/////////////////////PARAMETER FLOW METER///////////////////////
byte statusLed    = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2 for interrupt on UNO
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;   //Flow characteristic: F=11*L/min

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

//////////////////////////////////////////////////////////////

void setup() {
    // INIT LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Wire.setClock(10000); //set lower scan speed

  // Initiate Ultra Sonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(38400);

  // INIT RELAY
  pinMode (5, OUTPUT);

  //INIT FLUSH BUTTON
  pinMode(inPin, INPUT);

  lcd.print("LOADING...");
  delay (3000);
  lcd.setCursor (0,0);
  lcd.print("AUTO VALVE");
  lcd.setCursor (0,1);
  lcd.print("BY:DEYAPARINATA");
  delay(3000);
  lcd.clear();

  lcd.setCursor (0,0);
  lcd.print("NRP:");
  lcd.setCursor (0,1);
  lcd.print("143020360");
  delay(3000);
  lcd.clear();
  

  //INIT FLOW METER
   pinMode (8, OUTPUT);
   digitalWrite(8,LOW);
   Serial.println ("Low");
   
  // Initialize a serial connection for reporting values to the host
   Serial.begin(38400);
   
  // Set up the status LED line as an output
   pinMode(statusLed, OUTPUT);
   digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
   pinMode(sensorPin, INPUT);
   digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void ultraSonic()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;
  Serial.println(cm);
  lcd.setCursor(0,0);
}


void flowMeter()
{
  ///////////////////////////////////////FLOWMETER/////////////////////////////////
    if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
      detachInterrupt(sensorInterrupt);
      
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
      oldTime = millis();
    
     flowMilliLitres = (flowRate / 60) * 1000;
    
     // Add the millilitres passed in this second to the cumulative total
     totalMilliLitres += flowMilliLitres;
      
      unsigned int frac;
    
      // Print the flow rate for this second in litres / minute
      Serial.print("Flow rate: ");
      Serial.print(int(flowRate));  
      Serial.print(".");             // Print the decimal point
      // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
      frac = (flowRate - int(flowRate)) * 10;
      Serial.print(frac, DEC) ;      // Print the fractional part of the variable
      Serial.print("L/min");
      // Print the number of litres flowed in this second
      Serial.print("  Current Liquid Flowing: ");
      Serial.print(flowMilliLitres);
      Serial.print("mL/Sec");

      // Print the cumulative total of litres flowed since starting
      Serial.print("  Output Liquid Quantity: ");
      Serial.print(totalMilliLitres);
      Serial.println("mL"); 

      // Reset the pulse counter so we can start incrementing again
      pulseCount = 0;
    
      // Enable the interrupt again now that we've finished sending output
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

}

void loop() {


  switch (state){
    case 1:
    Serial.println("State 1");
    lcd.setCursor (0,0);
    lcd.print("WAITING FOR");
    lcd.setCursor (0,1);
    lcd.print("BOTTLE");
    
    button.loop(); //EZButton Library       //
    if (button.isReleased()){               //
      state=7;                              //
    }                                       //
    
    ultraSonic();
      if (cm<10){
      //totalMilliLitres=0;
      state=2;
      
      
    }
    break;

    case 2:
    Serial.println("State 2");
    digitalWrite(8,HIGH);
    lcd.clear();
    lcd.print("FILLING...");
    state=3;
    
    break;

    case 3:
    Serial.println("State 3");
    
    if (totalMilliLitres > 200){
        state=4;
      } else{
          flowMeter();
      }
     
    break;

    case 4:
    Serial.println("State 4");
    lcd.clear();
    digitalWrite(8,LOW);
    state=5;
    break;

    case 5:
    Serial.println("State 5");
    ultraSonic();
      if (cm > 30){
      totalMilliLitres=0;
      state=6;
      }else{
        lcd.print("REMOVE BOTTLE");
        
      }
      
    break;
    
    case 6:
    Serial.println("State 6");
    lcd.clear();
    state=1;
    break;

    case 7:
    Serial.println("State 7");
    lcd.clear();
    lcd.print("FLUSHING...");

    if(flowMilliLitres > 3){
      state = 8;
    }else{
    flowMeter();
    digitalWrite(8,HIGH);
    }
    break;

    case 8:
    Serial.println("State 8");
    if(flowMilliLitres < 1){
      digitalWrite(8,LOW);
      state=1;
    }else{
    flowMeter();
    }
  }
  delay(1);
   
}
