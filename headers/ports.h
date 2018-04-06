/*
 *
 *
 * 			         Ports: Competition Robot 2018
 *						Competition Legal 2018
 *
 *
 */

/********************************/
/*    Pulse Width Modulation    */
/********************************/

/* Drive Train */
#define PWM_LEFT_DRIVE 0
#define PWM_RIGHT_DRIVE 1

#define PWM_LIFT 2

#define PWM_ARM_ROTATE 3

#define PWM_WINCH 4


/******************************/
/*    Digital Input/Output    */
/******************************/

/* Drive Train Encoders */
#define DIO_ENC_LEFT_CHANNEL_A 9
#define DIO_ENC_LEFT_CHANNEL_B 8
#define DIO_ENC_RIGHT_CHANNEL_A 6
#define DIO_ENC_RIGHT_CHANNEL_B 7

/* IR Sensors */
#define DIO_ANTENNA_LEFT 5
#define DIO_ANTENNA_BLOCK 4
#define DIO_ANTENNA_FRONT 0

/* Limit Switches */
#define DIO_LIMIT_TOP 1
#define DIO_LIMIT_BOTTOM 2

#define DIO_LIMIT_ARM_TOP 3
#define DIO_LIMIT_ARM_BOTTOM 14

/**********************************/
/*    Pneumatic Control Module    */
/**********************************/

/* Cube Grabber */
#define PCM_CLAW_LEFT 0
#define PCM_CLAW_RIGHT 1

/*********************************/
/*    Human Interface Devices    */
/*********************************/

/* XBox Controllers */
#define XBOX_PRIMARY 0
#define XBOX_SECONDARY 1
