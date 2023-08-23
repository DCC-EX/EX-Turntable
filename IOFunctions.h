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

#ifndef IOFUNCTIONS_H
#define IOFUNCTIONS_H

#include <Arduino.h>
#include <Wire.h>
#include "defines.h"
#include "TurntableFunctions.h"
#include "EEPROMFunctions.h"

extern bool testCommandSent;    // Flag a test command has been sent via serial.
extern uint8_t testStepsMSB;    // MSB of test steps sent via serial.
extern uint8_t testStepsLSB;    // LSB of test steps sent via serial.
extern uint8_t testActivity;    // Activity sent via serial.

void processSerialInput();
void displayTTEXConfig();
void receiveEvent(int received);
void requestEvent();

#endif
