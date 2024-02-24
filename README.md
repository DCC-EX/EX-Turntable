# EX-Turntable

This README is a very brief summary, for the full documentation relating to EX-Turntable, please refer to the [DCC-EX Website](https://dcc-ex.com/).

**AccelStepper.h credit:** This project would not be effective without the excellent work by Mike McCauley on the AccelStepper.h library that enables us to have somewhat prototypical acceleration and deceleration of the turntable. More details on the library can be found on the official [AccelStepper](http://www.airspayce.com/mikem/arduino/AccelStepper/) web page, and we have included the library within the EX-Turntable software so you don't need to find or download it.

**NmraDcc.h credit:** Also, while not directly used in this software, Alex Shephard's "DCCInterface_TurntableControl" was the inspiration for the initial turntable logic for another DCC driven turntable that translated into the beginnings of EX-Turntable. You can see this code as part of the [NmraDcc Arduino library](https://github.com/mrrwa/NmraDcc).

EX-Turntable is a fully integrated turntable controller for DCC-EX, using an Arduino microcontroller to drive a stepper controller and motor to rotate a turntable bridge or drive a horizontal or vertical traverser.

The integration includes:

- I2C device driver
- EXRAIL automation support
- Interactive commands via the serial console for debugging and testing
- Out-of-the-box support for several common stepper motor drivers
- DCC signal phase switching to align bridge track phase with layout phase
- Operates in either turntable or traverser mode
