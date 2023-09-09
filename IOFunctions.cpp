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

const unsigned long gearingFactor = STEPPER_GEARING_FACTOR;
const byte numChars = 20;
char serialInputChars[numChars];
bool newSerialData = false;
bool testCommandSent = false;
uint8_t testActivity = 0;
uint8_t testStepsMSB = 0;
uint8_t testStepsLSB = 0;

// Function to setup Wire library and functions
void setupWire() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

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
    int16_t steps = atoi(strtokIndex);
    strtokIndex = strtok(NULL," ");
    testActivity = atoi(strtokIndex);
    if (steps < 0) {
      Serial.println(F("Cannot provide a negative"));
    } else {
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
}

// Function to display the defined stepper motor config.
void displayTTEXConfig() {
  // Basic setup, display what this is.
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("License GPLv3 fsf.org (c) dcc-ex.com"));
  Serial.print(F("EX-Turntable version "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  if (fullTurnSteps == 0) {
    Serial.println(F("EX-Turntable has not been calibrated yet"));
  } else {
#ifdef FULL_STEP_COUNT    
    Serial.print(F("Manual override has been set for "));
#else
    Serial.print(F("EX-Turntable has been calibrated for "));
#endif
    Serial.print(fullTurnSteps);
    Serial.println(F(" steps per revolution"));
  }
  Serial.print(F("Gearing factor set to "));
  Serial.println(gearingFactor);
#if PHASE_SWITCHING == AUTO
  Serial.print(F("Automatic phase switching enabled at "));
  Serial.print(PHASE_SWITCH_ANGLE);
  Serial.println(F(" degrees"));
  Serial.print(F("Phase will switch at "));
  Serial.print(phaseSwitchStartSteps);
  Serial.print(F(" steps from home, and revert at "));
  Serial.print(phaseSwitchStopSteps);
  Serial.println(F(" steps from home"));
#else
  Serial.println(F("Manual phase switching enabled"));
#endif
#if TURNTABLE_EX_MODE == TRAVERSER
  Serial.println(F("EX-Turntable in TRAVERSER mode"));
#else
  Serial.println(F("EX-Turntable in TURNTABLE mode"));
#endif

#ifdef SENSOR_TESTING
// If in sensor testing mode, display this, don't enable stepper or I2C
  Serial.println(F("SENSOR TESTING ENABLED, EX-Turntable operations disabled"));
  Serial.print(F("Home/limit switch current state: "));
  Serial.print(homeSensorState);
  Serial.print(F("/"));
  Serial.println(limitSensorState);
  Serial.print(F("Debounce delay: "));
  Serial.println(DEBOUNCE_DELAY);
#else
  if (calibrating) {
    Serial.println(F("Calibrating..."));
  } else {
    Serial.println(F("Homing..."));
  }
#endif
}

// Function to define the action on a received I2C event.
void receiveEvent(int received) {
#ifdef DEBUG
  Serial.print(F("DEBUG: Received "));
  Serial.print(received);
  Serial.println(F(" bytes"));
#endif
  int16_t receivedSteps;
  long steps;  
  uint8_t activity;
  uint8_t receivedStepsMSB;
  uint8_t receivedStepsLSB;
  // We need 3 received bytes in order to care about what's received.
  if (received == 3) {
    // Get our 3 bytes of data, bit shift into steps.
    if (testCommandSent == true) {
      receivedStepsMSB = testStepsMSB;
      receivedStepsLSB = testStepsLSB;
      activity = testActivity;
      testCommandSent = false;
    } else {
      receivedStepsMSB = Wire.read();
      receivedStepsLSB = Wire.read();
      activity = Wire.read();
    }
#ifdef DEBUG
    Serial.print(F("DEBUG: stepsMSB:"));
    Serial.print(stepsMSB);
    Serial.print(F(", stepsLSB:"));
    Serial.print(stepsLSB);
    Serial.print(F(", activity:"));
    Serial.println(activity);
#endif
    receivedSteps = (receivedStepsMSB << 8) + receivedStepsLSB;
    steps = receivedSteps * gearingFactor;
    if (steps <= fullTurnSteps && activity < 2 && !stepper.isRunning() && !calibrating) {
      // Activities 0/1 require turning and setting phase, process only if stepper is not running.
#ifdef DEBUG
      Serial.print(F("DEBUG: Requested valid step move to: "));
      Serial.print(steps);
      Serial.print(F(" with phase switch: "));
      Serial.println(activity);
#endif
      moveToPosition(steps, activity);
    } else if (activity == 2 && !stepper.isRunning() && (!calibrating || homed == 2)) {
      // Activity 2 needs to reset our homed flag to initiate the homing process, only if stepper not running.
#ifdef DEBUG
      Serial.println(F("DEBUG: Requested to home"));
#endif
      initiateHoming();
    } else if (activity == 3 && !stepper.isRunning() && (!calibrating || homed == 2)) {
      // Activity 3 will initiate calibration sequence, only if stepper not running.
#ifdef DEBUG
      Serial.println(F("DEBUG: Calibration requested"));
#endif
      initiateCalibration();
    } else if (activity > 3 && activity < 8) {
      // Activities 4 through 7 set LED state.
#ifdef DEBUG
      Serial.print(F("DEBUG: Set LED state to: "));
      Serial.println(activity);
#endif
      setLEDActivity(activity);
    } else if (activity == 8) {
      // Activity 8 turns accessory pin on at any time.
#ifdef DEBUG
      Serial.println(F("DEBUG: Turn accessory pin on"));
#endif
      setAccessory(HIGH);
    } else if (activity == 9) {
      // Activity 9 turns accessory pin off at any time.
#ifdef DEBUG
      Serial.println(F("DEBUG: Turn accessory pin off"));
#endif
      setAccessory(LOW);
    } else {
#ifdef DEBUG
      Serial.print(F("DEBUG: Invalid step count or activity provided, or turntable still moving: "));
      Serial.print(steps);
      Serial.print(F(" steps, activity: "));
      Serial.println(activity);
#endif
    }
  } else {
  // Even if we have nothing to do, we need to read and discard all the bytes to avoid timeouts in the CS.
#ifdef DEBUG
    Serial.println(F("DEBUG: Incorrect number of bytes received, discarding"));
#endif
    while (Wire.available()) {
      Wire.read();
    }
  }
}

// Function to return the stepper status when requested by the IO_TurntableEX.h device driver.
// 0 = Finished moving to the correct position.
// 1 = Still moving.
void requestEvent() {
  uint8_t stepperStatus;
  if (stepper.isRunning()) {
    stepperStatus = 1;
  } else  {
    stepperStatus = 0;
  }
  Wire.write(stepperStatus);
}
