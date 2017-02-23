/****************************************************************************
 RoboTerraLED.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.7

 Description
 This is a library for controlling ROBOTERRA LED.

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ----------------
 08/11/2015   Quincy Liu    1.0         Initially create
 09/14/2015   Bai Chen      1.1         Implement fastBlink and slowBlink   
 09/15/2015   Bohan Hao     1.2         Change the class name to RoboTerraLED
 10/18/2015   Bai Chen      1.3         Implement state machine for non-blocking blinks
 11/17/2015   Bai Chen      1.4         Rewrite for event-driven implementation
 12/30/2015   Bai Chen      1.5         Add stopBlink() function
 01/02/2015   Bai Chen      1.6         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately 
                                        3. Keep track No. of active LEDs
 01/15/2015   Bai Chen      1.7         Implement RoboTerraLED state diagram                                        
 ****************************************************************************/
 
#include <RoboTerraLED.h>

#define SLOW_BLINK_INTERVAL 500 // Blink once per second
#define FAST_BLINK_INTERVAL 125 // Blink 4 times per second

#define STATE_OFF           1
#define STATE_ON            2
#define STATE_BLINK         3 

#define DEVICE_ID           100
#define MSG_LENGTH          4

/***************************** Module Variable *****************************/

unsigned char activeLEDNum = 0; // No. of active LEDs
unsigned char onLEDNum = 0;     // No. of on LEDs

/************************** Class Member Functions *************************/ 

void RoboTerraLED::activate() {
    if (isActive) {
        return;
    }
    RoboTerraElectronics::activate(); // Parent class
    activeLEDNum++;

    state = STATE_OFF;
    stateMachineFlag = false; // Let kernal NOT call runStateMachine()
    
    sendEventMessage(STATE_OFF, ACTIVATE, (int)activeLEDNum);
    generateEvent(ACTIVATE, (int)activeLEDNum);
}

void RoboTerraLED::deactivate() {
    if (!isActive) {
        return;
    }
    digitalWrite(pin, LOW);
    RoboTerraElectronics::deactivate(); // Parent class 
    activeLEDNum--;

    state = STATE_INACTIVE;
    stateMachineFlag = false; // To let kernal NOT call runStateMachine()

    sendEventMessage(STATE_INACTIVE, DEACTIVATE, (int)activeLEDNum);
    generateEvent(DEACTIVATE, (int)activeLEDNum);
}

void RoboTerraLED::turnOn() {
    if (!isActive) {
        return;
    }
    if (state == STATE_OFF) {
        digitalWrite(pin, HIGH);
        onLEDNum++;

        state = STATE_ON;
        stateMachineFlag = false;
        
        sendEventMessage(STATE_ON, LED_TURNON, (int)onLEDNum);
        generateEvent(LED_TURNON, (int)onLEDNum);
    }
    else if (state == STATE_BLINK) {
        digitalWrite(pin, HIGH);

        state = STATE_ON;
        stateMachineFlag = false;

        sendEventMessage(STATE_ON, BLINK_END, intervalCount / 2);
        generateEvent(BLINK_END, intervalCount / 2);
        sendEventMessage(STATE_ON, LED_TURNON, (int)onLEDNum);
        generateEvent(LED_TURNON, (int)onLEDNum);
    }
}

void RoboTerraLED::turnOff() {
    if (!isActive) {
        return;
    }
    if (state == STATE_ON) {
        digitalWrite(pin, LOW);
        onLEDNum--;

        state = STATE_OFF;
        stateMachineFlag = false;

        sendEventMessage(STATE_OFF, LED_TURNOFF, (int)onLEDNum);
        generateEvent(LED_TURNOFF, (int)onLEDNum);
    }
    else if (state == STATE_BLINK) {
        digitalWrite(pin, LOW);
        onLEDNum--;

        state = STATE_OFF;
        stateMachineFlag = false;

        sendEventMessage(STATE_OFF, BLINK_END, intervalCount / 2);
        generateEvent(BLINK_END, intervalCount / 2);
        sendEventMessage(STATE_OFF, LED_TURNOFF, (int)onLEDNum);
        generateEvent(LED_TURNOFF, (int)onLEDNum);
    }
}

void RoboTerraLED::toggle() {
    if (!isActive) {
        return;
    }
    if (state == STATE_BLINK) {
        return;
    }

    switch (state) {
        case STATE_OFF:
            digitalWrite(pin, HIGH);
            onLEDNum++;

            state = STATE_ON;
            stateMachineFlag = false;
            
            sendEventMessage(STATE_ON, LED_TURNON, (int)onLEDNum);
            generateEvent(LED_TURNON, (int)onLEDNum);
        break;
        
        case STATE_ON:
            digitalWrite(pin, LOW);
            onLEDNum--;

            state = STATE_OFF;
            stateMachineFlag = false;

            sendEventMessage(STATE_OFF, LED_TURNOFF, (int)onLEDNum);
            generateEvent(LED_TURNOFF, (int)onLEDNum);
        break; 
    }
}

void RoboTerraLED::slowBlink() {
    if (!isActive) {
        return;
    }
    if (state != STATE_BLINK) {
        digitalWrite(pin, LOW);
        blinkInterval = SLOW_BLINK_INTERVAL;
        isBlinkFinite = false;
        intervalCount = 0;
        blinkTimes = -1; // Make sure STATE_BLINK stays
        lastMillis = millis(); // Start recording time

        lastState = state;
        state = STATE_BLINK;
        stateMachineFlag = true; // Let Kernal call runStateMachine()

        sendEventMessage(STATE_BLINK, SLOWBLINK_BEGIN, 0);
        generateEvent(SLOWBLINK_BEGIN, 0); // 0 -> Indefinite blink
    }  
    else {
        if (isBlinkFinite == false) {
            if (blinkInterval == FAST_BLINK_INTERVAL) {
                blinkInterval = SLOW_BLINK_INTERVAL;

                sendEventMessage(STATE_BLINK, SLOWBLINK_BEGIN, 0);
                generateEvent(SLOWBLINK_BEGIN, 0); // 0 -> Indefinite blink
            }
        }
    }  
}

