#ifndef version_h
#define version_h

#define VERSION "0.0.3"

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
