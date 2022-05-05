/*
 *  © 2022 Peter Cole
 *
 *  This file is part of Turntable-EX
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
 *  along with Turntable-EX.  If not, see <https://www.gnu.org/licenses/>.
*/

// Include required libraries.
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "AccelStepper.h"

// Include standard stepper definitions, version history, and position definitions.
#include "standard_steppers.h"
#include "version.h"

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

// Define global variables here.
#ifndef SANITY_STEPS
#define SANITY_STEPS 10000                          // Define sanity steps if not in config.h.
#endif

#ifndef HOME_SENSITIVITY
#define HOME_SENSITIVITY 150                        // Define homing sensitivity if not in config.h.
#endif
bool lastRunningState;                              // Stores last running state to allow turning the stepper off after moves.
int16_t fullTurnSteps;                              // Assign our defined full turn steps from config.h.
int16_t halfTurnSteps;                              // Defines a half turn to enable moving the least distance.
const int16_t sanitySteps = SANITY_STEPS;           // Define an arbitrary number of steps to prevent indefinite spinning if homing/calibrations fails.
const int16_t homeSensitivity = HOME_SENSITIVITY;   // Define the minimum number of steps required before homing sensor deactivates.
int16_t lastStep = 0;                               // Holds the last step value we moved to (enables least distance moves).
int16_t lastTarget = sanitySteps;                   // Holds the last step target (prevents continuous rotatins if homing fails).
uint8_t homed = 0;                                  // Flag to indicate homing state: 0 = not homed, 1 = homed, 2 = failed.
const uint8_t homeSensorPin = 5;                    // Define pin 5 for the home sensor.
const uint8_t relay1Pin = 3;                        // Control pin for relay 1.
const uint8_t relay2Pin = 4;                        // Control pin for relay 2.
const uint8_t ledPin = 6;                           // Pin for LED output.
const uint8_t accPin = 7;                           // Pin for accessory output.
uint8_t ledState = 7;                               // Flag for the LED state: 4 on, 5 slow, 6 fast, 7 off.
bool ledOutput = LOW;                               // Boolean for the actual state of the output LED pin.
unsigned long ledMillis = 0;                        // Required for non blocking LED blink rate timing.
bool calibrating = false;                           // Flag to prevent other rotation activities during calibration.
uint8_t calibrationPhase = 0;                       // Flag for calibration phase.
unsigned long calMillis = 0;                        // Required for non blocking calibration pauses.
char eepromFlag[4] = {'T', 'T', 'E', 'X'};          // EEPROM location 0 to 3 should contain TTEX if we have stored steps.
uint8_t eepromVersion = 1;                          // Version of stored EEPROM data.

AccelStepper stepper = STEPPER_DRIVER;

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

// Function to display the defined stepper motor config.
void displayTTEXConfig() {
  if (fullTurnSteps == 0) {
    Serial.println(F("Turntable-EX has not been calibrated yet"));
  } else {
    Serial.print(F("Turntable-EX has been calibrated for "));
    Serial.print(fullTurnSteps);
    Serial.println(F(" steps per revolution"));
  }
}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
}

// Function to find the home position.
void moveHome() {
  if (digitalRead(homeSensorPin) == HOME_SENSOR_ACTIVE_STATE) {
    stepper.stop();
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
    stepper.setCurrentPosition(0);
    lastStep = 0;
    homed = 1;
    Serial.println(F("Turntable homed successfully"));
#ifdef DEBUG
    Serial.print(F("DEBUG: Stored values for lastStep/lastTarget: "));
    Serial.print(lastStep);
    Serial.print(F("/"));
    Serial.println(lastTarget);
#endif
  } else if(!stepper.isRunning()) {
#ifdef DEBUG
    Serial.print(F("DEBUG: Recorded/last actual target: "));
    Serial.print(lastTarget);
    Serial.print(F("/"));
    Serial.println(stepper.targetPosition());
#endif
    if (stepper.targetPosition() == lastTarget) {
      stepper.setCurrentPosition(0);
      lastStep = 0;
      homed = 2;
      Serial.println(F("ERROR: Turntable failed to home, setting random home position"));
    } else {
      stepper.enableOutputs();
      stepper.move(sanitySteps);
      lastTarget = stepper.targetPosition();
#ifdef DEBUG
      Serial.print(F("DEBUG: lastTarget: "));
      Serial.println(lastTarget);
#endif
      Serial.println(F("Homing started"));
    }
  }
}

