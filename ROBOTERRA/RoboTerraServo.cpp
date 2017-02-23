/****************************************************************************
 RoboTerraServo.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.
 Inspired by Michael Margolis's and Philip van Allen's VarSpeedServo library
 for speed control 

 Current Revision
 1.2

 Description
 This is a library for munipulating ROBOTERRA RC Servo.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 09/16/2015   Bai Chen      1.0         Initially create  
 11/17/2015   Bai Chen      1.1         Rewrite for event-driven implementation
 01/03/2016   Bai Chen      1.2         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active servos
 07/28/2016   Bai Chen      1.3         1. Function activate now with initalAngle
                                        2. Function deactivate now with finalAngle
                                        3. Make it deactivated by default
 ****************************************************************************/

#include "RoboTerraServo.h"

#define MIN_PULSE_WIDTH      500   // The shortest pulse (us) sent to a servo  
#define MAX_PULSE_WIDTH      2500  // The longest pulse (us) sent to a servo 
#define REFRESH_INTERVAL     20000 // minumim time (us) to refresh servos 

#define TRIM_TICK            1 // Compensation ticks to trim adjust for digitalWrite delays
#define usToTicks(_us)       ((_us) * 2) // Convert microseconds to tick (assumes prescale of 8)
#define ticksToUs(_tick)     (_tick / 2) // Convert tick to microseconds (assumes prescale of 8) 

#define STATE_STOP           1
#define STATE_MOVE           2  

#define DEVICE_ID            120   
#define MSG_LENGTH           6

/************************ Module Variable ***********************/

static volatile servo_t servos[MAX_SERVO_NUMBER]; // Static array of servo data structures
static volatile char channel; // Not unsigned because -1 used for state transition
unsigned int widthArray[181]; // Store mapping data between angle(0-181) to width(500-2500) 
unsigned char servoCount = 0; // Total number of attached servos (must be in .cpp file)
unsigned char activeServoNum = 0; // No. of active servos

/************************ Public Functions **********************/

/*****************************************************************
 Description
 Constructor of RoboTerraServo class 

 Parameters 
 None
 
 Return 
 None

 Note
 1. Allow maximum of 4 servos
 2. Mapping array is initialized when the first instance is created
*****************************************************************/

RoboTerraServo::RoboTerraServo() {
    if (servoCount == 0) { // Only initialize array when the fist instance is created 
        for (int i = 0; i < 181; i++) {
            widthArray[i] = MIN_PULSE_WIDTH + i * 11 + i / 9; // Mapping from 0-181 to 500-2500
        }
    }
    if (servoCount < MAX_SERVO_NUMBER) {
        servoIndex = servoCount++;
    }
    else { // Too many servos
        servoIndex = INVALID_SERVO_INDEX;
    }
}

