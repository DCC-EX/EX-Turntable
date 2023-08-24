/*
 *  © 2023 Peter Cole
 *  © 2022 Peter Cole
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

// Include required libraries.
#include <Arduino.h>
#include "defines.h"

// Include local files
#include "IOFunctions.h"
#include "TurntableFunctions.h"

bool lastRunningState;   // Stores last running state to allow turning the stepper off after moves.

void setup() {
  // Run startup configuration
  startupConfiguration();

  // Display EX-Turntable configuration
  displayTTEXConfig();

  // Set up the stepper driver
  setupStepperDriver();
}

void loop() {
#ifdef SENSOR_TESTING
// If we're only testing sensors, don't do anything else.
  bool testHomeSensorState = getHomeState();
  if (testHomeSensorState != homeSensorState) {
    if (testHomeSensorState == HOME_SENSOR_ACTIVE_STATE) {
      Serial.println(F("Home sensor ACTIVATED"));
    } else {
      Serial.println(F("Home sensor DEACTIVATED"));
    }
    homeSensorState = testHomeSensorState;
  }
  getHomeState();

#if TURNTABLE_EX_MODE == TRAVERSER
  bool testLimitSensorState = getLimitState();
  if (testLimitSensorState != limitSensorState) {
    if (testLimitSensorState == LIMIT_SENSOR_ACTIVE_STATE) {
      Serial.println(F("Limit sensor ACTIVATED"));
    } else {
      Serial.println(F("Limit sensor DEACTIVATED"));
    }
    limitSensorState = testLimitSensorState;
  }
#endif

#else

#if TURNTABLE_EX_MODE == TRAVERSER
// If we hit our limit switch when not calibrating, stop!
  if (getLimitState() == LIMIT_SENSOR_ACTIVE_STATE && !calibrating && stepper.isRunning() && stepper.targetPosition() < 0) {
    Serial.println(F("ALERT! Limit sensor activitated, halting stepper"));
    if (!homed) {
      homed = 1;
    }
    stepper.stop();
    stepper.setCurrentPosition(stepper.currentPosition());
  }

// If we hit our home switch when not homing, stop!
  if (getHomeState() == HOME_SENSOR_ACTIVE_STATE && homed && !calibrating && stepper.isRunning() && stepper.distanceToGo() > 0) {
    Serial.println(F("ALERT! Home sensor activitated, halting stepper"));
    stepper.stop();
    stepper.setCurrentPosition(0);
  }
#endif

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

// Receive and process and serial input for test commands.
  processSerialInput();

#endif
}
