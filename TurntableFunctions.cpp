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

/*=============================================================
 * This file contains all functions pertinent to turntable
 * operation including stepper movements, relay phase switching,
 * and LED/accessory related functions.
=============================================================*/

#include "TurntableFunctions.h"
#include "EEPROMFunctions.h"
#include "IOFunctions.h"

const long sanitySteps = SANITY_STEPS;              // Define an arbitrary number of steps to prevent indefinite spinning if homing/calibrations fails.
const uint8_t limitSensorPin = 2;                   // Define pin 2 for the traverser mode limit sensor.
const uint8_t homeSensorPin = 5;                    // Define pin 5 for the home sensor.
const uint8_t relay1Pin = 3;                        // Control pin for relay 1.
const uint8_t relay2Pin = 4;                        // Control pin for relay 2.
const uint8_t ledPin = 6;                           // Pin for LED output.
const uint8_t accPin = 7;                           // Pin for accessory output.
const long homeSensitivity = HOME_SENSITIVITY;      // Define the minimum number of steps required before homing sensor deactivates.

long lastStep = 0;                                  // Holds the last step value we moved to (enables least distance moves).
uint8_t homed = 0;                                  // Flag to indicate homing state: 0 = not homed, 1 = homed, 2 = failed.
long fullTurnSteps;                                 // Assign our defined full turn steps from config.h.
long halfTurnSteps;                                 // Defines a half turn to enable moving the least distance.
long phaseSwitchStartSteps;                         // Defines the step count at which phase should automatically invert.
long phaseSwitchStopSteps;                          // Defines the step count at which phase should automatically revert.
long lastTarget = sanitySteps;                      // Holds the last step target (prevents continuous rotatins if homing fails).
uint8_t ledState = 7;                               // Flag for the LED state: 4 on, 5 slow, 6 fast, 7 off.
bool ledOutput = LOW;                               // Boolean for the actual state of the output LED pin.
unsigned long ledMillis = 0;                        // Required for non blocking LED blink rate timing.
bool calibrating = false;                           // Flag to prevent other rotation activities during calibration.
uint8_t calibrationPhase = 0;                       // Flag for calibration phase.
unsigned long calMillis = 0;                        // Required for non blocking calibration pauses.
bool homeSensorState;                               // Stores the current home sensor state.
bool limitSensorState;                              // Stores the current limit sensor state.
bool lastHomeSensorState;                           // Stores the last home sensor state.
bool lastLimitSensorState;                          // Stores the last limit sensor state.
unsigned long lastLimitDebounce = 0;                // Stores the last time the limit sensor switched for debouncing.
unsigned long lastHomeDebounce = 0;                 // Stores the last time the home sensor switched for debouncing.

AccelStepper stepper = STEPPER_DRIVER;

// Function configure sensor pins
void startupConfiguration() {
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(homeSensorPin, INPUT_PULLUP);
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(homeSensorPin, INPUT);
#endif
#if TURNTABLE_EX_MODE == TRAVERSER
// Configure limit sensor pin in traverser mode
#if LIMIT_SENSOR_ACTIVE_STATE == LOW
  pinMode(limitSensorPin, INPUT_PULLUP);
#elif LIMIT_SENSOR_ACTIVE_STATE == HIGH
  pinMode(limitSensorPin, INPUT);
#endif
#endif
// Get the current sensor state
  lastHomeSensorState = digitalRead(homeSensorPin);
  homeSensorState = getHomeState();
#if TURNTABLE_EX_MODE == TRAVERSER
  lastLimitSensorState = digitalRead(limitSensorPin);
  limitSensorState = getLimitState();
#endif

// Configure relay output pins
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

// Ensure relays are inactive on startup
  setPhase(0);

// Configure LED and accessory output pins
  pinMode(ledPin, OUTPUT);
  pinMode(accPin, OUTPUT);

// If step count explicitly defined, use that
#ifdef FULL_STEP_COUNT
  fullTurnSteps = FULL_STEP_COUNT;
#else
// Else read steps from EEPROM
  fullTurnSteps = getSteps();
#endif
  halfTurnSteps = fullTurnSteps / 2;

#if PHASE_SWITCHING == AUTO
// Calculate phase invert/revert steps
  processAutoPhaseSwitch();
#endif
}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
}

