/*
 *  © 2022 Peter Cole
 *
 *  These are the standard stepper controller and motor definitions.
*/

#ifndef STANDARD_STEPPERS_h
#define STANDARD_STEPPERS_h

#include <Arduino.h>
#include "AccelStepper.h"

#define UNUSED_PIN 127

#define FULLSTEPS 800

#define ULN2003_HALF_CW AccelStepper(AccelStepper::HALF4WIRE, A3, A1, A2, A0)
#define ULN2003_HALF_CCW AccelStepper(AccelStepper::HALF4WIRE, A0, A2, A1, A3)
#define ULN2003_FULL_CW AccelStepper(AccelStepper::FULL4WIRE, A3, A1, A2, A0)
#define ULN2003_FULL_CCW AccelStepper(AccelStepper::FULL4WIRE, A0, A2, A1, A3)
#define TWO_WIRE AccelStepper(AccelStepper::FULL2WIRE, A0, A1, A2)
#define TWO_WIRE_INV AccelStepper(AccelStepper::FULL2WIRE, A0, A1, A2, UNUSED_PIN, true, true)

#endif
