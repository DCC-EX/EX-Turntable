#ifndef version_h
#define version_h

#define VERSION "0.0.8"

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