void RoboTerraServo::activate(int initialAngle) {
    if (isActive) {
        return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeServoNum++;
    
    generateEvent(ACTIVATE, (int)activeServoNum, initialAngle);
    sendEventMessage(STATE_STOP, ACTIVATE, (int)activeServoNum, initialAngle);

    // Make sure go to initial angle as quickly as posibble
    servos[servoIndex].isInitializing = true;
    servos[servoIndex].initialTicks = usToTicks(angleToPulseWidth(initialAngle));
    
    if (!servos[servoIndex].isInterrupt) {
        startISR(); // No other active pins, thus need to start ISR timer
        servos[servoIndex].isInterrupt = true;
    }
}
  
/*****************************************************************
 Description
 Deactivate a servo by stopping pulsing the attached pin

 Parameters 
 None
 
 Return 
 None
*****************************************************************/

void RoboTerraServo::deactivate(int finalAngle) {
    if (!isActive) {
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 
    if (activeServoNum != 0) {
        activeServoNum--;
    }

    generateEvent(DEACTIVATE, (int)activeServoNum, finalAngle);
    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeServoNum, finalAngle);

    servos[servoIndex].speed = 0; // Skip speed control logic in ISR
    servos[servoIndex].state = STATE_STOP; // Stop servo 
    servos[servoIndex].isInitializing = true;
    servos[servoIndex].initialTicks = usToTicks(angleToPulseWidth(finalAngle));
}

/*****************************************************************
 Description
 Move a servo to a final angle with a controlled speed

 Parameter
 1. Final angle
 Value: an interger ranges from 0 - 180
 2. Speed
 Value: an interger ranges from 1 - 10

 Return 
 None

 Note
 The period during which a servo is moving is featured non-blocking. 
*****************************************************************/

void RoboTerraServo::rotate(int finalAngle, int speed) {
    if (!isActive) {
        return;
    }
    if (finalAngle < 0 || finalAngle > 180) {
        return;
    }
    if (speed < 1 || speed > 10) {
        return;
    }

    if (servos[servoIndex].state == STATE_STOP) {
        if ((usToTicks(angleToPulseWidth(finalAngle)) - TRIM_TICK) == servos[servoIndex].targetTicks) {
            return; // Target already reached
        }
        speedTick = speed * 11 - 7; // Mapping to 4 - 103     
        servos[servoIndex].targetTicks = usToTicks(angleToPulseWidth(finalAngle)) - TRIM_TICK;
        servos[servoIndex].speed = speedTick; // Enable rotate logic
        servos[servoIndex].state = STATE_MOVE;
        
        generateEvent(SERVO_MOVE_BEGIN, finalAngle, speed);
        sendEventMessage(STATE_MOVE, SERVO_MOVE_BEGIN, finalAngle, speed);
    }
}
  
/*****************************************************************
 Description
 Stop a servo at its current angle

 Parameter
 None

 Return 
 None
*****************************************************************/

void RoboTerraServo::pause() {
    if (!isActive) {
        return;
    }
    if (servos[servoIndex].state == STATE_MOVE) {
        servos[servoIndex].state = STATE_STOP;

        // Generate EVENT and send EVENT message are done in runStateMachine() 
    }
}

/*****************************************************************
 Description
 Resume a servo to its target angle

 Parameter
 None

 Return 
 None
*****************************************************************/

void RoboTerraServo::resume() {
    if (!isActive) {
        return;
    }
    if (servos[servoIndex].state == STATE_STOP) {
        if (servos[servoIndex].currentTicks == servos[servoIndex].targetTicks) {
            return; // Target already reached
        }
        servos[servoIndex].speed = speedTick; // Enable rotate logic
        servos[servoIndex].state = STATE_MOVE;

        generateEvent(SERVO_MOVE_BEGIN, pulseWidthToAngle(ticksToUs(servos[servoIndex].targetTicks)), (speedTick + 7) / 11);
        sendEventMessage(STATE_MOVE, SERVO_MOVE_BEGIN, pulseWidthToAngle(ticksToUs(servos[servoIndex].targetTicks)), (speedTick + 7) / 11);
    }
}

/*****************************************************************
 Description
 Assign pin number to attached servos

 Parameters 
 Pin to which a servo is attached 
 Value: SERVO_A, SERVO_B, SERVO_C or SERVO_D
 
 Return 
 None

 Note
 When too many servo attached, UI would give a warning
*****************************************************************/

void RoboTerraServo::attach(int portID) {
    if (servoIndex < MAX_SERVO_NUMBER) {
        // Allocate memomry for RoboTerraEventQueue
        sourceEventQueue = new RoboTerraEventQueue; 

        pinMode(portID, OUTPUT);
        servos[servoIndex].pinNumber = portID;
        servos[servoIndex].state = STATE_STOP;
        servos[servoIndex].stateMachineFlag = false;

        sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeServoNum, 0);
        generateEvent(DEACTIVATE, (int)activeServoNum, 0);
    }
    // More than 4 servo if reach here
}

bool RoboTerraServo::readStateMachineFlag() {
    return servos[servoIndex].stateMachineFlag;
}

void RoboTerraServo::runStateMachine() {
    // Kernal does NOT call it to run State Machine b/c servo is interrupt driven.
    // However, this function is called to send EVENT messeage, genereted in ISR
    if (servos[servoIndex].stateMachineFlag) {
        generateEvent(servos[channel].typeToSend, pulseWidthToAngle(ticksToUs(servos[servoIndex].currentTicks)), 0);
        sendEventMessage(STATE_STOP, servos[channel].typeToSend, pulseWidthToAngle(ticksToUs(servos[servoIndex].currentTicks)), 0);
    }    
    servos[servoIndex].stateMachineFlag = false; // Only genrate and send ONE EVENT
}

/************************** Private Class Functions *************************/

unsigned int RoboTerraServo::angleToPulseWidth(int angle) {
    return widthArray[angle]; // return pulse width in us
}

int RoboTerraServo::pulseWidthToAngle(unsigned int time) {
    for (int i = 1; i < 181; i++) { // Begin from 2nd element
        if(time <= widthArray[i]) {
            if (time <= (widthArray[i - 1] + widthArray[i]) / 2) {
                return i - 1;
            }
            else {
                return i;
            }
        }
    }
    return INVALID_SERVO_ANGLE;
}

void RoboTerraServo::startISR() {
    // Set up Timer 1 (16-bit) and start Interrupt Service Routine  
    cli(); // Disables all interrupts by clearing the global interrupt mask 
    TCCR1A = 0;             // Normal counting mode 
    TCCR1B = _BV(CS11);     // Set prescaler of 8, 2MHz, 0.5us/tick
    TCNT1 = 0;              // Clear the timer counter
    TIFR1 |= _BV(OCF1A);    // OCF1A cleared by writing a logic one to its bit location
    TIMSK1 |= _BV(OCIE1A);  // Enable output compare A match interrupt 
    sei(); // Enables interrupts by setting the global interrupt mask
}

