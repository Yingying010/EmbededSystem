#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

/*
  LED Toggle
  led-toggle.ino
  Use pushbutton switch to wake up Arduino for 5s

*/
 
// Define LED and switch connections
const byte ledPin = 13;
const byte buttonPin = 2; 
 

void wakeup(){
  Serial.println("Interrupt fired!");
  sleep_disable();
  digitalWrite(ledPin,HIGH);
  detachInterrupt(digitalPinToInterrupt(buttonPin));
  delay(5000);
}

void goingToSleep(){
  
  attachInterrupt(digitalPinToInterrupt(buttonPin),wakeup, FALLING); 
  /* Set the type of sleep mode we want. Can be one of (in order of power saving):
        SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
        SLEEP_MODE_ADC
        SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
        SLEEP_MODE_EXT_STANDBY
        SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
        SLEEP_MODE_PWR_DOWN (Deep sleep)
    */
   Serial.println("Going to sleep");
   sleep_enable();
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   digitalWrite(ledPin,LOW);
   delay(1000);
   sleep_cpu();
   Serial.println("Just woke up!");
}
 
void setup() {
  // Set LED pin as output
  pinMode(ledPin, OUTPUT);
  // Set switch pin as INPUT with pullup
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin,HIGH);
  // Setup Serial Monitor
  Serial.begin(9600);
  //Attach Interrupt to Interrupt Service Routine
}
 
void loop() {
  delay(5000);
  // Check switch
  goingToSleep();
}