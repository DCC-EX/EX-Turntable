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

// Define global variables here.
bool lastRunningState;                              // Stores last running state to allow turning the stepper off after moves.
const int16_t fullTurnSteps = FULLSTEPS;            // Assign our defined full turn steps from config.h.
const int16_t halfTurnSteps = fullTurnSteps / 2;    // Defines a half turn to enable moving the least distance.
int16_t lastStep = 0;                               // Holds the last step value we moved to (enables least distance moves).
int16_t lastTarget = fullTurnSteps * 2;             // Holds the last step target (prevents continuous rotatins if homing fails).
bool homed = false;                                 // Flag to indicate if homing has been successful or not.
const uint8_t homeSensorPin = 5;                    // Define pin 5 for the home sensor.
const uint8_t relay1Pin = 3;                        // Control pin for relay 1.
const uint8_t relay2Pin = 4;                        // Control pin for relay 2.
const uint8_t ledPin = 6;                           // Pin for LED output.
const uint8_t accPin = 7;                           // Pin for accessory output.
uint8_t ledState = 7;                               // Flag for the LED state: 4 on, 5 slow, 6 fast, 7 off.
bool ledOutput = LOW;                               // Boolean for the actual state of the output LED pin.
unsigned long ledMillis = 0;                        // Required for non blocking LED blink rate timing.
bool calibrating = false;                           // Flag to prevent other rotation activities during calibration.
bool calibrationStarted = false;                    // Flag to allow delays during calibration process.
unsigned long calMillis = 0;                        // Required for non blocking calibration pauses.

// Setup our stepper object based on the standard definitions.
// #if STEPPER_CONTROLLER == ULN2003

// #if STEPPER_DIRECTION == CLOCKWISE
// AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_4, STEPPER_2, STEPPER_3, STEPPER_1);
// #elif STEPPER_DIRECTION == COUNTER_CLOCKWISE
// AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_1, STEPPER_3, STEPPER_2, STEPPER_4);
// #endif

// #elif STEPPER_CONTROLLER == A4988
AccelStepper stepper(AccelStepper::FULL2WIRE, STEPPER_1, STEPPER_2);

// #elif STEPPER_CONTROLLER == DRV8825

// #elif STEPPER_CONTROLLER == TMC2208

// #endif

// Function to display the defined stepper motor config.
// Currently not able to print text based macros.
void displayStepperConfig() {
  // Serial.println(STEPPER_CONTROLLER);
  // Serial.println(STEPPER_MOTOR);
  Serial.print(F("Stepper motor configured for "));
  Serial.print(fullTurnSteps);
  Serial.println(F(" steps"));
}