// Function to find the home position.
void moveHome() {
  setPhase(0);
  if (getHomeState() == HOME_SENSOR_ACTIVE_STATE) {
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

// Function to move to the indicated position.
void moveToPosition(long steps, uint8_t phaseSwitch) {
  if (steps != lastStep) {
    Serial.print(F("Received notification to move to step postion "));
    Serial.println(steps);
    int16_t moveSteps;
    Serial.print(F("Position steps: "));
    Serial.print(steps);
#if PHASE_SWITCHING == AUTO
    Serial.print(F(", Auto phase switch"));
#else
    Serial.print(F(", Phase switch flag: "));
    Serial.print(phaseSwitch);
#endif
#if TURNTABLE_EX_MODE == TRAVERSER
// If we're in traverser mode, very simple logic, negative move to limit, positive move to home.
    moveSteps = lastStep - steps;
#else
    if ((steps - lastStep) > halfTurnSteps) {
      moveSteps = steps - fullTurnSteps - lastStep;
    } else if ((steps - lastStep) < -halfTurnSteps) {
      moveSteps = fullTurnSteps - lastStep + steps;
    } else {
      moveSteps = steps - lastStep;
    }
#endif
    Serial.print(F(" - moving "));
    Serial.print(moveSteps);
    Serial.println(F(" steps"));
#if PHASE_SWITCHING == AUTO
    if ((steps >= 0 && steps < phaseSwitchStartSteps) || (steps <= fullTurnSteps && steps >= phaseSwitchStopSteps)) {
      phaseSwitch = 0;
    } else {
      phaseSwitch = 1;
    }
#endif
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

// Function to set phase.
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

// The calibration function is used to determine the number of steps required for a single 360 degree rotation,
// or, in traverser mode, the steps between the home and limit switches.
// This should only be trigged when either there are no stored steps in EEPROM, the stored steps are invalid,
// or the calibration command has been initiated by the CommandStation.
// Logic:
// - Move away from home if already homed and erase EEPROM.
// - Perform initial home rotation, set to 0 steps when homed.
// - Perform second home rotation, set steps to currentPosition().
// - Write steps to EEPROM.
void calibration() {
  setPhase(0);
#if TURNTABLE_EX_MODE == TRAVERSER
  if (calibrationPhase == 3 && getLimitState() != LIMIT_SENSOR_ACTIVE_STATE) {
#else
  if (calibrationPhase == 2 && getHomeState() == HOME_SENSOR_ACTIVE_STATE && stepper.currentPosition() > homeSensitivity) {
#endif
    stepper.stop();
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
    fullTurnSteps = stepper.currentPosition();
    if (fullTurnSteps < 0) {
      fullTurnSteps = -fullTurnSteps;
    }
    halfTurnSteps = fullTurnSteps / 2;
#if PHASE_SWITCHING == AUTO
    processAutoPhaseSwitch();
#endif
    calibrating = false;
    calibrationPhase = 0;
    writeEEPROM(fullTurnSteps);
    Serial.print(F("CALIBRATION: Completed, storing full turn step count: "));
    Serial.println(fullTurnSteps);
    stepper.setCurrentPosition(stepper.currentPosition());
    homed = 0;
    lastTarget = sanitySteps;
    displayTTEXConfig();
#if TURNTABLE_EX_MODE == TRAVERSER
  } else if (calibrationPhase == 2 && getLimitState() == LIMIT_SENSOR_ACTIVE_STATE) {
    // In TRAVERSER mode, we want our full step count to stop short of the limit switch, so need phase 3 to move away.
    stepper.stop();
    stepper.setCurrentPosition(stepper.currentPosition());
    Serial.println(F("CALIBRATION: Phase 3, counting limit steps..."));
    stepper.moveTo(0);
    lastStep = 0;
    calibrationPhase = 3;
#endif
#if TURNTABLE_EX_MODE == TRAVERSER
  } else if (calibrationPhase == 1 && lastStep == sanitySteps && getHomeState() == HOME_SENSOR_ACTIVE_STATE) {
    Serial.println(F("CALIBRATION: Phase 2, finding limit switch..."));
#else
  } else if (calibrationPhase == 1 && lastStep == sanitySteps && getHomeState() == HOME_SENSOR_ACTIVE_STATE && stepper.currentPosition() > homeSensitivity) {
    Serial.println(F("CALIBRATION: Phase 2, counting full turn steps..."));
#endif
    stepper.stop();
    stepper.setCurrentPosition(0);
    calibrationPhase = 2;
    stepper.enableOutputs();
#if TURNTABLE_EX_MODE == TRAVERSER
    stepper.moveTo(-sanitySteps);
    lastStep = sanitySteps;
#else
    stepper.moveTo(sanitySteps);
    lastStep = sanitySteps;
#endif
  } else if (calibrationPhase == 0 && !stepper.isRunning() && homed == 1) {
    Serial.println(F("CALIBRATION: Phase 1, homing..."));
    calibrationPhase = 1;
#if TURNTABLE_EX_MODE == TRAVERSER
    if (getHomeState() == HOME_SENSOR_ACTIVE_STATE) {
      Serial.println(F("Turntable already homed"));
    } else {
      stepper.enableOutputs();
      stepper.moveTo(sanitySteps);
    }
#else
    stepper.enableOutputs();
    stepper.moveTo(sanitySteps);
#endif
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

// If phase switching is set to auto, calculate the trigger point steps based on the angle.
#if PHASE_SWITCHING == AUTO
void processAutoPhaseSwitch() {
  if (PHASE_SWITCH_ANGLE + 180 >= 360) {
    Serial.print(F("ERROR: The defined phase switch angle of "));
    Serial.print(PHASE_SWITCH_ANGLE);
    Serial.println(F(" degrees is invalid, setting to default 45 degrees"));
  }
#if PHASE_SWITCH_ANGLE + 180 >= 360
#undef PHASE_SWITCH_ANGLE
#define PHASE_SWITCH_ANGLE 45
#endif
  phaseSwitchStartSteps = fullTurnSteps / 360 * PHASE_SWITCH_ANGLE;
  phaseSwitchStopSteps = fullTurnSteps / 360 * (PHASE_SWITCH_ANGLE + 180);
}
#endif

// Function to debounce and get the state of the homing sensor
bool getHomeState() {
  bool newHomeSensorState = digitalRead(homeSensorPin);
  if (newHomeSensorState != lastHomeSensorState && (millis() - lastHomeDebounce) > DEBOUNCE_DELAY) {
    lastHomeDebounce = millis();
    lastHomeSensorState = newHomeSensorState;
  }
  return lastHomeSensorState;
}

// Function to debounce and get the state of the limit sensor
bool getLimitState() {
  bool newLimitSensorState = digitalRead(limitSensorPin);
  if (newLimitSensorState != lastLimitSensorState && (millis() - lastLimitDebounce) > DEBOUNCE_DELAY) {
    lastLimitDebounce = millis();
    lastLimitSensorState = newLimitSensorState;
  }
  return lastLimitSensorState;
}

// Function to reset home state, triggering homing to happen
void initiateHoming() {
  homed = 0;
  lastTarget = sanitySteps;
}

// Function to trigger calibration to begin
void initiateCalibration() {
  calibrating = true;
  homed = 0;
  lastTarget = sanitySteps;
  clearEEPROM();
}

// Function to set LED activity
void setLEDActivity(uint8_t activity) {
  ledState = activity;
}

// Function to set the state of the accessory pin
void setAccessory(bool state) {
  digitalWrite(accPin, state);
}
