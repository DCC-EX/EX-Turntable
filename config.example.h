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
#define STEPPER_CONTROLLER ULN2003

/////////////////////////////////////////////////////////////////////////////////////
//  Define the stepper motor in use according to those available:
// 
//  ULN2003COMBO  : 28BYJ-48 stepper with the ULN2003 (Default)
//  NEMA17        : 
//  NEMA23        :
//  Other?        :
// 
//  NOTE: If you are using a different controller than those already defined, refer to
//  the documentation to define the appropriate configuration variables. Note there are
//  some controllers that are pin-compatible with an existing defined controller, and
//  in those instances, no custom configuration would be required.
// 
#define STEPPER_MOTOR ULN2003COMBO

/////////////////////////////////////////////////////////////////////////////////////
//  Define the various stepper configuration items below if the defaults don't suit
// 
//  If you need to adjust the full rotation step count after calibration, uncomment the
//  following line and set the correct number here.
// #define FULLSTEPS 1234
// 
//  The direction the stepper turns when homing, CLOCKWISE or COUNTER_CLOCKWISE.
#define STEPPER_DIRECTION CLOCKWISE
// 
//  Disable the stepper controller when idling, comment out to leave on. Note that this
//  is handy to prevent controllers overheating, so this is a recommended setting.
#define DISABLE_OUTPUTS_IDLE
// 
//  Define the acceleration and speed settings.
#define STEPPER_MAX_SPEED 100     // Maximum possible speed the stepper will reach
#define STEPPER_ACCELERATION 25   // Acceleration and deceleration rate
#define STEPPER_SPEED 100         // Constant speed for the stepper (eg. when homing)

/////////////////////////////////////////////////////////////////////////////////////
//  Define the LED blink rates for fast and slow blinking in milliseconds.
// 
//  The LED will alternative on/off for these durations.
#define LED_FAST 100
#define LED_SLOW 500

/////////////////////////////////////////////////////////////////////////////////////
//  Define the delay between calibration positions in milliseconds (default 15000).
// 
//  Depending on the speed settings, this may need to be longer or shorter to allow
//  time to validate the positions are correct.
#define CALIBRATION_DELAY 15000

/////////////////////////////////////////////////////////////////////////////////////
//  Enable debug outputs if required during troubleshooting.
// 
// #define DEBUG
