/****************************************************************************
 RoboTerraJoystick.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.1

 Description
 This is the library for controlling ROBOTERRA Joystick.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 04/06/2016   Zan Li    	1.0         Initially created
 07/30/2016   Bai Chen      1.1         Add portID checking logic 
 ****************************************************************************/

#include <RoboTerraJoystick.h>

#define DEBOUNCETIME	50 // millisecond

#define STATE_NORMAL	1
#define STATE_DEBOUNCE	2

#define DEVICE_ID		40
#define MSG_LENGTH		4

/***************************** Module Variable *****************************/

unsigned char activeJoystickNum = 0; // No. of active Joysticks

/************************** Class Member Functions *************************/

void RoboTerraJoystick::activate() {
	if(isActive) {
		return;
	}
	RoboTerraElectronics::activate();
	activeJoystickNum++;

	state = STATE_NORMAL;
	stateMachineFlag = true; // let kernal call runStateMachine()

	sendEventMessage(STATE_NORMAL, ACTIVATE, (int)activeJoystickNum);
	generateEvent(ACTIVATE, (int)activeJoystickNum);
}

void RoboTerraJoystick::deactivate() {
	if(!isActive) {
		return;
	}
	RoboTerraElectronics::deactivate();
	activeJoystickNum--;

	state = STATE_INACTIVE;
	stateMachineFlag = false;

	sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeJoystickNum);
	generateEvent(DEACTIVATE, (int)activeJoystickNum);
}

void RoboTerraJoystick::attach(int portIDX, int portIDY) {
	if (portIDX != AI_1 && portIDX != AI_2) {
        return;
    }
    if (portIDY != AI_1 && portIDY != AI_2) {
        return;
    }

    // Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

    // Instance variables
    pinX = (char)portIDX;
    pinY = (char)portIDY;
    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);

    lastDebounceMillis = 0;
    xValue = 0;
    yValue = 0;
    lastXValue = 0;
    lastYValue = 0;
    
    activate();
}

bool RoboTerraJoystick::readStateMachineFlag() {
	return stateMachineFlag;
}

void RoboTerraJoystick::runStateMachine() {
    if (state == STATE_DEBOUNCE) {
        if((millis() - lastDebounceMillis) > DEBOUNCETIME) {
            state = STATE_NORMAL;
                
            xValue = handleRawAnalogValue(analogRead(pinX));
            yValue = handleRawAnalogValue(analogRead(pinY));
            if(xValue != lastXValue) {
                sendEventMessage(STATE_NORMAL, JOYSTICK_X_UPDATE, xValue);
                generateEvent(JOYSTICK_X_UPDATE, xValue);

                lastXValue = xValue;
            }
            if(yValue != lastYValue) {
                sendEventMessage(STATE_NORMAL, JOYSTICK_Y_UPDATE, yValue);
                generateEvent(JOYSTICK_Y_UPDATE, yValue);
                    
                lastYValue = yValue;
            }
        }
    }
    else {
        // Joystick X and Y value
        xValue = handleRawAnalogValue(analogRead(pinX));
        yValue = handleRawAnalogValue(analogRead(pinY));
        if(xValue != lastXValue || yValue != lastYValue) {
            state = STATE_DEBOUNCE;
            lastDebounceMillis = millis(); // Record time tick
        }
    } 
}

/************************** Private Class Functions *************************/

int RoboTerraJoystick::handleRawAnalogValue(int valueInput) { // map the analog value to -5 to 5
    if(valueInput > 920) return 5;
	else if(valueInput > 840) return 4;
	else if(valueInput > 760) return 3; 
    else if(valueInput > 680) return 2; 
    else if(valueInput > 600) return 1; 
    else if(valueInput > 400) return 0; 
    else if(valueInput > 320) return -1;
    else if(valueInput > 240) return -2;
    else if(valueInput > 160) return -3; 
    else if(valueInput > 80)  return -4;
    else return -5; 
    
}

void RoboTerraJoystick::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
    switch(typeToSend) {
        case ACTIVATE:
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinX);
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinY);
        break;
        case DEACTIVATE:
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinX);
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinY);
        break;
        case JOYSTICK_X_UPDATE:
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinX);
        break;
        case JOYSTICK_Y_UPDATE:
            sendEventMessageHelper(stateToSend, typeToSend, firstDataToSend, pinY);
        break;
    }
}

void RoboTerraJoystick::sendEventMessageHelper(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, char pin) {
    uint8_t eventMessageLength = 6 + MSG_LENGTH;
    uint8_t eventMessage[eventMessageLength]; // EVENT Message
    
    eventMessage[0] = 0xF0;                   // EVENT Message Begin
    eventMessage[1] = 0x01;                   // EVENT Count
    eventMessage[2] = (uint8_t)DEVICE_ID;     // EVENT Source Device ID    
    eventMessage[3] = (uint8_t)pin;           // EVENT Source Port
    eventMessage[4] = (uint8_t)MSG_LENGTH;    // Message Length
    eventMessage[5] = (uint8_t)stateToSend;
    eventMessage[6] = (uint8_t)typeToSend;
    eventMessage[7] = (uint8_t)firstDataToSend;
    eventMessage[8] = (uint8_t)(firstDataToSend >> 8);
    eventMessage[9] = 0xFF;                  // End marker

    Serial.write(eventMessage, eventMessageLength);
}

void RoboTerraJoystick::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}