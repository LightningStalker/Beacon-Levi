#include <EEPROM.h> // You must copy EEPROM directory from Arduino core
#include <DigiCDC.h>

#include "morsym.h" // ITU morse symbol table

/*
 * Output letters, numbers, symbols
 */
#define keyPin 1 // output for keying the transmitter
#define ledPin 1 // LED indicator
#define modePin 0 // jumper to enable USBserial message input mode
//#define DEBUG // uncomment for debugging

void symOut(int thisym, int rate) { // Translate an ASCII character into ITU morse and generate keying signal.
  int c = 0;
  uint8_t symval1, symval2;

  if(thisym == *" ") {
    delay(rate * 2); // special case " "
    return;
  }
  // letter number or symbol
  thisym = thisym - 0x20; // align ASCII with start of morse symbol table
  symval1 = pgm_read_byte(&sym[thisym]); // Reading from our symbol table array stored in flash
  symval2 = symval1;
  
  while(symval1 >>= 1) { // we must find the position inside the byte where the symbol starts
    c++; // move past initial zeros and high start bit to first signal
  }
  #ifdef DEBUG
    SerialUSB.println(thisym, HEX); // primitive serial debug
    SerialUSB.println(symval, BIN);
    SerialUSB.println(rate, DEC);
    SerialUSB.println(c, DEC);
  #endif
  for(c--; c >= 0; c--) {
    if ((symval2 >> c) & 1) {
      digitalWrite(keyPin, HIGH); // We'll just use the uino output functions here for now.
      delay(rate * 3);
      digitalWrite(keyPin, LOW); // for readability
    }
    else {
      digitalWrite(keyPin, HIGH); // since a need for speed and extremely accurate timing is not anticipated
      delay(rate);
      digitalWrite(keyPin, LOW);
    }
    delay(rate);
  }
}

 // Users input ASCII text that will be translated into the CW message
 // to be keyed and sent over the air.
 
void serialInputMode() { // We read the message from USBseraial and store it to EE.
  bool garbageday = true;
  int c = 0;
  char inchar;
  char charbuf;
  char num[2];

  // initialize the seareel vore input.
  digitalWrite(ledPin, HIGH); //light LED to indicate serial message input mode
  SerialUSB.begin();
  SerialUSB.delay(1000); // give it a little time to settle
  SerialUSB.println("Type ["); // hopefully we don't see one in the weirdness
  while(1) { // change it to something else if you have to
    if (SerialUSB.available()) {
      inchar = SerialUSB.read();
      if(garbageday) { // wait for "[" to start because of
        SerialUSB.println(".."); // wait for these dots to stop appearing before typing "["
        if(inchar == *"[") {
          garbageday = false; // this EXCELLENT SERIAL LIBRARY
          while((charbuf = EEPROM.read(c)) != 0x0) { // display current EE contents
            SerialUSB.write(charbuf);
            c++;
          }
          SerialUSB.println();
          c = 0;
          SerialUSB.println("wpm:"); // 0 = 5wpm, 1 = 10wpm, etc. see switch mintstate below
        }
      } // --> SHRUG <--
      else {
        if(inchar == *"]") {
          EEPROM.write(c, 0x0); // special case "]" end of input, message terminator = 0x0
          for(int t = 0; t < c; t++) SerialUSB.write(EEPROM.read(t)); // show confirmation of EE contents
          while(1); // I'm Cave Johnson and we're done here
          // since it is undefined in Int'l Morse Code
        }
        if(inchar > 0x60) inchar -= 0x20; // lower case is against the rules
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
SerialUSB.end(); // Now the user must remove the jumper and reset to verify. 
}

// the setup routine runs once when you press reset:
void setup() {
 // initialize the digital pin as an output.
  pinMode(keyPin, OUTPUT); // LED on Model A  or Pro
  pinMode(modePin, INPUT);  // Input for jumper
  digitalWrite(modePin, HIGH);  // with weak pullups
  if(!digitalRead(modePin)) { // Check for jumper presence 
    serialInputMode();
  }
  #ifdef DEBUG
  SerialUSB.begin(); // Uncomment above for serial debugging
  SerialUSB.delay(1000);
  #endif
} // keep going

// the loop routine runs over and over again forever:
void loop() { // Beacon Levi? Beacon?
  char thisym = 1;
  int c = 1;
  int rate;

  switch(EEPROM.read(0)) { // Read wpm from first byte of EE
    case '0':
      rate = 240; // 5wpm
      break;
    case '1':
      rate = 120; // 10wpm
      break;
    case '2':
      rate = 80; // 15wpm
      break;
    case '3':
      rate = 60; // 20wpm
      break;
    default:
      rate = 240;
      break;
  } // this saves 80 bytes of flash over a single divide instruction believe it or not
  
  for(int t = 0; t < 3; t++) { // 100ms pulses
    digitalWrite(keyPin, HIGH);
    delay(100);
    digitalWrite(keyPin, LOW);
    delay(1900);
  }
  
  while((thisym = EEPROM.read(c)) != 0x0) {
    symOut(thisym, rate); // until the 0x0 EOM terminator
    delay(rate * 2); // intercharacter space
    c++;
  }
  // perform the end-of-message thingies here
  delay(3000); // 3s dealie
}