// Function to define the stepper parameters.
void setupStepperDriver() {
  stepper.setMaxSpeed(STEPPER_MAX_SPEED);
  stepper.setAcceleration(STEPPER_ACCELERATION);
  stepper.setSpeed(STEPPER_SPEED);
  stepper.setEnablePin(STEPPER_3);
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
    homed = true;
    Serial.println(F("Turntable homed successfully"));
#ifdef DEBUG
    Serial.print(F("DEBUG: Stored values for lastStep/lastTarget: "));
    Serial.print(lastStep);
    Serial.print(F("/"));
    Serial.println(lastTarget);
#endif
  } else if(!stepper.isRunning()) {
    Serial.print(F("DEBUG: Recorded/last actual target: "));
    Serial.print(lastTarget);
    Serial.print(F("/"));
    Serial.println(stepper.targetPosition());
    if (stepper.targetPosition() == lastTarget) {
      stepper.setCurrentPosition(0);
      lastStep = 0;
      homed = true;
      Serial.println(F("ERROR: Turntable failed to home, setting random home position"));
    } else {
      stepper.move(fullTurnSteps * 2);
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
    } else if (activity == 2 && !stepper.isRunning() && !calibrating) {
      // Activity 2 needs to reset our homed flag to initiate the homing process, only if stepper not running.
#ifdef DEBUG
      Serial.println(F("DEBUG: Requested to home"));
#endif
      homed = false;
      lastTarget = fullTurnSteps * 2;
    } else if (activity == 3 && !stepper.isRunning() && !calibrating) {
      // Activity 3 will initiate calibration sequence, only if stepper not running.
#ifdef DEBUG
      Serial.println(F("DEBUG: Calibration requested"));
#endif
      calibrating = true;
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
#ifdef DEBUG
  Serial.print(F("DEBUG: Setting relay outputs for relay 1/2: "));
  Serial.println(phase);
#endif
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

// Function to rotate the turntable to 90, 180, 270, 360 degrees to validate step count.
// Turntable will home first, then refer to lastStep to calculate next sequence position.
// Non-blocking delays utilised.
// At completion, it will move forward 100 steps, then home again.
// Turntable positions to calibration:
// 180 degress = halfTurnSteps
// 360 degress = fullTurnSteps
void calibration() {
  if (!stepper.isRunning()) {
    unsigned long currentMillis = millis();
    if (lastStep == 0 && calibrationStarted) {
      // If we're homed, move to 180 degree step position first.
      Serial.print(F("Calibration: 180 degree step position: "));
      Serial.println(halfTurnSteps);
      moveToPosition(halfTurnSteps, 0);
      calMillis = currentMillis;
    } else if (lastStep == halfTurnSteps && currentMillis - calMillis >= CALIBRATION_DELAY) {
      // If our last was 180 degrees and we've waited 10 seconds, move to 360 degrees.
      Serial.print(F("Calibration: 360 degree step position: "));
      Serial.println(fullTurnSteps);
      moveToPosition(fullTurnSteps, 0);
      calMillis = currentMillis;
    } else if (lastStep == fullTurnSteps && currentMillis - calMillis >= CALIBRATION_DELAY) {
      // If our last was 360 degrees and we've waited 10 seconds, move to 5% of steps so we're not homed.
      Serial.print(F("Calibration completed, moving to step "));
      Serial.print(round(fullTurnSteps * 0.05));
      Serial.println(F(" then homing"));
      moveToPosition(round(fullTurnSteps * 0.05), 0);
    } else if (lastStep == round(fullTurnSteps * 0.05)) {
      // If we're at 5% of steps, calibration is done, trigger homing.
      calibrating = false;
      calibrationStarted = false;
      homed = false;
      lastTarget = fullTurnSteps * 2;
    } else if (lastStep == round(fullTurnSteps * 0.1)) {
      // If we're at 10% of steps, calibration starting, trigger homing.
      homed = false;
      lastTarget = fullTurnSteps * 2;
    } else if (!calibrationStarted)  {
      // Calibration starts at any position, so move to 10% as a known starting point then home again
      Serial.print(F("Calibration initiated, moving to step "));
      Serial.print(round(fullTurnSteps * 0.1));
      Serial.println(F(" then homing"));
      moveToPosition(round(fullTurnSteps * 0.1), 0);
      calibrationStarted = true;
    }
  }
}

void setup() {
// Basic setup, display what this is.
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("License GPLv3 fsf.org (c) dcc-ex.com"));
  Serial.print(F("Turntable-EX version "));
  Serial.println(VERSION);

// Configure homing sensor pin
#if HOME_SENSOR_ACTIVE_STATE == LOW
  pinMode(homeSensorPin, INPUT_PULLUP);
#elif HOME_SENSOR_ACTIVE_STATE == HIGH
  pinMode(homeSensorPin, INPUT);
#endif

// Configure relay output pins
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

// Configure LED and accessory output pins
  pinMode(ledPin, OUTPUT);
  pinMode(accPin, OUTPUT);

// Display the configured stepper details
  displayStepperConfig();

// Set up the stepper driver
  setupStepperDriver();

// Now we're ready, set up I2C.
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.println(F("Homing..."));
}

void loop() {
// If we haven't successfully homed yet, do it.
  if (!homed) {
    moveHome();
  }

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
