/*
 *  © 2023 Peter Cole
 *
 *  This file is part of EX-Turntable
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
 *  along with EX-Turntable.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DEFINES_H
#define DEFINES_H

// Ensure AUTO and MANUAL phase switching has a value to test.
#define AUTO 1
#define MANUAL 0

// Ensure TURNTABLE and TRAVERSER modes also have a value to test.
#define TURNTABLE 0
#define TRAVERSER 1

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

// Define global variables here.
#ifndef TURNTABLE_EX_MODE
#define TURNTABLE_EX_MODE TURNTABLE                 // If the mode isn't defined, put it in turntable mode.
#endif

#ifndef SANITY_STEPS
#define SANITY_STEPS 10000                          // Define sanity steps if not in config.h.
#endif

#ifndef HOME_SENSITIVITY
#define HOME_SENSITIVITY 300                        // Define homing sensitivity if not in config.h.
#endif

#ifndef PHASE_SWITCHING
#define PHASE_SWITCHING AUTO                        // Define automatic phase switching if not in config.h
#endif

#ifndef PHASE_SWITCH_ANGLE
#define PHASE_SWITCH_ANGLE 45                       // Define phase switch at 45 degrees if not in config.h
#endif

#ifndef DEBOUNCE_DELAY                              // Define debounce delay in ms if not in config.h
#if TURNTABLE_EX_MODE == TRAVERSER
#define DEBOUNCE_DELAY 10                           // If we're a traverser, use a delay because switches likely in use
#elif TURNTABLE_EX_MODE == TURNTABLE
#define DEBOUNCE_DELAY 0                            // If we're a turntable, use 0 because hall effect sensor likely in use
#endif
#endif

#ifndef STEPPER_GEARING_FACTOR
#define STEPPER_GEARING_FACTOR 1                    // Define the gearing factor to default of 1 if not in config.h
#endif

// Define current version of EEPROM configuration
#define EEPROM_VERSION 2

#if defined(ROTATE_FORWARD_ONLY) && defined(ROTATE_REVERSE_ONLY)
#error Both ROTATE_FORWARD_ONLY and ROTATE_REVERSE_ONLY defined, please only define one or the other
#endif

#endif
