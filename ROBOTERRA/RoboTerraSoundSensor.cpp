/****************************************************************************
 RoboTerraSoundSensor.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.5

 Description
 Sound sensor is a digital sensor that is used to detect sound or air vibration.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 08/12/2015   Quincy Liu    1.0         Initially create   
 09/08/2015   Bai Chen      1.1         1. Rename files
                                        2. Complete documentation
                                        3. Reimplement isSoundDetected
 09/15/2015   Bohan Hao     1.2         Change the class name to RoboTerraSoundSensor
 10/16/2015   Bai Chen      1.3         1. Implement a state machine and deadband timer
                                        2. Make it subclass of RoboTerraElectronics
 11/17/2015   Bai chen      1.4         Rewrite for event-driving implementation
 01/03/2015   Bai Chen      1.5         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active sound sensors
 ****************************************************************************/

#include <RoboTerraSoundSensor.h>

#define DEADBANDTIME    200 // millisecond

#define LEVEL_SOUND     0  // Sound is active low
#define LEVEL_NORMAL    1

#define STATE_QUIET     1 
#define STATE_NOISY     2
#define STATE_DEADBAND  3

#define DEVICE_ID       14
#define MSG_LENGTH      4

/***************************** Module Variable *****************************/

unsigned char activeSoundSensorNum = 0; // No. of active sound sensors

/************************** Class Member Functions *************************/

void RoboTerraSoundSensor::activate() {
    if (isActive) { // Repeat call
         return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeSoundSensorNum++;

    state = STATE_QUIET;
    stateMachineFlag = true; // Let Kernal call runStateMachine()

    sendEventMessage(STATE_QUIET, ACTIVATE, (int)activeSoundSensorNum);
    generateEvent(ACTIVATE, (int)activeSoundSensorNum);
}

void RoboTerraSoundSensor::deactivate() {
    if (!isActive) { // Repeat call
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 
    activeSoundSensorNum--;

    state = STATE_INACTIVE;
    stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeSoundSensorNum);
    generateEvent(DEACTIVATE, (int)activeSoundSensorNum);
}
  
void RoboTerraSoundSensor::attach(int portID) {
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

bool RoboTerraSoundSensor::readStateMachineFlag() {
    return stateMachineFlag;
}

/*********************************************************************
 Note 
 Sound Level starts to present when there exists sound or air vibration.
 A clap usually lasts about 2 ms with a HIGH-LOW alternating patern. However,
 during a sound process, the normal level almost never lasts long than 20 ms. 
 Then it goes up to sound level again, which is the key to the state machine. 
 The function is implemented in a way such that the first transition from normal
 to sound level promotes to state to STATE_NOISY. As soon as it sees a 
 transition from sound to normal level, a 20 ms deadband timer fires up. If within
  20ms, there is a transition from normal to sound level again, STATE_SOUND is
  entered. Otherwise, timer expires and state goes back to STATE_QUIET

*********************************************************************/
void RoboTerraSoundSensor::runStateMachine() {
    char currentLevel = digitalRead(pin);
    if(currentLevel != lastLevel) {
        lastLevel = currentLevel;
        if(currentLevel == LEVEL_SOUND) { // LEVEL_NORMAL to LEVEL_SOUND
            if(state == STATE_QUIET) {
                state = STATE_NOISY;
                count++; // Sound count
                sendEventMessage(STATE_NOISY, SOUND_BEGIN, count);
                generateEvent(SOUND_BEGIN, count);
                return;
            }
            else if(state == STATE_DEADBAND) {
                state = STATE_NOISY;
                return;
            }
        }
        else { // LEVEL_SOUND to LEVEL_NORMAL
            if (state == STATE_NOISY) {
                state = STATE_DEADBAND;
                lastDebounceMillis = millis();
                // Let below codes run
            }
        }
    }

    if (state == STATE_DEADBAND) {
        if((millis() - lastDebounceMillis) > DEADBANDTIME) { // Time up
            state = STATE_QUIET;
            sendEventMessage(STATE_QUIET, SOUND_END, count);
            generateEvent(SOUND_END, count);
        }   
    }
} 

/************************** Private Class Functions *************************/

void RoboTerraSoundSensor::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
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

void RoboTerraSoundSensor::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}