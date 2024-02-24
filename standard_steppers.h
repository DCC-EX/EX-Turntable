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

#if STEPPER_DRIVER == ULN2003_HALF
#define STEPPER_DEFINITION AccelStepper(AccelStepper::HALF4WIRE, A0, A2, A1, A3)
#elif STEPPER_DRIVER == ULN2003_FULL
#define STEPPER_DEFINITION AccelStepper(AccelStepper::FULL4WIRE, A0, A2, A1, A3)
#elif STEPPER_DRIVER == A4988
#define STEPPER_DEFINITION AccelStepper(AccelStepper::DRIVER, A0, A1)
#else
#error Invalid stepper driver selected
#endif

#endif