// Function to define the action on a received I2C event.
void receiveEvent(int received) {
#ifdef DEBUG
  Serial.print(F("DEBUG: Received "));
  Serial.print(received);
  Serial.println(F(" bytes"));
#endif
  int16_t steps;  
  uint8_t activity;
  // We need 3 received bytes in order to care about what's received.
  if (received == 3) {
    // Get our 3 bytes of data, bit shift into steps.
    uint8_t stepsMSB = Wire.read();
    uint8_t stepsLSB = Wire.read();
    activity = Wire.read();
#ifdef DEBUG
    Serial.print(F("DEBUG: stepsMSB:"));
    Serial.print(stepsMSB);
    Serial.print(F(", stepsLSB:"));
    Serial.print(stepsLSB);
    Serial.print(F(", activity:"));
    Serial.println(activity);
#endif
    steps = (stepsMSB << 8) + stepsLSB;
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
      homed = 0;
      lastTarget = sanitySteps;
    } else if (activity == 3 && !stepper.isRunning() && (!calibrating || homed == 2)) {
      // Activity 3 will initiate calibration sequence, only if stepper not running.
#ifdef DEBUG
      Serial.println(F("DEBUG: Calibration requested"));
#endif
      calibrating = true;
      homed = 0;
      lastTarget = sanitySteps;
      clearEEPROM();
    } else if (activity > 3 && activity < 8) {
      // Activities 4 through 7 set LED state.
#ifdef DEBUG
      Serial.print(F("DEBUG: Set LED state to: "));
      Serial.println(activity);
#endif
      ledState = activity;
    } else if (activity == 8) {
      // Activity 8 turns accessory pin on at any time.
#ifdef DEBUG
      Serial.println(F("DEBUG: Turn accessory pin on"));
#endif
      digitalWrite(accPin, HIGH);
    } else if (activity == 9) {
      // Activity 9 turns accessory pin off at any time.
#ifdef DEBUG
      Serial.println(F("DEBUG: Turn accessory pin off"));
#endif
      digitalWrite(accPin, LOW);
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

// Function to move to the indicated position.
void moveToPosition(int16_t steps, uint8_t phaseSwitch) {
  if (steps != lastStep) {
    Serial.print(F("Received notification to move to step postion "));
    Serial.println(steps);
    int16_t moveSteps;
    Serial.print(F("Position steps: "));
    Serial.print(steps);
    Serial.print(F(", Phase switch flag: "));
    Serial.print(phaseSwitch);
    if ((steps - lastStep) > halfTurnSteps) {
      moveSteps = steps - fullTurnSteps - lastStep;
    } else if ((steps - lastStep) < -halfTurnSteps) {
      moveSteps = fullTurnSteps - lastStep + steps;
    } else {
      moveSteps = steps - lastStep;
    }
    Serial.print(F(" - moving "));
    Serial.print(moveSteps);
    Serial.println(F(" steps"));
    Serial.print(F("Setting phase switch flag to: "));
    Serial.println(phaseSwitch);
    setPhase(phaseSwitch);
    lastStep = steps;
    stepper.enableOutputs();
    stepper.move(moveSteps);
    lastTarget = stepper.targetPosition();
#ifdef DEBUG
    Serial.print(F("DEBUG: Stored values for lastStep/lastTarget: "));
    Serial.print(lastStep);
    Serial.print(F("/"));
    Serial.println(lastTarget);
#endif
  }
}

// If phase switching is enabled, function to set it.
void setPhase(uint8_t phase) {
#if RELAY_ACTIVE_STATE == HIGH
  digitalWrite(relay1Pin, phase);
  digitalWrite(relay2Pin, phase);
#elif RELAY_ACTIVE_STATE == LOW
  digitalWrite(relay1Pin, !phase);
  digitalWrite(relay2Pin, !phase);
#endif
}

// Function to set/maintain our LED state for on/blink/off.
// 4 = on, 5 = slow blink, 6 = fast blink, 7 = off.
void processLED() {
  uint16_t currentMillis = millis();
  if (ledState == 4 ) {
    ledOutput = 1;
  } else if (ledState == 7) {
    ledOutput = 0;
  } else if (ledState == 5 && currentMillis - ledMillis >= LED_SLOW) {
    ledOutput = !ledOutput;
    ledMillis = currentMillis;
  } else if (ledState == 6 && currentMillis - ledMillis >= LED_FAST) {
    ledOutput = !ledOutput;
    ledMillis = currentMillis;
  }
  digitalWrite(ledPin, ledOutput);
}

// The calibration function is used to determine the number of steps required for a single 360 degree rotation.
// This should only be trigged when either there are no stored steps in EEPROM, the stored steps are invalid,
// or the calibration command has been initiated by the CommandStation.
// Logic:
// - Move away from home if already homed and erase EEPROM.
// - Perform initial home rotation, set to 0 steps when homed.
// - Perform second home rotation, set steps to currentPosition().
// - Write steps to EEPROM.
void calibration() {
  setPhase(0);
  if (calibrationPhase == 2 && digitalRead(homeSensorPin) == HOME_SENSOR_ACTIVE_STATE && stepper.currentPosition() > homeSensitivity) {
    stepper.stop();
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
    fullTurnSteps = stepper.currentPosition();
    halfTurnSteps = fullTurnSteps / 2;
    calibrating = false;
    calibrationPhase = 0;
    writeEEPROM(fullTurnSteps);
    Serial.print(F("CALIBRATION: Completed, storing full turn step count: "));
    Serial.println(fullTurnSteps);
    homed = 0;
    lastTarget = sanitySteps;
  } else if (calibrationPhase == 1 && lastStep == sanitySteps && digitalRead(homeSensorPin) == HOME_SENSOR_ACTIVE_STATE && stepper.currentPosition() > homeSensitivity) {
    Serial.println(F("CALIBRATION: Phase 2, counting full turn steps..."));
    stepper.stop();
    stepper.setCurrentPosition(0);
    calibrationPhase = 2;
    stepper.enableOutputs();
    stepper.moveTo(sanitySteps);
    lastStep = sanitySteps;
  } else if (calibrationPhase == 0 && !stepper.isRunning() && homed == 1) {
    Serial.println(F("CALIBRATION: Phase 1, homing..."));
    calibrationPhase = 1;
    stepper.enableOutputs();
    stepper.moveTo(sanitySteps);
    lastStep = sanitySteps;
  } else if ((calibrationPhase == 2 || calibrationPhase == 1) && !stepper.isRunning() && stepper.currentPosition() == sanitySteps) {
    Serial.println(F("CALIBRATION: FAILED, could not home, could not determine step count"));
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
    calibrating = false;
    calibrationPhase = 0;
  }
}

void setup() {
// Basic setup, display what this is.
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("License GPLv3 fsf.org (c) dcc-ex.com"));
  Serial.print(F("Turntable-EX version "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);

// Configure homing sensor pin
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(homeSensorPin, INPUT_PULLUP);
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(homeSensorPin, INPUT);
#endif

// Configure relay output pins
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

// Ensure relays are inactive on startup
  setPhase(0);

// Configure LED and accessory output pins
  pinMode(ledPin, OUTPUT);
  pinMode(accPin, OUTPUT);

// Read steps from EEPROM
  fullTurnSteps = getSteps();
  halfTurnSteps = fullTurnSteps / 2;

// Display the configured stepper details
  displayTTEXConfig();

// Set up the stepper driver
  setupStepperDriver();

// Now we're ready, set up I2C.
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  if (calibrating) {
    Serial.println(F("Calibrating..."));
  } else {
    Serial.println(F("Homing..."));
  }
}

void loop() {
// If we haven't successfully homed yet, do it.
  if (homed == 0) {
    moveHome();
  }

// If flag is set for calibrating, do it.
  if (calibrating) {
    calibration();
  }

// Process the stepper object continuously.
  stepper.run();

// Process our LED.
  processLED();

// If disabling on idle is enabled, disable the stepper.
#if defined(DISABLE_OUTPUTS_IDLE)
  if (stepper.isRunning() != lastRunningState) {
    lastRunningState = stepper.isRunning();
    if (!lastRunningState) {
      stepper.disableOutputs();
    }
  }
#endif
}
