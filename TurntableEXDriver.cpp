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
#include "TurntableEXDriver.h"

TurntableEXDriver :: TurntableEXDriver(
    byte stepperType,
    byte stepperPin1,
    byte stepperPin2,
    byte stepperPin3,
    byte stepperPin4,
    float maxSpeed,
    float acceleration,
    bool invertEnable) {
    _stepperType = stepperType;
    _stepperPin1 = stepperPin1;
    _stepperPin2 = stepperPin2;
    _stepperPin3 = stepperPin3;
    _stepperPin4 = stepperPin4;
    _maxSpeed = maxSpeed;
    _acceleration = acceleration;
    _invertEnable = invertEnable;

    if (_stepperType == TTEX_FULL2WIRE) {
      stepper.setEnablePin(_stepperPin3);
      if (invertEnable) {
        stepper.setPinsInverted(false, false, true);
      }
    }

    stepper.setMaxSpeed(_maxSpeed);
    stepper.setAcceleration(_acceleration);
  }
  

void TurntableEXDriver::move(long relative) {
  stepper.move(relative);
}

bool TurntableEXDriver::run() {
  return stepper.run();
}

long TurntableEXDriver::distanceToGo() {
  return stepper.distanceToGo();
}

long TurntableEXDriver::targetPosition() {
  return stepper.targetPosition();
}

long TurntableEXDriver::currentPosition() {
  return stepper.currentPosition();
}

void TurntableEXDriver::setCurrentPosition(long position) {
  stepper.setCurrentPosition(position);
}

void TurntableEXDriver::stop() {
  stepper.stop();
}

void TurntableEXDriver::enableOutputs() {
  stepper.enableOutputs();
}

void TurntableEXDriver::disableOutputs() {
  stepper.disableOutputs();
}

bool TurntableEXDriver::isRunning() {
  return stepper.isRunning();
}
