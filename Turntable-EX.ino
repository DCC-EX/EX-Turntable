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
#include <AccelStepper.h>

// Include standard stepper definitions, version history, and position definitions.
#include "standard_steppers.h"
#include "version.h"
#include "positions.h"

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

// Define global variables here.
uint8_t lastPosition;           // Holds the last position we moved to.
bool lastRunningState;          // Stores last running state to allow turning the stepper off after moves.

// Create our struct for the position definitions.
typedef struct 
{
  uint16_t positionSteps;
  uint8_t phaseSwitch;
}
turntablePosition;
turntablePosition turntablePositions[NUM_POSITIONS];

// Setup our stepper object based on the standard definitions.
#if STEPPER_CONTROLLER == ULN2003

#if STEPPER_DIRECTION == CLOCKWISE
AccelStepper stepper(AccelStepper::FULL4WIRE, ULN2003_PIN4, ULN2003_PIN2, ULN2003_PIN3, ULN2003_PIN1);
#elif STEPPER_DIRECTION == COUNTER_CLOCKWISE
AccelStepper stepper(AccelStepper::FULL4WIRE, ULN2003_PIN1, ULN2003_PIN3, ULN2003_PIN2, ULN2003_PIN4);
#endif

#elif STEPPER_CONTROLLER == A4988

#elif STEPPER_CONTROLLER == DRV8825

#elif STEPPER_CONTROLLER == TMC2208

#endif

void displayStepperConfig() {

}

void displayPositions() {

}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
  stepper.setSpeed(STEPPER_SPEED);
}

bool moveHome() {
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(HOME_SENSOR_PIN, INPUT_PULLUP);
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(HOME_SENSOR_PIN, INPUT);
#endif
  stepper.move(FULLSTEPS * 2);
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
  } else {
    Serial.println("ERROR: Cannot find home position, check homing sensor.");
  }
}

void loop() {

}
