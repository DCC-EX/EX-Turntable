/*
 *  © 2022 Peter Cole
 *
 *  This is the configuration file for Turntable-EX.
 */

/////////////////////////////////////////////////////////////////////////////////////
//  Define a valid (and free) I2C address, 0x60 is the default.
// 
#define I2C_ADDRESS 0x60

/////////////////////////////////////////////////////////////////////////////////////
//  Define the mode for Turntable-EX.
//  TURNTABLE : Use this for normal, 360 degree rotation turntables (Default).
//  TRAVERSER : Use this for vertical or horizontal traversers, or turntables that do
//              do not rotate a full 360 degrees.
// 
#define TURNTABLE_EX_MODE TURNTABLE
// #define TURNTABLE_EX_MODE TRAVERSER

/////////////////////////////////////////////////////////////////////////////////////
//  Enable sensor testing only, prevents all Turntable-EX operations.
//  Uncomment this line to disable all normal Turntable-EX operations in order to test
//  and validate that homing and limit sensors activate and deactivate correctly.
//  Note that you can enable sensor testing interactively in the serial console with <T>
// 
// #define SENSOR_TESTING

/////////////////////////////////////////////////////////////////////////////////////
//  Define the active state for the homing sensor.
//  LOW = When activated, the input is pulled down (ground or 0V).
//  HIGH = When activated, the input is pulled up (typically 5V).
// 
#define HOME_SENSOR_ACTIVE_STATE LOW

/////////////////////////////////////////////////////////////////////////////////////
//  REQUIRED FOR TRAVERSER MODE ONLY
//  Define the active state for the limit sensor.
//  LOW = When activated, the input is pulled down (ground or 0V).
//  HIGH = When activated, the input is pulled up (typically 5V).
// 
#define LIMIT_SENSOR_ACTIVE_STATE LOW

/////////////////////////////////////////////////////////////////////////////////////
//  Define the active state for the phase switching relays.
//  LOW = When activated, the input is pulled down (ground or 0V).
//  HIGH = When activated, the input is pulled up (typically 5V).
// 
#define RELAY_ACTIVE_STATE HIGH

/////////////////////////////////////////////////////////////////////////////////////
//  Define phase switching behaviour.
// 
//  PHASE_SWITCHING options:
//  AUTO    : When defined, phase will invert at PHASE_SWITCH_START_ANGLE, and revert
//            at PHASE_SWITCH_STOP_ANGLE (see below).
//  MANUAL  : When defined, phase will only invert using the Turn_PInvert command.
//  
//  Refer to the documentation for the full explanation on phase switching, and when
//  it is recommended to change these options.
// 
#define PHASE_SWITCHING AUTO
// #define PHASE_SWITCHING MANUAL

/////////////////////////////////////////////////////////////////////////////////////
//  Define automatic phase switching angle.
// 
//  If PHASE_SWITCHING is set to AUTO (see above), then when the turntable rotates
//  PHASE_SWITCH_ANGLE degrees from home, the phase will automatically invert.
//  Once the turntable reaches a further 180 degrees, the phase will automatically
//  revert.
// 
//  Refer to the documentation for the full explanation on phase switching, and how to
//  define the angle that's relevant for your layout.
// 
#define PHASE_SWITCH_ANGLE 45

/////////////////////////////////////////////////////////////////////////////////////
//  Define the stepper controller in use according to those available below, refer to the
//  documentation for further details on which to select for your application.
// 
//  ULN2003_HALF    : ULN2003 in half step mode
//  ULN2003_FULL    : ULN2003 in full step mode
//  A4988           : Two wire drivers (eg. A4988, DRV8825, TMC2208)
// 
//  NOTE: If you are using a different controller than those already defined, refer to
//  the documentation to define the appropriate configuration variables. Note there are
//  some controllers that are pin-compatible with an existing defined controller, and
//  in those instances, no custom configuration would be required.
// 

#define STEPPER_DRIVER ULN2003_HALF
// #define STEPPER_DRIVER ULN2003_FULL
// #define STEPPER_DRIVER A4988
// 
// If you need to invert the direction of the stepper, uncomment this line. This is likely
// required when using a TMC2208. It may also be required to change the rotation from
// counter clockwise to clockwise when using the ULN2003.
// #define INVERT_DIRECTION
// 
// When using a two wire driver (eg. A4988, DRV8825, TMC2208), it may be necessary to invert
// the step pin. If so, uncomment this line.
// #define INVERT_STEPS
// 
// When using a two wire driver (eg. A4988, DRV8825, TMC2208), it may be necessary to invert
// the enable pin behaviour if you wish to have the stepper driver disabled when not moving.
// #define INVERT_ENABLE

/////////////////////////////////////////////////////////////////////////////////////
//  Define the various stepper configuration items below if the defaults don't suit
//
//  Disable the stepper controller when idling, comment out to leave on. Note that this
//  is handy to prevent controllers overheating, so this is a recommended setting.
#define DISABLE_OUTPUTS_IDLE
// 
//  Define the acceleration and speed settings.
#define STEPPER_MAX_SPEED 200     // Maximum possible speed the stepper will reach
#define STEPPER_ACCELERATION 25   // Acceleration and deceleration rate
// 
// If using a gearing or microstep setup with larger than 32767 steps, you need to set the
// gearing factor appropriately.
// Step counts sent from EX-CommandStation will be multiplied by this number.
#define STEPPER_GEARING_FACTOR 1

/////////////////////////////////////////////////////////////////////////////////////
//  If dealing with steppers that have a lot of slop, it can be beneficial to force
//  rotating in one direction only. Enable one (and one only) of the below options if
//  a single rotation direction is required.
//
//  #define ROTATE_FORWARD_ONLY
//  #define ROTATE_REVERSE_ONLY

/////////////////////////////////////////////////////////////////////////////////////
//  Define the LED blink rates for fast and slow blinking in milliseconds.
// 
//  The LED will alternative on/off for these durations.
#define LED_FAST 100
#define LED_SLOW 500

/////////////////////////////////////////////////////////////////////////////////////
//  ADVANCED OPTIONS
//  In normal circumstances, the settings below should not need to be adjusted unless
//  requested by support ticket, or if Tinkerers or Engineers are working with alternative
//  stepper drivers and motors.
// 
//  Enable debug outputs if required during troubleshooting.
//  Note you can enable debug output interactively in the serial console with <D>
// 
// #define DEBUG
// 
//  Define the maximum number of steps homing and calibration will perform before marking
//  these activities as failed. This step count must exceed a single full rotation in order
//  to be useful.
// #define SANITY_STEPS 10000
// 
//  Define the minimum number of steps the turntable needs to move before the homing sensor
//  deactivates, which is required during the calibration sequence. For high step count
//  setups, this may need to be increased.
// #define HOME_SENSITIVITY 300
// 
//  Override the step count determined by automatic calibration by uncommenting the line
//  below, and manually defining a specific step count.
// #define FULL_STEP_COUNT 4096
// 
//  Override the default debounce delay (in ms) if using mechanical home/limit switches that have
//  "noisy" switch bounce issues.
//  In TRAVERSER mode, default is 10ms as these would typically use mechanical switches.
//  In TURNTABLE mode, default is 0ms as these would typically use hall effect sensors.
// #define DEBOUNCE_DELAY 10
