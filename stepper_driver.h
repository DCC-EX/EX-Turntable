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

#ifndef STEPPER_DRIVER_h
#define STEPPER_DRIVER_h

#include "AccelStepper.h"

#ifndef UNUSED_PIN
#define UNUSED_PIN 127
#endif

class TurntableEXStepper {
  public:
    TurntableEXStepper(byte stepperPin1, byte stepperPin2, byte stepperPin3, byte stepperPin4, byte stepperType, bool invertEnable, bool ulnDir);
  
  private:
    byte _stepperPin1;
    byte _stepperPin2;
    byte _stepperPin3;
    byte _stepperPin4;
    byte _stepperType;
    bool _invertEnable;
    bool _ulnDir;
};

#endif
