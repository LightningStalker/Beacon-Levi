# Beacon Levi
Programmable ATtiny85 CW keyer

- Uses Digispark library
- Small form factor
- USB serial programmable
- DC 5-30V (With regulator on Digispark)
- Configurable 5-20 WPM+
- 510 character message storage (EEPROM size - 2)

## Usage
1. Program using Arduino IDE. You may need to install the Digispark libraries.
2. Copy the EEPROM directory from Arduino core to the Digispark core.
3. Start serial monitor
4. Jumper modePin to ground (default Digispark pin 0)
5. Plug in or reset Digispark
6. Unfortunately due to a bug in the software USB serial library, you must wait for fullstops and garbage characters to stop appearing. This may take over a minute in some cases.
7. Press the open bracket key '['
8. The 3 above steps may take several tries unfortunately. It should however start working. I would fix the affected libraries myself however I feel that is the job of the maintainers.
9. You should see a prompt for wpm. 1 is 5wpm, 2 is 10wpm and so on up to 20wpm. Other input is ignored and set to the default of 5wpm.
10. Type your alphanumeric message followed by the close bracket ']'
11. You should now see your entered information displayed for confirmation.
12. Reset, remove the jumper and your message should begin wandering out of keyPin as morse code and the LED on the Digispark should flash. Default is Digispark pin 1
13. Quince Johnny Adamsson
