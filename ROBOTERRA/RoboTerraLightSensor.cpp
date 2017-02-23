/****************************************************************************
 RoboTerraLightSensor.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.6

 Description
 Light sensor is a digital sensor that detects whether the environment is dark
 or bright. Note that a potentiometer needs to be adjusted to set a default 
 level of light intensity. The working principle of this sensor is that the 
 resistance of a photoresistor decreases with increasing incident light intensity,
 in other words,  it exhibits photoconductivity. 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 08/12/2015   Quincy Liu    1.0         Initially create   
 09/08/2015   Bai Chen      1.1			1. Rename files
                                        2. Complete documentation
                                        3. Create an example LightSensorDemo
 09/15/2015   Bohan Hao     1.2         Change the class name to RoboTerraLightSensor  
 10/17/2015   Bai Chen      1.3         1. Implement state machine
                                        2. Make it subclass of RoboTerraElectronics
                                        3. Add functions called be the framework  
 11/02/2015   Bai Chen      1.4         Change to Active low 
 11/20/2015   Bai chen      1.5         Rewrite for event-driving implementation     
 01/03/2015   Bai Chen      1.6         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active light sensors                          
 ****************************************************************************/

#include <RoboTerraLightSensor.h>

#define DEBOUNCETIME    200 // millisecond

#define LEVEL_DARK      0 // Active low
#define LEVEL_NORMAL    1

#define STATE_BRIGHT    1
#define STATE_DARK      2
#define STATE_DEBOUNCE  3

#define DEVICE_ID       12
#define MSG_LENGTH      4

/***************************** Module Variable *****************************/

unsigned char activeLightSensorNum = 0; // No. of active light sensors

/************************** Class Member Functions *************************/ 

void RoboTerraLightSensor::activate() {
    if (isActive) { // Repeat call
         return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeLightSensorNum++;

    state = STATE_BRIGHT;
    stateMachineFlag = true; // Let Kernal call runStateMachine()
    
    sendEventMessage(STATE_BRIGHT, ACTIVATE, (int)activeLightSensorNum);
    generateEvent(ACTIVATE, (int)activeLightSensorNum);
}

void RoboTerraLightSensor::deactivate() {
     if (!isActive) { // Repeat call
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 
    activeLightSensorNum--;

    state = STATE_INACTIVE;
    stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeLightSensorNum);
    generateEvent(DEACTIVATE, (int)activeLightSensorNum);
}

void RoboTerraLightSensor::attach(int portID) {
    // Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

    // Instance variables
    pin = (char)portID;
    pinMode(pin, INPUT);
    lastLevel = LEVEL_NORMAL;
    lastDebounceMillis = 0;
    count = 0;

    activate();
}

bool RoboTerraLightSensor::readStateMachineFlag() {
    return stateMachineFlag;
}

/*****************************************************************
 Note  
 The darkness here is a relative quantity. Basically, it means that 
 the light intensity is below a certain threshold the sesnor has been 
 preset to. Any light intensity above it is regarded as normally bright.
 Also a debouncing period is implemented to avoid false detection 
 immediately after state transitions.

*****************************************************************/
void RoboTerraLightSensor::runStateMachine() {
    if (state == STATE_DEBOUNCE) {
        if ((millis() - lastDebounceMillis) > DEBOUNCETIME) {
            state = (lastLevel == LEVEL_NORMAL) ? STATE_BRIGHT : STATE_DARK;
        }
    }
    else {
        char currentLevel = digitalRead(pin);
        if(currentLevel != lastLevel) {
            state = STATE_DEBOUNCE; // Debouncing starts when LEVEL changes
            lastLevel = currentLevel; // Update lastLevel

            if (currentLevel == LEVEL_NORMAL) {
                sendEventMessage(STATE_BRIGHT, DARK_LEAVE, count);
                generateEvent(DARK_LEAVE, count);
            }
            else {
                count++; // Dark count
                sendEventMessage(STATE_DARK, DARK_ENTER, count);
                generateEvent(DARK_ENTER, count);
            }
            lastDebounceMillis = millis(); // Record time tick
        }
    }
} 

/************************** Private Class Functions *************************/

void RoboTerraLightSensor::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
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

void RoboTerraLightSensor::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}