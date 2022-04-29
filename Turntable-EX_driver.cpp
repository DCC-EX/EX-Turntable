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
#include "Turntable-EX_driver.h"

TurntableEXStepper :: TurntableEXStepper(
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
    new AccelStepper(AccelStepper::FULL2WIRE, _stepperPin1, _stepperPin2);
  } else if (_stepperType == TTEX_FULL4WIRE) {
    new AccelStepper(AccelStepper::FULL4WIRE, _stepperPin1, _stepperPin2, _stepperPin3, _stepperPin4);
  } else if (_stepperType == TTEX_HALF4WIRE) {
    new AccelStepper(AccelStepper::HALF4WIRE, _stepperPin1, _stepperPin2, _stepperPin3, _stepperPin4);
  }

  void move(long relative);
  
  bool run();
  
  long distanceToGo();
  
  long targetPosition();
  
  long currentPosition();
  
  void setCurrentPosition(long position);
  
  void stop();
  
  void enableOutputs();
  
  void disableOutputs();
  
  bool isRunning();
}