void RoboTerraLED::fastBlink() {
    if (!isActive) {
        return;
    }
    if (state != STATE_BLINK) {
        digitalWrite(pin, LOW);
        blinkInterval = FAST_BLINK_INTERVAL;
        isBlinkFinite = false;
        intervalCount = 0;
        blinkTimes = -1; // Make sure STATE_BLINK stays
        lastMillis = millis(); // Start recording time

        lastState = state;
        state = STATE_BLINK;
        stateMachineFlag = true; // Let Kernal call runStateMachine()
        
        sendEventMessage(STATE_BLINK, FASTBLINK_BEGIN, 0);
        generateEvent(FASTBLINK_BEGIN, 0); // 0 -> Indefinite blink        
    }  
    else {
        if (isBlinkFinite == false) {
            if (blinkInterval == SLOW_BLINK_INTERVAL) {
                blinkInterval = FAST_BLINK_INTERVAL;

                sendEventMessage(STATE_BLINK, FASTBLINK_BEGIN, 0);
                generateEvent(FASTBLINK_BEGIN, 0); // 0 -> Indefinite blink  
            }
        }
    }
}

void RoboTerraLED::slowBlink(int num) {
    if (!isActive) {
        return;
    }
    if (state != STATE_BLINK) {
        digitalWrite(pin, LOW);
        blinkInterval = SLOW_BLINK_INTERVAL;
        isBlinkFinite = true;
        intervalCount = 0;
        blinkTimes = num;
        lastMillis = millis(); // Start recording time

        lastState = state; // Remember which state comming into STATE_BLINK
        state = STATE_BLINK;
        stateMachineFlag = true; // Let Kernal call runStateMachine()
        
        sendEventMessage(STATE_BLINK, SLOWBLINK_BEGIN, blinkTimes);
        generateEvent(SLOWBLINK_BEGIN, blinkTimes);
    }  
}

void RoboTerraLED::fastBlink(int num) {
    if (!isActive) {
        return;
    }
    if (state != STATE_BLINK) {
        digitalWrite(pin, LOW);
        blinkInterval = FAST_BLINK_INTERVAL;
        isBlinkFinite = true;
        intervalCount = 0;
        blinkTimes = num;
        lastMillis = millis(); // Start recording time
        
        lastState = state; // Remember which state comming into STATE_BLINK
        state = STATE_BLINK;
        stateMachineFlag = true; // Kernal calls runStateMachine()
        
        sendEventMessage(STATE_BLINK, FASTBLINK_BEGIN, blinkTimes);
        generateEvent(FASTBLINK_BEGIN, blinkTimes);
    }  
}

void RoboTerraLED::stopBlink() {
    if (!isActive) {
        return;
    }
    if (state == STATE_BLINK) {
        switch (lastState) {
            case STATE_ON:
                digitalWrite(pin, HIGH);

                state = STATE_ON;
                stateMachineFlag = false; 

                sendEventMessage(STATE_ON, BLINK_END, intervalCount / 2);
                generateEvent(BLINK_END, intervalCount / 2);
            break;
            case STATE_OFF:
                digitalWrite(pin, LOW);
                
                state = STATE_OFF;
                stateMachineFlag = false;

                sendEventMessage(STATE_OFF, BLINK_END, intervalCount / 2);
                generateEvent(BLINK_END, intervalCount / 2);
            break; 
        }
    }
}

void RoboTerraLED::attach(int portID) {
    // Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

    pin = (char)portID;
    pinMode(pin, OUTPUT);
    blinkInterval = 0;
    isBlinkFinite = false;
    intervalCount = 0;
    blinkTimes = 0;
    lastMillis = 0;

    activate();
}

bool RoboTerraLED::readStateMachineFlag() {
    return stateMachineFlag;
}

void RoboTerraLED::runStateMachine() {
    if (state == STATE_BLINK) {
        if (millis() - lastMillis > blinkInterval) {
            if ((intervalCount * isBlinkFinite) == 2 * blinkTimes) { // Finite blinks finish
                
                switch (lastState) {
                    case STATE_ON:
                        digitalWrite(pin, HIGH);

                        state = STATE_ON;
                        stateMachineFlag = false; 

                        sendEventMessage(STATE_ON, BLINK_END, intervalCount / 2);
                        generateEvent(BLINK_END, intervalCount / 2);
                    break;
                    case STATE_OFF:
                        digitalWrite(pin, LOW);
                        
                        state = STATE_OFF;
                        stateMachineFlag = false;

                        sendEventMessage(STATE_OFF, BLINK_END, intervalCount / 2);
                        generateEvent(BLINK_END, intervalCount / 2);
                    break; 
                }
            }
            else { // Blink Not finished 
                digitalWrite(pin, !digitalRead(pin));
                intervalCount++;
                lastMillis = millis(); // Update recorded time
            }
        }
    }
}

/************************** Private Class Functions *************************/

void RoboTerraLED::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
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

void RoboTerraLED::generateEvent(RoboTerraEventType type, int firstData) {
    RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}