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

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

// Define global variables here.
bool lastRunningState;                              // Stores last running state to allow turning the stepper off after moves.
const int16_t fullTurnSteps = FULLSTEPS;            // Assign our defined full turn steps from config.h.
const int16_t halfTurnSteps = fullTurnSteps / 2;    // Defines a half turn to enable moving the least distance.
int16_t lastStep = 0;                               // Holds the last step value we moved to.
bool homed = false;                                 // Flag to indicate if homing has been successful or not.
uint16_t homingSteps = 0;                           // Counter to limit how many rotations until homing fails.

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
  Serial.print(fullTurnSteps);
  Serial.println(" steps");
}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
  stepper.setSpeed(STEPPER_SPEED);
}

// Function to find the home position.
void moveHome() {
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(HOME_SENSOR_PIN, INPUT_PULLUP);
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(HOME_SENSOR_PIN, INPUT);
#endif
  stepper.move(fullTurnSteps);
  if(digitalRead(HOME_SENSOR_PIN) == HOME_SENSOR_ACTIVE_STATE) {
    stepper.stop();
#if defined(DISABLE_OUTPUTS_IDLE)
    stepper.disableOutputs();
#endif
    stepper.setCurrentPosition(0);
    lastStep = 0;
    homed = true;
    Serial.println("Turntable homed successfully");
  }
}

// Function to define the action on a received I2C event.
void receiveEvent(int received) {
  Serial.print("DEBUG: Received ");
  Serial.print(received);
  Serial.println(" bytes");
  int16_t steps;
  uint8_t activity;
  if (received == 3) {
    uint8_t stepsMSB = Wire.read();
    uint8_t stepsLSB = Wire.read();
    activity = Wire.read();
    Serial.print("DEBUG: stepsMSB:");
    Serial.print(stepsMSB);
    Serial.print(", stepsLSB:");
    Serial.print(stepsLSB);
    Serial.print(", activity:");
    Serial.println(activity);
    steps = (stepsMSB << 8) + stepsLSB;
    if (steps <= fullTurnSteps && activity < 2) {
      Serial.print("DEBUG: Requested valid step move to: ");
      Serial.print(steps);
      Serial.print(" with phase switch: ");
      Serial.println(activity);
      moveToPosition(steps, activity);
    } else if (activity == 2) {
      Serial.println("DEBUG: Requested to home");
      // moveHome();
      homed = false;
      homingSteps = 0;
    } else {
      Serial.print("DEBUG: Invalid step count or activity provided: ");
      Serial.print(steps);
      Serial.print(" steps, activity: ");
      Serial.println(activity);
    }
  } else {
    Serial.println("DEBUG: Incorrect number of bytes received, discarding");
    while (Wire.available()) {
      Wire.read();
    }
  }
}

// Function to move to the indicated position.
void moveToPosition(int16_t steps, uint8_t phaseSwitch) {
  if (steps != lastStep) {
    Serial.print("Received notification to move to step postion ");
    Serial.println(steps);
      int16_t moveSteps;
      Serial.print((String)"Position steps: " + steps + ", Phase switch flag: " + phaseSwitch);
      if ((steps - lastStep) > halfTurnSteps) {
        moveSteps = steps - fullTurnSteps - lastStep;
      } else if ((steps - lastStep) < -halfTurnSteps) {
        moveSteps = fullTurnSteps - lastStep + steps;
      } else {
        moveSteps = steps - lastStep;
      }
      Serial.println((String)" - moving " + moveSteps + " steps");
      Serial.print("Setting phase switch flag to: ");
      Serial.println(phaseSwitch);
      setPhase(phaseSwitch);
      lastStep = steps;
      stepper.move(moveSteps);
  }
  Serial.print("DEBUG: Stored values for lastStep: ");
  Serial.println(lastStep);
}

// If phase switching is enabled, function to set it.
void setPhase(uint8_t phase) {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  Serial.print("DEBUG: Setting relay outputs for relay 1/2: ");
  Serial.println(phase);
  digitalWrite(RELAY1_PIN, phase);
  digitalWrite(RELAY2_PIN, phase);
}

void setup() {
// Basic setup, display what this is.
  Serial.begin(115200);
  while(!Serial);
  Serial.println("License GPLv3 fsf.org (c) dcc-ex.com");
  Serial.print("Turntable-EX version ");
  Serial.println(VERSION);

// Display the configured stepper details
  displayStepperConfig();

// Set up the stepper driver
  setupStepperDriver();

// Now we're ready, set up I2C.
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.println("Homing...");
}

void loop() {
// If we haven't successfully homed yet, do it.
  if (!homed) {
    moveHome();
  }

// Process the stepper object continuously.
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
