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

#define FULLSTEPS 4096

#define ULN2003_HALF AccelStepper(AccelStepper::HALF4WIRE, A0, A2, A1, A3)
#define ULN2003_FULL AccelStepper(AccelStepper::FULL4WIRE, A0, A2, A1, A3)
#define A4988 AccelStepper(AccelStepper::DRIVER, A0, A1, A2)

#endif
