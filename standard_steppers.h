/*
 *  © 2022 Peter Cole
 *
 *  These are the standard stepper controller and motor definitions.
*/

#ifndef STANDARD_STEPPERS_h
#define STANDARD_STEPPERS_h

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

#define STEPPER_1 A0
#define STEPPER_2 A1
#define STEPPER_3 A2
#define STEPPER_4 A3

#if STEPPER_CONTROLLER == ULN2003



#elif STEPPER_CONTROLLER == A4988



#elif STEPPER_CONTROLLER == DRV8825



#elif STEPPER_CONTROLLER == TMC2208



#endif

#if STEPPER_MOTOR == ULN2003COMBO

#ifndef FULLSTEPS
#define FULLSTEPS 2048
#endif

#endif

#endif
