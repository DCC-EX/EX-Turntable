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

int16_t lastStep = 0;
uint8_t homed = 0;
int16_t lastTarget = sanitySteps;
const int16_t sanitySteps = SANITY_STEPS;
const uint8_t limitSensorPin = 2;
const uint8_t homeSensorPin = 5;
const uint8_t relay1Pin = 3;
const uint8_t relay2Pin = 4;
const uint8_t ledPin = 6;
const uint8_t accPin = 7;
uint8_t ledState = 7;
bool ledOutput = LOW;
unsigned long ledMillis = 0;
bool calibrating = false;
uint8_t calibrationPhase = 0;
unsigned long calMillis = 0;
unsigned long lastLimitDebounce = 0;
unsigned long lastHomeDebounce = 0;
const int16_t homeSensitivity = HOME_SENSITIVITY;

AccelStepper stepper = STEPPER_DRIVER;

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
void moveToPosition(int16_t steps, uint8_t phaseSwitch) {
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
