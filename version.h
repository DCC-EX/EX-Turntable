#ifndef version_h
#define version_h

#define VERSION "0.5.0-Beta"

// 0.5.0-Beta:
//  - Rename to EX-Turntable in line with re-branding of DCC-EX.
//  - Add test commands via serial console to replicate diagnostic <D TT vpin steps activity> command for local testing.
//  - Disable phase switching during homing.
//  - Increase HOME_SENSITIVITY default to 300 steps due to not moving far enough away from home sensor during calibration.
//  - Correct stepper configuration for A4988/DRV8825 to use AccelStepper::DRIVER.
// 0.4.0-Beta:
//  - Introduced support for traversers and turntables that rotate less than 360 degrees.
//  - Added sensor testing mode that disables all operations and only reports sensor changes to the serial console.
// 0.3.1-Beta:
//  - Introduce automatic phase switching as the default behaviour.
//  - Default phase switching happens at 45 degrees from home, and reverts 180 degrees later, this is configurable via config.h.
//  - Added option to manually define the steps per rotation via config.h.
// 0.3.0-Beta:
//  - No actual changes, resetting version number for intial Beta release.
// 0.2.3 includes:
//  - Bug fix where two wire drivers were not enabling outputs during calibration, and stepper wasn't rotating.
// 0.2.2 includes:
//  - Bug fix where active low relays are activated during the calibration sequence.
// 0.2.1 includes:
//  - Bug fix where failed homing puts calibration into infinite loop.
//  - Calibration no longer initiated if homing fails.
// 0.2.0 includes:
//  - Refactor stepper support to enable easy configuration and support for multiple driver types.
//  - Eliminate defining step count, and use homing to determine rotational steps which are stored in EEPROM.
//  - Calibration sequence rewrite to erase EEPROM (if used) and use homing to count rotational steps.
//  - Start up checks for EEPROM and initiates calibration if not set.
//  - Add support for A4988 and DRV8825 as part of the supported stepper options.
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
