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

/*=============================================================
 * This file contains all functions pertinent to turntable
 * operation including stepper movements, relay phase switching,
 * and LED/accessory related functions.
=============================================================*/

#ifndef TURNTABLEFUNCTIONS_H
#define TURNTABLEFUNCTIONS_H

#include <Arduino.h>
#include "defines.h"
#include "AccelStepper.h"
#include "standard_steppers.h"

void configureSensorPins();
void setupStepperDriver();
void moveHome();
void moveToPosition(int16_t steps, uint8_t phaseSwitch);
void setPhase(uint8_t phase);
void processLED();
void calibration();
void processAutoPhaseSwitch();
bool getHomeState();
bool getLimitState();

#endif
