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
//  Define the pin that the home sensor is connected to, and if it is active low or high.
// 
#define HOME_SENSOR_PIN 2
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
//  Define the various stepper configuration items below if the defaults don't suit
// 
//  The direction the stepper turns when homing, CLOCKWISE or COUNTER_CLOCKWISE.
#define STEPPER_DEFAULT_DIRECTION CLOCKWISE
//  Disable the stepper controller when idling, comment out to leave on. Note that this
//  is handy to prevent controllers overheating, so this is a recommended setting.
#define DISABLE_OUTPUTS_IDLE
//  Define the acceleration and speed settings.
#define STEPPER_MAX_SPEED 100     // Maximum possible speed the stepper will reach
#define STEPPER_ACCELERATION 25   // Acceleration and deceleration rate
#define STEPPER_SPEED 100         // Constant speed for the stepper (eg. when homing)

/////////////////////////////////////////////////////////////////////////////////////
//  If using a dual relay board (or similar), enable phase switching of the bridge
//  tracks.
// 
#define PHASE_SWITCH

/////////////////////////////////////////////////////////////////////////////////////
//  If using phase switching, define the pins in use to control the relays.
// 
#if defined(PHASE_SWITCH)
#define RELAY1_PIN 3
#define RELAY2_PIN 4
#endif
