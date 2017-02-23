/****************************************************************************
 RoboTerraShareData.h
    Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

Current Revision
1.2

 Description
    This is a header files designed to store Port ID of RoboCore, 
    all EVENT types generated from event source devices,
    which could be included in multiple files.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 11/18/2015   Bai Chen      1.0         Initially created   
 01/02/2015   Bai Chen      1.1         Rename EVENT types
 05/16/2016   Zan Li        1.2         1. Add Joystick & Accelerometer EVENT types
                                        2. Add RoboCore V1.1, V1.2, V1.3, V1.5 RoboCorePortID
 07/30/2016   Bai Chen      1.3         Remove IR_INTERFER and EVENT types of acceleromter 
 07/31/2016   Bai Chen      1.4         Add RoboTerraTimeUnit                                       
 ****************************************************************************/

#ifndef RoboTerraShareData_h
#define RoboTerraShareData_h

typedef enum { 

    // // RoboCore V1.1
    // SERIAL    = 1,
    // DIO_1     = 3,
    // MTR_B     = 6,
    // MTR_A     = 5,
    // SRV_A     = 7,
    // SRV_B     = 8,
    // SRV_C     = 9,
    // SRV_D     = 10,
    // DIO_2     = 11,
    // DIO_3     = 12,
    // DIO_4     = 13,
    // LED_A     = 14,
    // LED_B     = 15,
    // LED_C     = 16,
    // ADIO_5    = 17,
    // ADIO_4    = 18,
    // ADIO_3    = 19,
    // ADIO_2    = 20,
    // ADIO_1    = 21,

    // // RoboCore V1.2
    // TX_RX     = 1,
    // IR_TRAN   = 3,
    // MOTOR_B   = 6,
    // MOTOR_A   = 5,
    // SERVO_A   = 7,
    // SERVO_B   = 8,
    // SERVO_C   = 9,
    // SERVO_D   = 10,
    // DIO_1     = 11,
    // DIO_2     = 12,
    // DIO_3     = 13,
    // DIO_4     = 14,
    // DIO_5     = 15,
    // DIO_6     = 16,
    // DIO_7     = 17,
    // DIO_8     = 18,
    // DIO_9     = 19,
    // AI_1      = 20,
    // AI_2      = 21,

    // RoboCore V1.3, V1.4 & V1.5
    TX_RX     = 1,  
    POWER     = 2,
    IR_TRAN   = 3,
    USB       = 4,    
    MOTOR_A   = 5, 
    MOTOR_B   = 6, 
    SERVO_D   = 7, 
    SERVO_C   = 8, 
    SERVO_B   = 9, 
    SERVO_A   = 10,
    DIO_1     = 11,
    DIO_2     = 12,
    DIO_3     = 13,
    DIO_4     = 19,
    DIO_5     = 18, 
    DIO_6     = 17,
    DIO_7     = 16,
    DIO_8     = 15,
    DIO_9     = 14,
    AI_2      = 20,
    AI_1      = 21,

    // RoboTerra Shield V2.2
    D0        = 0,
    D1        = 1,
    D2        = 2,
    D3        = 3,
    D4        = 4,
    D5        = 5,
    D6        = 6,
    D7        = 7,
    D8        = 8,
    D9        = 9, 
    SRVD10    = 10,
    SRVD11    = 11,
    SRVD12    = 12,
    SRVD13    = 13

} RoboCorePortID;

typedef enum {
    // System level event
    EVENT_NULL              = 0,

    // RoboTerraRoboCore event
    ROBOCORE_LAUNCH         = 1,
    ROBOCORE_TERMINATE      = 2,
    ROBOCORE_TIME_UP        = 3,
    
    // All RoboTerraElectronics 
    DEACTIVATE              = 10,
    ACTIVATE                = 11, 
    
    // RoboTerraButton 
    BUTTON_PRESS            = 100,
    BUTTON_RELEASE          = 101,
    
    // RoboTerraTape
    BLACK_TAPE_ENTER        = 102,
    BLACK_TAPE_LEAVE        = 103,
    
    // RoboTerraLightSensor
    DARK_ENTER              = 104,
    DARK_LEAVE              = 105,
    
    // RoboTerraSoundSensor
    SOUND_BEGIN             = 106,
    SOUND_END               = 107,

    // RoboTerraIRReceiver
    IR_MESSAGE_REPEAT       = 108,
    IR_MESSAGE_RECEIVE      = 109,

    // RoboTerraLED
    LED_TURNON              = 200,
    LED_TURNOFF             = 201,
    SLOWBLINK_BEGIN         = 202,
    FASTBLINK_BEGIN         = 203,
    BLINK_END               = 204,
    
    // RoboTerraServo
    SERVO_MOVE_BEGIN        = 205,
    SERVO_INCREASE_END      = 206,
    SERVO_DECREASE_END      = 207, 

    // RoboTerraMotor
    MOTOR_SPEED_CHANGE      = 208,
    MOTOR_SPEED_ZERO        = 209,
    MOTOR_REVERSE           = 210,

    // RoboTerraIRTransmitter
    IR_MESSAGE_EMIT         = 211,

    // RoboTerraJoystick
    JOYSTICK_X_UPDATE       = 111,
    JOYSTICK_Y_UPDATE       = 112

} RoboTerraEventType;

typedef enum {
    TENTH_SEC   = 100,
    QUARTER_SEC = 250,
    HALF_SEC    = 500,
    ONE_SEC     = 1000,
    TWO_SEC     = 2000,
    THREE_SEC   = 3000,
    FOUR_SEC    = 4000,
    FIVE_SEC    = 5000,
    SIX_SEC     = 6000,
    SEVEN_SEC   = 7000,
    EIGHT_SEC   = 8000,
    NINE_SEC    = 9000,
    TEN_SEC     = 10000,
    HALF_MIN    = 30000,
    ONE_MIN     = 60000,
    TWO_MIN     = 120000
} RoboTerraTimeUnit;

#endif