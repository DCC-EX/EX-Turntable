/*
 *  © 2023 Peter Cole
 *
 *  This file is part of EX-Turntable
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EX-Turntable.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "EEPROMFunctions.h"

// Function to retrieve step count from EEPROM.
// Looks for identifier "TTEX" at 0 to 3.
// MSB of steps stored in 4.
// LSB of steps stored in 5.
int16_t getSteps() {
  char data[4];
  int16_t eepromSteps;
  bool stepsSet = true;
  for (uint8_t i = 0; i < 4; i ++) {
    data[i] = EEPROM.read(i);
    if (data[i] != eepromFlag[i]) {
      stepsSet = false;
      break;
    }
  }
  if (stepsSet) {
    eepromSteps = (EEPROM.read(5) << 8) + EEPROM.read(6);
    if (eepromSteps <= sanitySteps) {
#ifdef DEBUG
      Serial.print(F("DEBUG: TTEX steps defined in EEPROM: "));
      Serial.println(eepromSteps);
#endif
      return eepromSteps;
    } else {
#ifdef DEBUG
      Serial.print(F("DEBUG: TTEX steps defined in EEPROM are invalid: "));
      Serial.println(eepromSteps);
#endif
      calibrating = true;
      return 0;
    }
  } else {
#ifdef DEBUG
    Serial.println(F("DEBUG: TTEX steps not defined in EEPROM"));
#endif
    calibrating = true;
    return 0;
  }
}

// Function to write step count with "TTEX" identifier to EEPROM.
void writeEEPROM(int16_t steps) {
  for (uint8_t i = 0; i < 4; i++) {
    EEPROM.write(i, eepromFlag[i]);
  }
  EEPROM.write(4, eepromVersion);
  EEPROM.write(5, steps >> 8);
  EEPROM.write(6, steps & 0xFF);
}

// Function to clear step count and identifier from EEPROM.
void clearEEPROM() {
  for (uint8_t i = 0; i < 6; i++) {
    EEPROM.write(i, 0);
  }
}
