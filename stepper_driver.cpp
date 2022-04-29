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
#include "stepper_driver.h"

TurntableEXStepper :: TurntableEXStepper(byte stepperPin1, byte stepperPin2, byte stepperPin3, byte stepperPin4, byte stepperType, bool invertEnable, bool ulnDir) {
  _stepperPin1 = stepperPin1;
  _stepperPin2 = stepperPin2;
  _stepperPin3 = stepperPin3;
  _stepperPin4 = stepperPin4;
  _stepperType = stepperType;
  _invertEnable = invertEnable;
  _ulnDir = ulnDir;
}
