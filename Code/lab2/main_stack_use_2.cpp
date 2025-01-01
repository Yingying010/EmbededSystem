#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

/*
  Stack use example
  Understand stack working checking memory addresses on serial monitor
  Adapted from https://www.logiqbit.com/arduino-heap-vs-stack
*/

void function_three(void) {
  int more_data_still = 3;

    Serial.print(("address of more_data_still is: "));
    Serial.println((unsigned int)&more_data_still, HEX);
}

void function_two(void) {
   int some_more_data = 9;

    Serial.print(("address of some_more_data is: "));
    Serial.println((unsigned int)&some_more_data, HEX);
    
}

void function_one(void) {
    int some_data = 5;

    Serial.print(("address of some_data is: "));
    Serial.println((unsigned int)&some_data, HEX);

    function_two();
    function_three();
}

void setup() {
    Serial.begin(9600);
    function_one();
}

void loop(){

}