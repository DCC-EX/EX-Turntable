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
//  ULN2003_HALF_CW     : ULN2003 in half step mode, clockwise homing/calibration
//  ULN2003_HALF_CCW    : ULN2003 in half step mode, counter clockwise homing/calibration
//  ULN2003_FULL_CW     : ULN2003 in full step mode, clockwise homing/calibration
//  ULN2003_FULL_CCW    : ULN2003 in full step mode, counter clockwise homing/calibration
//  TWO_WIRE            : Two wire drivers (eg. A4988, DRV8825)
//  TWO_WIRE_INV        : Two wire drivers (eg. A4988, DRV8825), with enable pin inverted
// 
//  NOTE: If you are using a different controller than those already defined, refer to
//  the documentation to define the appropriate configuration variables. Note there are
//  some controllers that are pin-compatible with an existing defined controller, and
//  in those instances, no custom configuration would be required.
// 
#define STEPPER_DRIVER ULN2003_HALF_CW
// #define STEPPER_DRIVER ULN2003_HALF_CCW
// #define STEPPER_DRIVER ULN2003_FULL_CW
// #define STEPPER_DRIVER ULN2003_FULL_CCW
// #define STEPPER_DRIVER TWO_WIRE
// #define STEPPER_DRIVER TWO_WIRE_INV

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
// #define HOME_SENSITIVITY 150
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
