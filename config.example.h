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
//  Define the active state for the homing sensor.
//  LOW = When activated, the input is pulled down (ground or 0V).
//  HIGH = When activated, the input is pulled up (typically 5V).
// 
#define HOME_SENSOR_ACTIVE_STATE LOW

/////////////////////////////////////////////////////////////////////////////////////
//  Define the active state for the phase switching relays.
//  LOW = When activated, the input is pulled down (ground or 0V).
//  HIGH = When activated, the input is pulled up (typically 5V).
// 
#define RELAY_ACTIVE_STATE HIGH

/////////////////////////////////////////////////////////////////////////////////////
//  Define the stepper controller in use according to those available:
// 
//  ULN2003   : Commonly paired with the 28BYJ-48 unipolar stepper motor (Default)
//  A4988     : Common, inexpensive controller for NEMA17 and other steppers
//  DRV8825   : Common A4988 alternative, somewhat quieter
//  TMC2208   : Very quiet stepper control
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
