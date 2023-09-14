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
#include <EEPROM.h>

char eepromFlag[4] = {'T', 'T', 'E', 'X'};          // EEPROM location 0 to 3 should contain TTEX if we have stored steps.
const uint8_t eepromVersion = EEPROM_VERSION;       // Version of stored EEPROM data to invalidate stored steps if config changes.

// Function to retrieve step count from EEPROM.
// Looks for identifier "TTEX" at 0 to 3.
// Looks for version in 4.
// MSB -> LSB of steps stored in 5 - 8.
long getSteps() {
  char data[4];
  long eepromSteps;
  bool stepsSet = true;
  for (uint8_t i = 0; i < 4; i ++) {
    data[i] = EEPROM.read(i);
    if (data[i] != eepromFlag[i]) {
      stepsSet = false;
      break;
    }
  }
  uint8_t version = EEPROM.read(4);
  if (version != eepromVersion) {
    Serial.println(F("EEPROM version outdated, calibration required"));
    stepsSet = false;
  }
  if (stepsSet) {
    eepromSteps = ((long)EEPROM.read(5) << 24) + ((long)EEPROM.read(6) << 16) + ((long)EEPROM.read(7) << 8) + (long)EEPROM.read(8);
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
void writeEEPROM(long steps) {
  (void) EEPROM;
  for (uint8_t i = 0; i < 4; i++) {
    EEPROM.write(i, eepromFlag[i]);
  }
  EEPROM.write(4, eepromVersion);
  EEPROM.write(5, (steps >> 24) & 0xFF);
  EEPROM.write(6, (steps >> 16) & 0xFF);
  EEPROM.write(7, (steps >> 8) & 0xFF);
  EEPROM.write(8, steps & 0xFF);
}

// Function to clear step count and identifier from EEPROM.
void clearEEPROM() {
  for (uint8_t i = 0; i < 9; i++) {
    EEPROM.write(i, 0);
  }
}
