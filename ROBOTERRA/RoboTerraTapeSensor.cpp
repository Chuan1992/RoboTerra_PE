/****************************************************************************
 RoboTerraTapeSensor.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.6

 Description
 IR reflectance sensor is a digital sensor that is used for object detection 
 and color-based line tracking. The package incldues an IR emitting LED and
 phototransistor.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 08/12/2015   Quincy Liu    1.0         Initially create  
 09/08/2015   Bai Chen		1.1			1. Rename files and add functions
                                        2. Complete documentation
                                        3. Create an example IRReflectanceSensorDemo
 09/15/2015   Bohan Hao     1.2         Change the class name to RoboTerraTapeSensor                                     
 10/17/2015   Bai Chen      1.3         1. Implement state machine
                                        2. Make it subclass of RoboTerraElectronics
                                        3. Add functions called be the framework
 11/02/2015   Bai Chen      1.4         Change to Active low    
 11/17/2015   Bai Chen      1.5         Rewrite for event-driven implementation 
 01/03/2016   Bai Chen      1.6         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active tape sensors
 ****************************************************************************/

#include <RoboTerraTapeSensor.h>

#define DEBOUNCETIME 	200 // millisecond

#define LEVEL_TAPE      0 // Active low
#define LEVEL_NORMAL    1  

#define STATE_OFFTAPE   1
#define STATE_ONTAPE    2
#define STATE_DEBOUNCE  3 

#define DEVICE_ID       11
#define MSG_LENGTH      4

/***************************** Module Variable *****************************/

unsigned char activeTapeSensorNum = 0; // No. of active tape sensors

/************************** Class Member Functions *************************/

void RoboTerraTapeSensor::activate() {
    if (isActive) { // Repeat call
         return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeTapeSensorNum++;

    state = STATE_OFFTAPE;
    stateMachineFlag = true; // Let Kernal call runStateMachine()

    sendEventMessage(STATE_OFFTAPE, ACTIVATE, (int)activeTapeSensorNum);
    generateEvent(ACTIVATE, (int)activeTapeSensorNum);
}

void RoboTerraTapeSensor::deactivate() {
    if (!isActive) { // Repeat call
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 
    activeTapeSensorNum--;

    state = STATE_INACTIVE;
    stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeTapeSensorNum);
    generateEvent(DEACTIVATE, (int)activeTapeSensorNum);
}

void RoboTerraTapeSensor::attach(int portID) {
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

bool RoboTerraTapeSensor::readStateMachineFlag() {
    return stateMachineFlag;
}

/*********************************************************************
 Note  
 Only the transition from an IR reflective material to a non-
 reflective material is detected. A typical example is the edge 
 between a white board and a black tape where the intensity of 
 the reflected infrared drops significantly when moving from 
 a white board to a black tape. In addition, a debouncing algorithm
 similar to Button class is implemented in order to prevent consecutive 
 false detections of an rising edge due to the fact that the signal is 
 bouncing inbetween transition.

*********************************************************************/
void RoboTerraTapeSensor::runStateMachine() {
    if (state == STATE_DEBOUNCE) {
        if ((millis() - lastDebounceMillis) > DEBOUNCETIME) {
            state = (lastLevel == LEVEL_NORMAL) ? STATE_OFFTAPE : STATE_ONTAPE;
        }
    }
    else {
        char currentLevel = digitalRead(pin);
        if(currentLevel != lastLevel) {
            state = STATE_DEBOUNCE; // Debouncing starts when LEVEL changes
            lastLevel = currentLevel;

            if (currentLevel == LEVEL_NORMAL) {
                sendEventMessage(STATE_OFFTAPE, BLACK_TAPE_LEAVE, count);
                generateEvent(BLACK_TAPE_LEAVE, count);
            }
            else {
                count++; // Tape count
                sendEventMessage(STATE_ONTAPE, BLACK_TAPE_ENTER, count);
                generateEvent(BLACK_TAPE_ENTER, count);
            }
            lastDebounceMillis = millis();
        }
    }
} 

/************************** Private Class Functions *************************/

void RoboTerraTapeSensor::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
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

void RoboTerraTapeSensor::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}