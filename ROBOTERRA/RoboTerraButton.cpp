/****************************************************************************
 RoboTerraButton.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.7

 Description
 Push button is the simplest form of a sensor. When pressed down, its output
 signal is logic HIGH (normally 5V). When released from pressing or left 
 untouched, its output signal is logic LOW (normally GND). This library is
 written based on the above features of a push button. 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 08/11/2015   Quincy Liu    1.0         Initially create   
 08/14/2015   Quincy Liu    1.1         Reimplement isPressed()
 08/19/2015   Bai Chen      1.2         1. Fix a bug inPressed()
                                        2. Complete documentation
                                        3. Create an example ButtonDemo
 09/15/2015   Bohan Hao     1.3         Change the class name to RoboTerraButton
 10/17/2015   Bai Chen      1.4         1. Implement state machine
                                        2. Make it subclass of RoboTerraElectronics
                                        3. Add functions called be the framework
 11/02/2015   Bai Chen      1.5         Change to Active low     
 11/17/2015   Bai chen      1.6         Rewrite for event-driving implementation
 01/02/2015   Bai Chen      1.7         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active buttons
 ****************************************************************************/

#include <RoboTerraButton.h>

#define DEBOUNCETIME    200 // millisecond

#define LEVEL_PRESS     0   // Button active low
#define LEVEL_NORMAL    1

#define STATE_NORMAL    1
#define STATE_DOWN      2
#define STATE_DEBOUNCE  3

#define DEVICE_ID       10
#define MSG_LENGTH      4

/***************************** Module Variable *****************************/

unsigned char activeButtonNum = 0; // No. of active buttons

/************************** Class Member Functions *************************/

void RoboTerraButton::activate() {
    if (isActive) { // Repeat call
         return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeButtonNum++;

    state = STATE_NORMAL;
    stateMachineFlag = true; // Let Kernal call runStateMachine()
    
    sendEventMessage(STATE_NORMAL, ACTIVATE, (int)activeButtonNum);
    generateEvent(ACTIVATE, (int)activeButtonNum);
}

void RoboTerraButton::deactivate() {
    if (!isActive) { // Repeat call
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 
    activeButtonNum--;

    state = STATE_INACTIVE;
    stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeButtonNum);
    generateEvent(DEACTIVATE, (int)activeButtonNum);
}

void RoboTerraButton::attach(int portID) {
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

bool RoboTerraButton::readStateMachineFlag() {
    return stateMachineFlag;
}

/*********************************************************************
 Note 
 When a button level is changed, namely the input pin presents a 
 rising or falling edge, a bouncing state is entered and last for
 a certain period of time. Notice that DEBOUNCETIME is critically 
 calibrated in that it has to be long enough to prevent a false 
 transition caught in the bouncing signal and to be short enough
 so that a series of frequent repetitive presses can be recorded. 

*********************************************************************/
void RoboTerraButton::runStateMachine() {
    if (state == STATE_DEBOUNCE) {
        if ((millis() - lastDebounceMillis) > DEBOUNCETIME) {
            state = (lastLevel == LEVEL_NORMAL) ? STATE_NORMAL : STATE_DOWN;
        }
    }
    else {
        char currentLevel = digitalRead(pin);
        if(currentLevel != lastLevel) {
            state = STATE_DEBOUNCE; // Debouncing starts when LEVEL changes
            lastLevel = currentLevel; // Update lastLevel
            
            if (currentLevel == LEVEL_NORMAL) {
                sendEventMessage(STATE_NORMAL, BUTTON_RELEASE, count);
                generateEvent(BUTTON_RELEASE, count);
            }
            else {
                count++; // Press count
                sendEventMessage(STATE_DOWN, BUTTON_PRESS, count);
                generateEvent(BUTTON_PRESS, count);
            }
            lastDebounceMillis = millis(); // Record time tick
        }
    }
} 

/************************** Private Class Functions *************************/

void RoboTerraButton::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
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

void RoboTerraButton::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}