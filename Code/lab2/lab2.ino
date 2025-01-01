#include <Arduino.h>

// Create global variables. heach declaration is 4bytes
String string1; 
String string2;
String string3;

void printNum(float value, String& result) {
  String title = F("Here is a random number: ");
  String units = F("Because why not!");
  formatResult(title, units, value, result);
}
void formatResult(const String& title, const String& units, float value, String& result) {
  result = title;
  result += String(value, 1); //random to 1 decimal only
  result += units;
}


void setup() {
  Serial.begin(9600);
  for (int i = 10; i > 0; i--) {
    Serial.print(i); Serial.print(' ');
    delay(500);
  }
  Serial.println(F("Before reserve (heap memory):"));
  Serial.print(F("string1 heap address: "));
  Serial.println((uintptr_t)string1.c_str(), HEX);

  Serial.print(F("string2 heap address: "));
  Serial.println((uintptr_t)string2.c_str(), HEX);

  Serial.print(F("string3 heap address: "));
  Serial.println((uintptr_t)string3.c_str(), HEX);


  //explicitely research a buffer in memory for the strings. The reserve funtion looks at the first available memory space in the heap of the required size (32)
  string1.reserve(32);
  string2.reserve(32);
  if (!string3.reserve(32)) { // check the last largest reserve
    while (1) { // stop here and print repeating msg
      Serial.println(F("Strings out-of-memory"));
      delay(3000); // repeat msg every 3 sec
    }
  }
  Serial.println(F("After reserve (heap memory):"));
  Serial.print(F("string1 heap address: "));
  Serial.println((uintptr_t)string1.c_str(), HEX);

  Serial.print(F("string2 heap address: "));
  Serial.println((uintptr_t)string2.c_str(), HEX);

  Serial.print(F("string3 heap address: "));
  Serial.println((uintptr_t)string3.c_str(), HEX);
}


void loop() {
  // float randomN = random(100)/10;
  // printNum(randomN, string2);
  // Serial.println(string2);
  // Serial.println(F(" -- loop() returns --")); Serial.println();
} 