/****************************************************************************
 RoboTerraIRTransmitter.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.4

 Description
 This is a library for sending modifed NEC messages via IR transmitter.
 Only Pin 3 (OC2B) can be used. 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 09/14/2015    Bai Chen     1.0         Initially create  
 09/15/2015    Bohan Hao    1.1         Change the name of class to RoboTerraIRTransmitter
 01/03/2016    Bai Chen     1.2	        1. Rewrite for event-driven implementation
                                        2. Implement event queue to store EVENTs
 									                      3. Send event message to UI immediately
 04/11/2016    Zan Li       1.3         Add sendEventMessage and generateEvent function in the emit function
 07/30/2016    Bai Chen     1.4         1. Switch the arguments in emit function
                                        2. Make it deactivated by default
 ****************************************************************************/

#include <RoboTerraIRTransmitter.h>

#define PWM_TICK 210 //16MHz 210 x 2 = 420 ticks in period, 38.1KHz            
#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1690
#define NEC_ZERO_SPACE	560

#define MESSAGE_BITS    32 // Modified NEC 16-bit address + 16-bit value
#define TOPBIT 0x80000000  // A Mask to send message bit by bit MSB first

// IR Transmitter States
#define STATE_ACTIVE   	1

#define DEVICE_ID       110
#define MSG_LENGTH      6 

/************************** Class Member Functions *************************/ 

void RoboTerraIRTransmitter::activate() {
    // Activate IR transmitter with Phase Correct PWM output from Timer 2.
    // Activate the transmitter would deactivate the receiver by design.
 	// OC2B is the only pin (Pin 3) that can be used.
    if (isActive) {
        return;
    }
    RoboTerraElectronics::activate();

    state = STATE_ACTIVE;
  	stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

  	sendEventMessage(STATE_ACTIVE, ACTIVATE, 1, 0);
    generateEvent(ACTIVATE, 1, 0);

    TIMSK2 = 0; // Disable Timer 2 interrupt 
  	TCCR2A = _BV(WGM20); // Phase correct PWM Mode
  	TCCR2B = _BV(WGM22) | _BV(CS20); // No prescalar 16MHz
  	OCR2A = PWM_TICK; // Used to control PWM frequency
  	OCR2B = PWM_TICK / 2; // Used to control PWM duty cycle 50%
  	digitalWrite(pin, LOW); // Stay low when no PWM output
}

void RoboTerraIRTransmitter::deactivate() {
    // Deactivate the transmitter would NOT recover the receiver.
    // The receiver needs to be reactivated for normal use.
    if (!isActive) {
        return;
    }
    RoboTerraElectronics::deactivate(); // Parent class 

    state = STATE_INACTIVE;
  	stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

  	sendEventMessage(STATE_INACTIVE, DEACTIVATE, 0, 0);
    generateEvent(DEACTIVATE, 0, 0);
}

void RoboTerraIRTransmitter::emit(int value, int address) {
  	// Send a 16-bit integer as an address and a 16-bit 
  	// interger as a value thorugh IR communication 
  	long data = address;
  	data = (data << 16) + value; 

    if (state == STATE_ACTIVE) {
  	  	generateMark(NEC_HDR_MARK);
  	    generateSpace(NEC_HDR_SPACE);
  	    for (int i = 0; i < MESSAGE_BITS; i++) {
  	       	if (data & TOPBIT) {
  	      	    generateMark(NEC_BIT_MARK);
  	      	    generateSpace(NEC_ONE_SPACE);
  	        } 
  	        else {
                generateMark(NEC_BIT_MARK);
	      	    generateSpace(NEC_ZERO_SPACE);
  	        }
  	        data <<= 1;
  	    }
  		generateMark(NEC_BIT_MARK);
        generateSpace(0);

        sendEventMessage(STATE_ACTIVE, IR_MESSAGE_EMIT, value, address);
        generateEvent(IR_MESSAGE_EMIT, value, address);
    }	
}

void RoboTerraIRTransmitter::attach(int portID) {
	  // Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

    if (portID == IR_TRAN) {
        pin = portID;
        pinMode(pin, OUTPUT);

        // IR Transmitter cannot work with IR Receiver at the same time
        sendEventMessage(STATE_INACTIVE, DEACTIVATE, 0, 0);
        generateEvent(DEACTIVATE, 0, 0);
    }
  	
}

bool RoboTerraIRTransmitter::readStateMachineFlag() {
    return stateMachineFlag;
}

void RoboTerraIRTransmitter::runStateMachine() {
    // Intentionally left blank
}

/************************** Private Class Functions *************************/

void RoboTerraIRTransmitter::generateMark(int microseconds) {
  	// Sends an IR mark for the specified number of microseconds.
    // The mark output is modulated at the PWM frequency.
  	TCCR2A |= _BV(COM2B1); // Enable Pin 3 PWM Output
  	if (microseconds > 0) {
  	    delayMicroseconds(microseconds);
  	} 
}

void RoboTerraIRTransmitter::generateSpace(int microseconds) {  	
  	// Sends an IR space for the specified number of microseconds.
  	// A space is no output, so the PWM output is disabled.
  	TCCR2A &= ~(_BV(COM2B1)); // Disable Pin 3 PWM Output
  	if (microseconds > 0) {
        delayMicroseconds(microseconds);
  	} 
}

void RoboTerraIRTransmitter::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend) {
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
    eventMessage[9] = (uint8_t)secondDataToSend;
    eventMessage[10] = (uint8_t)(secondDataToSend >> 8);
    eventMessage[11] = 0xFF;                  // End marker

    Serial.write(eventMessage, eventMessageLength);
}

void RoboTerraIRTransmitter::generateEvent(RoboTerraEventType type, int firstData, int secondData) {
    RoboTerraEvent newEvent(this, type, firstData);
    newEvent.setEventData(secondData, 1);
    sourceEventQueue->enqueue(newEvent);
}