void RoboTerraServo::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend) {
    uint8_t eventMessageLength = 6 + MSG_LENGTH;
    uint8_t eventMessage[eventMessageLength]; // EVENT Message
    
    eventMessage[0] = 0xF0;                   // EVENT Message Begin
    eventMessage[1] = 0x01;                   // EVENT Count
    eventMessage[2] = (uint8_t)DEVICE_ID;     // EVENT Source Device ID
    eventMessage[3] = (uint8_t)servos[servoIndex].pinNumber; // EVENT Source Port
    eventMessage[4] = (uint8_t)MSG_LENGTH;    // Message Length
    eventMessage[5] = (uint8_t)stateToSend;
    eventMessage[6] = (uint8_t)typeToSend;
    eventMessage[7] = (uint8_t)firstDataToSend;
    eventMessage[8] = (uint8_t)(firstDataToSend >> 8);
    eventMessage[9] = (uint8_t)secondDataToSend;
    eventMessage[10] = (uint8_t)(secondDataToSend >> 8);
    eventMessage[11] = 0xFF;                  // End marker

    Serial.write(eventMessage, eventMessageLength);
}

void RoboTerraServo::generateEvent(RoboTerraEventType type, int firstData, int secondData) {
    RoboTerraEvent newEvent(this, type, firstData);
    newEvent.setEventData(secondData, 1);
    sourceEventQueue->enqueue(newEvent);
}

/*****************************************************************
 Description
 Timer 1 (16-bit) Output Compare Match A Interrupt Service Routine

*****************************************************************/

ISR(TIMER1_COMPA_vect) {   
    if (channel == -1) {
        TCNT1 = 0;
    }
    else {
        if (channel < servoCount /*&& servos[channel].isPinActive*/) {
            digitalWrite(servos[channel].pinNumber, LOW);
        }
    }

    channel++;
    if (channel < servoCount) {
        // Logic for function rotate(int finalAngle, int speed)
        // currentTick and targetTick range from 1000 - 5000
        // speed ranges from 1 - 100
        if (servos[channel].speed) {
            if (servos[channel].state == STATE_STOP) {
                servos[channel].speed = 0; // Make sure below section entered ONLY once
                if (servos[channel].currentTicks < servos[channel].targetTicks) { 
                    servos[channel].stateMachineFlag = true; // To send EVENT message
                    
                    servos[channel].typeToSend = SERVO_INCREASE_END;
                }
                else if (servos[channel].currentTicks > servos[channel].targetTicks) {
                    servos[channel].stateMachineFlag = true; // To send EVENT message
                    
                    servos[channel].typeToSend = SERVO_DECREASE_END;
                }
                else { // currentTicks == targetTicks
                       // Intentionally left blank
                       // Events have been fired 
                }
            } else { // servos[channel].state == STATE_MOVE
                     // Increment ticks by speed until we reach the target.
                     // When the target is reached, speed is set to 0 to disable that code.
                if (servos[channel].currentTicks < servos[channel].targetTicks) {
                    servos[channel].currentTicks += servos[channel].speed;
                    if (servos[channel].currentTicks >= servos[channel].targetTicks) {
                        // Going up finished
                        servos[channel].currentTicks = servos[channel].targetTicks;
                        servos[channel].speed = 0;

                        servos[channel].state = STATE_STOP;
                        servos[channel].stateMachineFlag = true; // To send EVENT message
                        
                        servos[channel].typeToSend = SERVO_INCREASE_END;
                    }
                }
                // currentTicks == targetTicks thus decrement excluded from rotate(int, int)
                else { // currentTicks > targetTicks thus decrement
                    servos[channel].currentTicks -= servos[channel].speed;
                    if (servos[channel].currentTicks <= servos[channel].targetTicks) {
                        // Going down finished
                        servos[channel].currentTicks = servos[channel].targetTicks;
                        servos[channel].speed = 0;

                        servos[channel].state = STATE_STOP;
                        servos[channel].stateMachineFlag = true; // To send EVENT message
                        
                        servos[channel].typeToSend = SERVO_DECREASE_END;
                    }
                }
            }
        }
        
        if (servos[channel].isInitializing) { // For initialization
            OCR1A = TCNT1 + servos[channel].initialTicks;
            servos[channel].isInitializing = false;
            servos[channel].currentTicks = servos[channel].initialTicks;
        }
        else { // For control that has begin and end position
            OCR1A = TCNT1 + servos[channel].currentTicks;
        }

        //if (servos[channel].isPinActive) {
            digitalWrite(servos[channel].pinNumber, HIGH);
        //}
    }
    else {
        OCR1A = (unsigned int)usToTicks(REFRESH_INTERVAL);  
        channel = -1; // Start from the first channel next time in
    }
}