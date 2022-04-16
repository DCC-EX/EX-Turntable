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
D2 | Hall effect sensor
D3 | Relay 1
D4 | Relay 2
D8 | Stepper pin 1
D9 | Stepper pin 2
D10 | Stepper pin 3
D11 | Stepper pin 4

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

Like other commands in EX-RAIL, to make these visible in Engine Driver and WiThrottle apps, define a ROUTE:

```
ROUTE(1, "Demo ROUTE")
  MOVETT(600, 100, 0)
  DONE

ROUTE(2, "Demo ROUTE with phase switch")
  MOVETT(600, 200, 1)
  DONE

ROUTE(3, "Demo homing")
  MOVETT(600, 0, 2)
  DONE
```

There are currently three valid activity flags:

Activity ID | Action
------------|-------
0 | No activity, just rotate the turntable
1 | Rotate the turntable, and activate both relays to switch phase/polarity
2 | Triggers the turntable to home (ignores the provided steps value)

# To do/future

There are a number of items remaining to be completed yet, as well as some extra ideas that could be implemented:

- Enable text based activity flags ([Issue #22](https://github.com/DCC-EX/Turntable-EX/issues/22))
- Add extra supported common steppers ([Issue #6](https://github.com/DCC-EX/Turntable-EX/issues/6))
- Add extra activity flags to control turntable accessories ([Issue #20](https://github.com/DCC-EX/Turntable-EX/issues/20))
- Potentially add a GC9A01 SPI round display ([Issue #15](https://github.com/DCC-EX/Turntable-EX/issues/15
))
