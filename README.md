# EX-Turntable

**Note: This is a Beta relase of EX-Turntable. When performing Beta testing, please follow the process on the [EX-Turntable overview page](https://dcc-ex.com/ex-turntable/index.html).**

**This README is a very brief summary aimed at DCC++ EX team members helping with development and testing. For the full documentation relating to EX-Turntable, please refer to the [DCC++ EX Website](https://dcc-ex.com/).**

**AccelStepper.h credit:** This project would not be effective without the excellent work by Mike McCauley on the AccelStepper.h library that enables us to have somewhat prototypical acceleration and deceleration of the turntable. A slightly modified version of this library is included with the EX-Turntable software (sans example sketches), and more details can be found on the official [AccelStepper](http://www.airspayce.com/mikem/arduino/AccelStepper/) web page. Modification comments are included within the library.

**NmraDcc.h credit:** Also, while not directly used in this software, Alex Shephard's "DCCInterface_TurntableControl" was the inspiration for the initial turntable logic for another DCC driven turntable that translated into the beginnings of EX-Turntable. You can see this code as part of the [NmraDcc Arduino library](https://github.com/mrrwa/NmraDcc).

EX-Turntable is a fully integrated turntable controller for DCC++ EX, using an Arduino microcontroller to drive a stepper controller and motor to spin the turntable bridge.

The integration includes:

- I2C device driver
- EX-RAIL automation support
- Debug/test command (handy for tuning step positions)
- Out-of-the-box support for several common stepper motor drivers
- DCC signal phase switching to align bridge track phase with layout phase

## Traverser mode

As of version 0.4.0-Beta, a new feature was added to support horizontal and vertical traversers as well as restricted rotation turntables that do not rotate a full 360 degrees.

Refer to the [DCC++ EX Website](https://dcc-ex.com/) website for the full documentation on enabling and using the traverser feature as this is not outlined in any detail in this README.

The primary difference is the addition of a limit sensor/switch in addition to the homing sensor, and a modification to the calibration sequence to calculate the steps based on the distance between the home and limit sensors.

All other features (phase switching, LED, accessory output) and control commands remain the same.

## Sensor testing

Also as of version 0.4.0-Beta, an option has been introduced to allow testing of both home and limit sensors while disabling all other EX-Turntable operations.

This is critical to ensuring sensors operate correctly in traverser mode to ensure no physical damage results from the stepper trying to drive a traverser or restricted rotation turntable beyond physical limitations.

# What you need for EX-Turntable

- A CommandStation running the "add-turntable-controller" branch (Displays as version 4.0.2)
- An Arduino microcontroller (tested on Nano V3, both old and new bootloader, an Uno R3 should also work)
- A supported stepper motor driver and stepper motor
- A digital hall effect (or similar) sensor for homing (eg. A3144 or 44E)
- A dual relay board (or similar) if you wish to use the phase switching capability
- A turntable capable of being driven by a stepper motor

# What's in (and not in) this repository?

This repository, EX-Turntable, contains the code required to run EX-Turntable on an Arduino microcontroller. The code is written and tested on a Nano, but should easily work on an Uno or (if you want overkill) a Mega.

In order to control EX-Turntable from the CommandStation, you will also need the [add-turntable-controller](https://github.com/DCC-EX/CommandStation-EX/tree/add-turntable-controller) branch of the CommandStation-EX repository. Note that this displays as version 4.0.2.

# Getting up and running

By default, EX-Turntable will be available on I2C address 0x60, and this is configurable.

You will need to include the IO_TurntableEX.h device driver and create the device in myHal.cpp (absolute bare minimum example below):

```
#if !defined(IO_NO_HAL)

#include "IODevice.h"
#include "IO_TurntableEX.h"

void halSetup() {
  TurntableEX::create(600, 1, 0x60);
}

#endif
```

Like other DCC++ EX code, config.example.h is provided and will be utilised automatically if a specific config.h is not defined.

If nothing is changed in EX-Turntable, it will support a ULN2003 stepper motor controller with a 28BYJ-48 stepper motor, an active-low hall effect sensor for homing, and an active-high dual relay board for phase/polarity switching of the track on the turntable bridge.

Other common stepper drivers and motors are supported, and if an alternative driver or stepper is listed as "pin compatible" with one of these, it will likely also work with the caveat that if it hasn't been tested, that cannot be confirmed. The list of currently tested and supported stepper drivers and motors is:

Driver | Stepper
----|---------
ULN2003 | 28BYJ-48
A4988 | NEMA17
DRV8825 | NEMA17

These pins are in use:

Pin | Function
----|---------
A0 | Stepper pin 1 (note for 2 wire drivers, this is the "step" pin)
A1 | Stepper pin 2 (note for 2 wire drivers, this is the "dir" pin)
A2 | Stepper pin 3 (note for 2 wire drivers, this is the "enable" pin)
A3 | Stepper pin 4
D3 | Relay 1
D4 | Relay 2
D5 | Hall effect sensor
D6 | LED output pin
D7 | Accessory output pin

These pins have been reserved for the potential addition of a GC9A01 round display using SPI:

Pin | Function
----|---------
D8 | Backlight (BL)
D9 | Reset (RST)
D10 | Data/command (DC)
D11 | SPI data (DIN/MOSI)
D12 | Chip select (CS)
D13 | SPI clock (SCK/CLK)

## Note on hall effect homing sensors and phase switching relays

Given there are both active LOW and HIGH options out in the market place for both hall effect sensors and dual relay boards, the active state for each of these is configurable via "config.h".

The default for homing sensors is LOW, and the default for the relay boards is HIGH.

Adjust these lines in "config.h" to suit:

```
#define HOME_SENSOR_ACTIVE_STATE LOW
#define RELAY_ACTIVE_STATE HIGH
```

# Automatic phase switching

As of version 0.3.1-Beta, EX-Turntable supports automatic phase switching as the default behaviour, based on a defined number of degrees from home (default, 45 degrees). This simplifies phase switching for Conductor level users, and will automatically revert the phase 180 degrees later. Manual phase switching via the EX-Turntable control commands is still supported and will not be deprecated. Refer to the documentation for the full explanation of how phase switching is implemented in EX-Turntable, and how to use and configure it.

# EX-Turntable control commands

The turntable is controlled by sending the desired position defined as the number of steps from home, and an activity flag.

There is a diganostic command available for testing purposes, and to enable tuning of specific turntable positons.

```
<D TT vpin steps activity>
```

To control the turntable via EX-RAIL, the MOVETT command has been added.

```
MOVETT(vpin, steps, activity)
```

There are currently three valid activity flags, and for the diagnostic commands, these are numerical, whereas for MOVETT() these are text:

Diagnostic Activity | MOVETT() Activity | Action
--------------------|------------------|------
0 | Turn | Turn to the desired step positions
1 | Turn_PInvert | Turn to the desired step position and invert the phase/polarity
2 | Home | Activate the homing process, ignore the provided step position
3 | Calibrate | Activate the calibration sequence
4 | LED_On | Turns the LED on, ignore the provided step position
5 | LED_Slow | Sets the LED to a slow blink, ignore the provided step position
6 | LED_Fast | Sets the LED to a fast blink, ignore the provided step position
7 | LED_Off | Turns the LED off, ignore the provided step position
8 | Acc_On | Turns the accessory output on, ignore the provided step position
9 | Acc_Off | Turns the accessory output off, ignore the provided step position

The two commands below are equivalent to turn to step position 100, and invert the phase/polarity:

```
<D TT 600 100 1>
MOVETT(600, 100, Turn_PInvert)
```

# Advertising the turntable to throttles

The recommended way to implement the turntable positions is to define a custom TURNTABLE_EX() macro that includes both the MOVETT() command with RESERVE()/FREE() and WAITFOR() commands to ensure nothing is able to interfere with the turntable until after movements have completed.

There will be a sample myEX-Turntable.h_example.txt file for users to copy and customise to suit their layout.

```
// For Conductor level users who wish to just use EX-Turntable, you don't need to understand this
// and can move to defining the turntable positions below. You must, however, ensure this remains
// before any position definitions or you will get compile errors when uploading.
//
// Definition of the TURNTABLE_EX macro to correctly create the ROUTEs required for each position.
// This includes RESERVE()/FREE() to protect any automation activities.
//
#define TURNTABLE_EX(route_id, reserve_id, vpin, steps, activity, desc) \
  ROUTE(route_id, desc) \
    RESERVE(reserve_id) \
    MOVETT(vpin, steps, activity) \
    WAITFOR(vpin) \
    FREE(reserve_id) \
    DONE

// Define your turntable positions here:
//
// TURNTABLE_EX(route_id, reserve_id, vpin, steps, activity, desc)
//
// route_id = A unique number for each defined route, the route is what appears in throttles
// reserve_id = A unique reservation number (0 - 255) to ensure nothing interferes with automation
// vpin = The Vpin defined for the EX-Turntable device driver, default is 600
// steps = The target step position
// activity = The activity performed for this ROUTE (Note do not enclose in quotes "")
// desc = Description that will appear in throttles (Must use quotes "")
//
TURNTABLE_EX(TTRoute1, Turntable, 600, 56, Turn, "Position 1")
TURNTABLE_EX(TTRoute2, Turntable, 600, 111, Turn, "Position 2")
TURNTABLE_EX(TTRoute3, Turntable, 600, 167, Turn, "Position 3")
TURNTABLE_EX(TTRoute4, Turntable, 600, 1056, Turn_PInvert, "Position 4")
TURNTABLE_EX(TTRoute5, Turntable, 600, 1111, Turn_PInvert, "Position 5")
TURNTABLE_EX(TTRoute6, Turntable, 600, 1167, Turn_PInvert, "Position 6")
TURNTABLE_EX(TTRoute7, Turntable, 600, 0, Home, "Home turntable")

// Pre-defined aliases to ensure unique IDs are used.
ALIAS(Turntable, 255)
ALIAS(TTRoute1, 5179)
ALIAS(TTRoute2, 5180)
ALIAS(TTRoute3, 5181)
ALIAS(TTRoute4, 5182)
ALIAS(TTRoute5, 5183)
ALIAS(TTRoute6, 5184)
ALIAS(TTRoute7, 5185)
```

# Calibration sequence

The calibration sequence is required to determine the number of steps to complete a single 360 degree rotation of the turntable. This avoids having to define step counts in advance, and instead enables an easy, automatic method that is "plug and play" for Conductors. This step value is then stored in the EEPROM for reference.

When EX-Turntable is first installed and started, it will initiate the calibration sequence automatically and display the stored step count in the serial console, as well as writing this value to the EEPROM.

The calibration sequence will first initiate a rotation to home the turntable (it doesn't matter where it is positioned at this point), and will then initiate a second rotation which is when the step counting occurs. Messages are written to the serial console to reflect the various calibration steps.

At any time, calibration can be performed again using the "Calibrate" (diagnostic activity 3) command, which will erase the EEPROM contents and initiate the sequence.

Note that if homing fails and calibration has not completed successfully to store the step counts, calibration will not commence until such time as a successful homing activity has been performed. In this instance, once homing issues are resolved, initiate either a "Home" (2) or "Calibrate" (3) command.

## Traverser mode calibration

When traverser mode is enabled, the calibration sequence has a slight modification in behaviour to make use of both the homing and limit sensors, and the step count is calculated on the distance from the home sensor to just prior to activating the limit sensor.

This results in a three step calibration process, whereby the traverser homes first, then moves until it activates the limit switch, and finally moves away to the point where the limit switch is deactivated, and this is recorded as the steps per revolution.

# To do/future

There are a number of items remaining to be completed yet, as well as some extra ideas that could be implemented.

To see all feature requests/enhancements and outstanding bugs that need fixing, refer to the EX-Turntable view of the [DCC++ EX GitHub Project](https://github.com/orgs/DCC-EX/projects/7/views/1).