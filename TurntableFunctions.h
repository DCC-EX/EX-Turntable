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

#ifndef TURNTABLEFUNCTIONS_H
#define TURNTABLEFUNCTIONS_H

#include <Arduino.h>
#include "defines.h"
#include "AccelStepper.h"
#include "standard_steppers.h"

int16_t lastStep; // Holds the last step value we moved to (enables least distance moves).
uint8_t homed;  // Flag to indicate homing state: 0 = not homed, 1 = homed, 2 = failed.
int16_t lastTarget; // Holds the last step target (prevents continuous rotatins if homing fails).
int16_t fullTurnSteps;  // Assign our defined full turn steps from config.h.
int16_t halfTurnSteps;  // Defines a half turn to enable moving the least distance.
int16_t phaseSwitchStartSteps;  // Defines the step count at which phase should automatically invert.
int16_t phaseSwitchStopSteps;   // Defines the step count at which phase should automatically revert.
const int16_t sanitySteps; // Define an arbitrary number of steps to prevent indefinite spinning if homing/calibrations fails.
const uint8_t limitSensorPin;    // Define pin 2 for the traverser mode limit sensor.
const uint8_t homeSensorPin;     // Define pin 5 for the home sensor.
const uint8_t relay1Pin;         // Control pin for relay 1.
const uint8_t relay2Pin;                        // Control pin for relay 2.
const uint8_t ledPin;                           // Pin for LED output.
const uint8_t accPin;                           // Pin for accessory output.
uint8_t ledState;                               // Flag for the LED state: 4 on, 5 slow, 6 fast, 7 off.
bool ledOutput;                               // Boolean for the actual state of the output LED pin.
unsigned long ledMillis;                        // Required for non blocking LED blink rate timing.
bool calibrating;                           // Flag to prevent other rotation activities during calibration.
uint8_t calibrationPhase;                       // Flag for calibration phase.
unsigned long calMillis;                        // Required for non blocking calibration pauses.
bool homeSensorState;                               // Stores the current home sensor state.
bool limitSensorState;                              // Stores the current limit sensor state.
bool lastHomeSensorState;                           // Stores the last home sensor state.
bool lastLimitSensorState;                          // Stores the last limit sensor state.
unsigned long lastLimitDebounce;                // Stores the last time the limit sensor switched for debouncing.
unsigned long lastHomeDebounce;                 // Stores the last time the home sensor switched for debouncing.
const int16_t homeSensitivity;   // Define the minimum number of steps required before homing sensor deactivates.

AccelStepper stepper;

void setupStepperDriver();
void moveHome();
void moveToPosition(int16_t steps, uint8_t phaseSwitch);
void setPhase(uint8_t phase);
void processLED();
void calibration();
void processAutoPhaseSwitch();
bool getHomeState();
bool getLimitState();

#endif
