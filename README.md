# Turntable-EX

**Note:** This code is currently considered experimental and under rapid development. This README is aimed at DCC++ EX team members who wish to help with development and testing prior to release.

Turntable-EX is a fully integrated turntable controller, using an Arduino microcontroller to drive a stepper controller and motor to spin the turntable bridge.

The integration includes:

- I2C device driver
- EX-RAIL automation support
- Debug/test command (handy for tuning step positions)
- Out-of-the-box support for several common stepper controllers
- DCC signal phase switching to align bridge track phase with layout phase

# What you need for Turntable-EX

- An Arduino microcontroller (tested on Nano V3, both old and new bootloader, an Uno R3 should also work)
- A supported stepper motor controller and stepper motor
- A hall effect (or similar) sensor for homing
- A dual relay board (or similar) if you wish to use the phase switching capability
- A turntable capable of being driven by a stepper motor

# What's in (and not in) this repository?

This repository, Turntable-EX, contains the code required to run Turntable-EX on an Arduino microcontroller. The code is written and tested on a Nano, but should easily work on an Uno or (if you want overkill) a Mega.

In order to control Turntable-EX from the CommandStation, you will also need the [add-turntable-controller](https://github.com/DCC-EX/CommandStation-EX/tree/add-turntable-controller) branch of the CommandStation-EX repository.

# Getting up and running

By default, Turntable-EX will be available on I2C address 0x60, and this is configurable.

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

If nothing is changed in Turntable-EX, it will support a ULN2003 stepper motor controller with a 28BYJ-48 stepper motor, an active-low hall effect sensor for homing, and a dual relay board for phase/polarity switching of the track on the turntable bridge.

Other common stepper drivers/motors will be supported in due course once they've been able to be tested. These will include A4988, DRV8825, and likely TMC208 drivers, all with a NEMA17 stepper.

These pins are in use:

Pin | Function
----|---------
A0 | Stepper pin 1
A1 | Stepper pin 2
A2 | Stepper pin 3
A3 | Stepper pin 4
D3 | Relay 1
D4 | Relay 2
D5 | Hall effect sensor
D6 | LED output pin
D7 | Accessory output pin

# Turntable-EX control commands

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

There will be a sample myTurntable-EX.h_example.txt file for users to copy and customise to suit their layout.

```
// For Conductor level users who wish to just use Turntable-EX, you don't need to understand this
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
// vpin = The Vpin defined for the Turntable-EX device driver, default is 600
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

The calibration sequence has been added to validate that the defined number of steps for a full 360 degree rotation is accurate.

The sequence is activated by ```<D TT 600 0 3>``` or ```MOVETT(600, 0, Calibrate)```.

To calibrate your turntable, mark the "home" position as 0/360 degrees, and put a mark at 180 degrees.

When initiating the calibration sequence, the turntable will automatically cycle through these positions, pausing at each to allow validation that it is aligning correctly with those positions.

The sequence will proceed as follows:

- The turntable will first move to 10% of the defined full step count and then home to ensure the process starts accurately at the home position.
- It will then move to 1/2 of the defined full step count and pause, which should align with the 180 degree mark.
- It will then move to the defined full step count and pause, which should align with the 0/360 degree mark.
- Finally, it will move to 5% of the defined full step count, and again home to ensure it returns to the home position.

If the alignment is not as expected, adjust the step count by uncommenting the below line in "config.h" and setting the correct number of steps:

```
// #define FULLSTEPS 1234
```

If the correct number of steps for a 360 degree rotation should be 2000, the resultant line should look like this:

```
#define FULLSTEPS 2000
```

If the pauses at each position are too long or too short to assess the alignment, update the following line in "config.h" to increase or decrease the pause time, noting that this is in milliseconds (the default of 15000 = 15 seconds). Note that the delay time includes the amount of time for the stepper to move from position to position.

```
#define CALIBRATION_DELAY 15000
```

# To do/future

There are a number of items remaining to be completed yet, as well as some extra ideas that could be implemented:

- Add extra supported common steppers ([Issue #6](https://github.com/DCC-EX/Turntable-EX/issues/6))
- Add installer tests ([Issue #24](https://github.com/DCC-EX/Turntable-EX/issues/24))
- Potentially add a GC9A01 SPI round display ([Issue #15](https://github.com/DCC-EX/Turntable-EX/issues/15))
