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

// For simplicity, we will only support 2 and 4 wire stepper drivers with the TurntableEXStepper driver.
#define TTEX_FULL2WIRE 2        // Use for all 2 wire drivers eg. A4988, DRV8825
#define TTEX_FULL4WIRE 4        // Use for ULN2003 and other 4 wire drivers in full step mode
#define TTEX_HALF4WIRE 8        // Use for ULN2003 and other 4 wire drivers in half step mode

class TurntableEXStepper {
  public:
    TurntableEXStepper(
      uint8_t stepperType,
      uint8_t stepperPin1,
      uint8_t stepperPin2,
      uint8_t stepperPin3,
      uint8_t stepperPin4,
      float maxSpeed,
      float acceleration,
      bool invertEnable);
    
    // Only defining required AccelStepper functions:
    void move(long relative);
    bool run();
    long distanceToGo();
    long targetPosition();
    long currentPosition();
    void setCurrentPosition(long position);
    void stop();
    virtual void enableOutputs();
    virtual void disableOutputs();
    bool isRunning();
  
  private:
    uint8_t _stepperType;
    uint8_t _stepperPin1;
    uint8_t _stepperPin2;
    uint8_t _stepperPin3;
    uint8_t _stepperPin4;
    float _maxSpeed;
    float _acceleration;
    bool _invertEnable;
};

#endif
