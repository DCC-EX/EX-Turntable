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

#define STEPPER_1 A0    // Pin 1 for 4 wire (ULN2003), step pin for 2 wire (A4988, DRV8825)
#define STEPPER_2 A1    // Pin 2 for 4 wire (ULN2003), dir pin for 2 wire (A4988, DRV8825)
#define STEPPER_3 A2    // Pin 3 for 4 wire (ULN2003), enable pin for 2 wire (A4988, DRV8825)
#define STEPPER_4 A3    // Pin 4 for 4 wire (ULN2003), unused for 2 wire

// #if STEPPER_MOTOR == ULN2003COMBO

// #ifndef FULLSTEPS
// #define FULLSTEPS 2048
// #endif

// #elif STEPPER_MOTOR == NEMA17

// #ifndef FULLSTEPS
#define FULLSTEPS 200
// #endif

// #endif

#endif
