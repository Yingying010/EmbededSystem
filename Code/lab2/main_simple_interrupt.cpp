#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

/*
  LED Toggle
  led-toggle.ino
  Use pushbutton switch to toggle LED
  Material adapted from 
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/
 
// Define LED and switch connections
const byte ledPin = 13;
const byte buttonPin = 2;
 
// Boolean to represent toggle state
 
volatile bool toggleState = false;
 
void checkSwitch() {
  // Check status of switch
  // Toggle LED if button pressed
  if (digitalRead(buttonPin) == LOW) {
    // Switch was pressed
    // Slight delay to debounce
    delay(200);
    // Change state of toggle
    toggleState = !toggleState;
    // Indicate state on LED
    digitalWrite(ledPin,toggleState);
  }
}
 
void setup() {
  // Set LED pin as output
  pinMode(ledPin, OUTPUT);
  // Set switch pin as INPUT with pullup
  pinMode(buttonPin, INPUT_PULLUP);
  // Setup Serial Monitor
  //Serial.begin(9600);
  //Attach Interrupt to Interrupt Service Routine
  //attachInterrupt(digitalPinToInterrupt(buttonPin),checkSwitch, FALLING); 
}
 
void loop() {
  
  // Check switch
  checkSwitch();
  // Add a 5-second time delay
  //Serial.println("Delay Started");
  //delay(5000);
  //Serial.println("Delay Finished");
  //Serial.println("..............");
  
}