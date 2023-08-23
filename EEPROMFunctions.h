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

#ifndef EEPROMFUNCTIONS_H
#define EEPROMFUNCTIONS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "defines.h"
#include "TurntableFunctions.h"

int16_t getSteps();
void writeEEPROM(int16_t steps);
void clearEEPROM();

#endif
