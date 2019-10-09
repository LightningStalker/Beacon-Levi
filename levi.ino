#include <EEPROM.h>
#include <DigiCDC.h>

#include "levi.h"

/*
 * Output letters, numbers, symbols
 */
void symOut(int thisym) {
  if (sym[53] == *"V") digitalWrite(1, HIGH);
}

void serialInputMode() {
  bool garbageday = true;
  int c = 0;
  char inchar;
  char num[2];

  // initialize the seareel vore input.
  digitalWrite(1, OUTPUT); //light LED to indicate serial message input mode
  SerialUSB.begin();
  SerialUSB.delay(1000); // give it a little time to settle
  SerialUSB.println("Type ["); // hopefully we don't see one in the weirdness
  while(1) { // change it to something else if you have to
    if (SerialUSB.available()) {
      inchar = SerialUSB.read();
      if(garbageday) { // wait for "[" to start because of
        if(inchar == *"[") garbageday = false; // this EXCELLENT SERIAL LIBRARY
        SerialUSB.println(".."); // wait for these dots to stop appearing before typing "["
      } // --> SHRUG <--
      else {
        if(inchar == *"]") {
          EEPROM.write(c, 0x0); // special case "]" end of message
          for(int t = 0; t < c; t++) SerialUSB.write(EEPROM.read(t));
          while(1); // I'm Cave Johnson and we're done here
          // since it is undefined in Int'l Morse Code
        }
        if(inchar > 0x60) inchar = inchar - 0x20; // lower case is against the rules
        EEPROM.write(c, inchar);
        if(c < EEPROM.length())
        SerialUSB.write(utoa(c, num, 10));
        SerialUSB.write(":");
        SerialUSB.println(EEPROM.read(c));
        c++;
      }
    }
  SerialUSB.delay(100);
  }
SerialUSB.end();
}

// the setup routine runs once when you press reset:
void setup() {
 // initialize the digital pin as an output.
  pinMode(1, OUTPUT); //LED on Model A  or Pro
  pinMode(0, INPUT);  //Input for jumper
  digitalWrite(0, HIGH);  //with weak pullups
  if(!digitalRead(0)) {
    serialInputMode();
  }
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(1, HIGH);
  delay(1000);               // wait for a second
  digitalWrite(1, LOW); 
  delay(1000);               // wait for a second
  symOut(EEPROM.read(3));
}
