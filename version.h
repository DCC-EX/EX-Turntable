#ifndef version_h
#define version_h

#define VERSION "0.1.2"

// 0.1.2 includes:
//  - Fix typo in TURNTABLE_EX() macro in README (activit).
//  - RAM optimisation with Serial.print(F()) and remove Serial.print((String)).
//  - Enabled RELAY_ACTIVE_STATE to cater for active low or high relays.
// 0.1.1 includes:
//  - Missed updating stepper pin definitions for COUNTER_CLOCKWISE.
//  - Home sensor pin still incorrectly mentions pin 2.
// 0.1.0 includes:
//  - "Breaking" change with revised pin allocations. This has been done to allow the potential future use of pin D2 for
//    interrupt driven activities if required (potentially a DCC controlled option rather than I2C).
//  - Also caters for GC9A01 round display if this proves viable.
//  - There is also a simplification of the calibration sequence.
//  - Also a clean up of debug outputs by using a DEBUG flag.
// 0.0.9 includes:
//  - Added calibration sequence to validate step count for 360 degree rotation is accurate.
// 0.0.8 includes:
//  - Added LED and accessory output support, with LED on/blink slow/blink fast/off, and accessory on/off.
// 0.0.7 includes:
//  - Revise return status to 0 (stopped) or 1 (moving) only, return status of 2 unused and causing issues with EX-RAIL RESERVE/FREE.
// 0.0.6 includes:
//  - Move to statically defined home sensor and relay pins.
// 0.0.5 includes:
//  - Returns status to the TurntableEX.h device driver when requested.
// 0.0.4 includes:
//  - Improvement to homing, limiting rotations to two full turns if homing fails.
// 0.0.3 includes:
//  - Bug fix for two homing issues and cleaner debug output.
//  - Also removed ability for users to enable/disable relay outputs to keep things simple.
//    If users don't wish to use the relay outputs, they can simply leave them unused.
// 0.0.2 includes:
//  - Steps sent by CommandStation SERVO command, no local position/step configuration.
//  - Phase switch also by SERVO command.
// 0.0.1 includes:
//  - Basic functionality with positions defined in code via array of structs.
//  - Phase switching via external dual relay.

#endif
