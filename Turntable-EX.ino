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

#include <Arduino.h>
#include <Wire.h>
#include "standard_steppers.h"
#include "version.h"

#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

// Define global variables here
uint8_t lastPosition;           // Holds the last position we moved to.
bool lastRunningState;          // Stores last running state to allow turning the stepper off after moves.


void displayStepperConfig() {

}

void displayPositions() {

}

void setupStepperDriver() {

}

bool moveHome() {
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(HOME_SENSOR_PIN, INPUT_PULLUP)
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(HOME_SENSOR_PIN, INPUT)
#endif
  stepper.move(fullSteps * 2);
  while(digitalRead(HOME_SENSOR_PIN) != HOME_SENSOR_ACTIVE_STATE) {
    stepper.run();
  }
  if(digitalRead(HOME_SENSOR_PIN) == HOME_SENSOR_ACTIVE_STATE) {
    stepper.stop();
    stepper.setCurrentPosition(0);
    return true;
  } else {
    return false;
  }
}

#if defined(PHASE_SWITCH)
void setPhase(uint8_t phase) {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
}
#endif

void setup() {
// Basic setup, display what this is.
  Serial.begin(115200);
  while(!Serial);
  Serial.println("License GPLv3 fsf.org (c) dcc-ex.com");
  Serial.print("Turntable-EX version ");
  Serial.println(VERSION);

// If we're switching phases, setup the relay pins


// Display the configured stepper details
  displayStepperConfig();

// Display the configured positions
  displayPositions();

// Set up the stepper driver
  setupStepperDriver();

// Home the stepper ready for action
  if(moveHome()) {
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
  }
}

void loop() {

}
