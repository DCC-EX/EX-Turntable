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
uint16_t halfTurnSteps = FULLSTEPS / 2; // Defines a half turn to enable moving the least distance.
uint16_t lastStep = 0;          // Holds the last step value we moved to.

// Create our struct for the position definitions.
typedef struct 
{
  uint16_t positionSteps;
  uint8_t phaseSwitch;
}
turntablePosition;
turntablePosition turntablePositions[NUM_POSITIONS];

// This array contains the Turnout Positions which can have lines added/removed to suit your turntable.
// Positions are defined with a step value (distance from home) and a phase switch flag: 0 no change, 1 reverse.
void initPositions() {
  turntablePositions[0] = (turntablePosition) {20, 0};
  turntablePositions[1] = (turntablePosition) {150, 0};
  turntablePositions[2] = (turntablePosition) {600, 0};
  turntablePositions[3] = (turntablePosition) {1200, 1};
  turntablePositions[4] = (turntablePosition) {2000, 1};
}

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

// Function to display the defined stepper motor config.
// Currently not able to print text based macros.
void displayStepperConfig() {
  // Serial.println(STEPPER_CONTROLLER);
  // Serial.println(STEPPER_MOTOR);
  Serial.print("Stepper motor configured for ");
  Serial.print(FULLSTEPS);
  Serial.println(" steps");
}

// Function to display our defined positions in the serial console.
void displayPositions() {
  Serial.print(NUM_POSITIONS);
  Serial.println(" turntable positions defined:");
  for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
    Serial.print("Position ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(turntablePositions[i].positionSteps);
    Serial.print(" steps, phase switch: ");
    Serial.println(turntablePositions[i].phaseSwitch);
  }
}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
  stepper.setSpeed(STEPPER_SPEED);
}

// Function to find the home position.
// Stepper will rotate up to two full circles to attempt to home.
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
    lastStep = 0;
    return true;
  } else {
    return false;
  }
}

// Function to define the action on a received I2C event.
void receiveEvent(int position) {
  while(Wire.available()) {
    Wire.read();
  }
  Serial.print("Received notification to move to postion ");
  Serial.println(position);
  moveToPosition(position);
}

// Function to move to the indicated position.
void moveToPosition(int position) {
  if (position != lastPosition && position <= NUM_POSITIONS) {
    if (position == 0) {
      moveHome();
    } else {
      int positionIndex = position - 1;     // Our array index will be one less than the position.
      uint16_t steps = turntablePositions[positionIndex].positionSteps;
      uint8_t phaseSwitch = turntablePositions[positionIndex].phaseSwitch;
      if (steps <= FULLSTEPS && phaseSwitch < 2) {
        int16_t moveSteps;
        Serial.print((String)"Position steps: " + steps + ", Phase switch flag: " + phaseSwitch);
        if ((steps - lastStep) > halfTurnSteps) {
          moveSteps = steps - FULLSTEPS - lastStep;
        } else if ((steps - lastStep) < -halfTurnSteps) {
          moveSteps = FULLSTEPS - lastStep + steps;
        } else {
          moveSteps = steps - lastStep;
        }
        Serial.println((String)" - moving " + moveSteps + " steps");
#if defined(PHASE_SWITCH)
        setPhase(phaseSwitch);
#endif
        lastStep = steps;
        stepper.move(moveSteps);
      }
    }
  }
}

// If phase switching is enabled, function to set it.
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

// Initialise the turntable positions.
  initPositions();

// Display the configured stepper details
  displayStepperConfig();

// Display the configured positions
  displayPositions();

// Set up the stepper driver
  setupStepperDriver();

// Home the stepper ready for action
  Serial.println("Homing...");
  if(moveHome()) {
    Serial.println("Homed successfully");
  } else {
    Serial.println("ERROR: Cannot find home position, check homing sensor.");
  }
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif

// Now we're ready, set up I2C.
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.println("Listening for I2C events");
}

void loop() {
// Process the stepper object.
  stepper.run();

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
