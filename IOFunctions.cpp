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

#include "IOFunctions.h"

// Function to read and process serial input for valid test commands
void processSerialInput() {
  static bool serialInProgress = false;
  static byte serialIndex = 0;
  char startMarker = '<';
  char endMarker = '>';
  char serialChar;
  while (Serial.available() > 0 && newSerialData == false) {
    serialChar = Serial.read();
    if (serialInProgress == true) {
      if (serialChar != endMarker) {
        serialInputChars[serialIndex] = serialChar;
        serialIndex++;
        if (serialIndex >= numChars) {
          serialIndex = numChars - 1;
        }
      } else {
        serialInputChars[serialIndex] = '\0';
        serialInProgress = false;
        serialIndex = 0;
        newSerialData = true;
      }
    } else if (serialChar == startMarker) {
      serialInProgress = true;
    }
  }
  if (newSerialData == true) {
    Serial.print(F("Received serial input: "));
    Serial.println(serialInputChars);
    newSerialData = false;
    char * strtokIndex;
    strtokIndex = strtok(serialInputChars," ");
    uint16_t steps = atoi(strtokIndex);
    strtokIndex = strtok(NULL," ");
    testActivity = atoi(strtokIndex);
    Serial.print(F("Test move "));
    Serial.print(steps);
    Serial.print(F(" steps, activity ID "));
    Serial.println(testActivity);
    testStepsMSB = steps >> 8;
    testStepsLSB = steps & 0xFF;
    testCommandSent = true;
    receiveEvent(3);
  }
}
