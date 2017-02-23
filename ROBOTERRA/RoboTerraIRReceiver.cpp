/****************************************************************************
 RoboTerraIRReceiver.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.
 Inspired by Ken Shirriff's multi-protocol infrared remote 
 library for the Arduino

 Current Revision
 1.5

 Description


 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 9/13/2015    Bai Chen      1.0         Initially create  
 9/15/2015    Bohan Hao     1.1         Change the class name to RoboTerraIRReceiver
 12/22/2015   Bai Chen      1.2         Rewrite for event-driven implementation
 01/03/2016   Bai Chen      1.3         1. Implement event queue to store EVENTs
                                        2. Send event message to UI immediately
 04/05/2016   Zan Li        1.4         Change the position of stateMachineFlag = true
 										in ISR outside of the if statement so that it could
 										detect hash signals    
 07/30/2016   Bai Chen 		1.5 		Remove IR_INTERFERE 							                    
 ****************************************************************************/

#include <RoboTerraIRReceiver.h>

#define TOLERANCE 25  // Percent tolerance in measurements
#define LTOL (1.0 - TOLERANCE/100.) // Lower bound 
#define UTOL (1.0 + TOLERANCE/100.) // Upper bound 

#define USECPERTICK     50  // Microseconds per tick
#define GAP_TICKS       100 // Minimum gap between transmissions 5000 us

// Timing microseconds
#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1690
#define NEC_ZERO_SPACE	560
#define MARK_EXCESS     100 // Marks tend to be 100us too long and spaces 100us too short when received due to sensor lag
#define RC5_T1          889

// IR detector output is active low
#define MARK  			0
#define SPACE 			1

// Message length
#define MESSAGE_BITS    32 // Modified NEC 16-bit address + 16-bit value
#define MIN_RC5_SAMPLES 11 // RC5 

// Invalid parameter
#define INVALID_VALUE   0x7FFF
#define INVALID_ADDRESS 0x7FFF

// IR Receiver States
#define STATE_IDLE     	1
#define STATE_MARK     	2
#define STATE_SPACE    	3
#define STATE_STOP     	4

#define DEVICE_ID       30
#define MSG_LENGTH      6 

/************************ Module Variable ***********************/

static volatile iParameter_t iParameter; // Struct variable used in ISR

/************************** Class Member Functions *************************/ 

void RoboTerraIRReceiver::activate() {
	if (isActive) {
        return;
    }
    RoboTerraElectronics::activate(); // Parent class
    iParameter.bufferIndex = 0;

  	iParameter.state = STATE_IDLE;
  	iParameter.stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

  	sendEventMessage(STATE_IDLE, ACTIVATE, 1, 0);
    generateEvent(ACTIVATE, 1, 0);

	cli(); // Disables all interrupts by clearing the global interrupt mask 
  	TCCR2A = (1 << WGM21); // Selecte Clear Timer on Compare Mode
  	TCCR2B = (1 << CS21); // Prescalor 8, 2 MHz, 0.5 us per tick
  	OCR2A = 100; // Interrupt happens every 50 us
  	TCNT2 = 0; // 8 bit counter ranges from 0 - 255
  	TIMSK2 = (1 << OCIE2A); // Enable Output Compare Match A interrupt
  	sei(); // Enables interrupts by setting the global interrupt mask
}

void RoboTerraIRReceiver::deactivate() {
	if (!isActive) {
        return;
    }
	RoboTerraElectronics::deactivate(); // Parent class 
	iParameter.tickCount = 0;
	iParameter.bufferIndex = 0;

	iParameter.state = STATE_INACTIVE;
  	iParameter.stateMachineFlag = false; // Let Kernal NOT call runStateMachine()

  	sendEventMessage(STATE_INACTIVE, DEACTIVATE, 0, 0);
    generateEvent(DEACTIVATE, 0, 0);

	TIMSK2 = 0; // Disable Output Compare Match A interrupt
}

/*********************** Test Functions **********************/

void RoboTerraIRReceiver::showRawBuffer() {
	Serial.println();
	Serial.print("Raw buffer ");
	Serial.print(rawMessageLength, DEC);
	Serial.println(" recorded intervals! ");
	for (int i = 0; i < rawMessageLength; ++i) {
		if ((i % 2) == 1) {
			Serial.print(rawMessage[i] * USECPERTICK, DEC);
		}
		else {
			Serial.print(-(int)rawMessage[i] * USECPERTICK, DEC);
		}
		Serial.print(" ");
	}
	Serial.println();
	Serial.println();
}

void RoboTerraIRReceiver::attach(int portID) {
  	// Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

  	iParameter.pin = (char)portID;
    pinMode(iParameter.pin, INPUT);
    address = 0;
    value = 0; 
    decodeData = 0;
    
    activate();
}

bool RoboTerraIRReceiver::readStateMachineFlag() {
    return iParameter.stateMachineFlag;
}

void RoboTerraIRReceiver::runStateMachine() {
    // The RoboTerraIRReceiver class is interrupt driven when processing raw incoming data.
    // Kernal runs the state machine whenever raw data is received in buffer for decoding.

	if (isActive && (iParameter.state == STATE_STOP)) {	
		//showRawBuffer(); // debug function
		if (decodeRC5()) { // Try decoding as RC5 protocol
			if ((value == (int)(decodeData)) && (address == (int)(decodeData >> 16))) {
				// IR_MESSAGE_REPEAT 
    			sendEventMessage(STATE_STOP, IR_MESSAGE_REPEAT, value, address);
    			generateEvent(IR_MESSAGE_REPEAT, value, address);
			} 
			else {
				// IR_MESSAGE_RECEIVE
				value = (int)(decodeData);
				address = (int)(decodeData >> 16);
				sendEventMessage(STATE_STOP, IR_MESSAGE_RECEIVE, value, address);
    			generateEvent(IR_MESSAGE_RECEIVE, value, address);
			} 
		}	
		else if (decodeModifiedNEC()) { // Try decoding as modified NEC protocol
			if ((value == (int)(decodeData)) && (address == (int)(decodeData >> 16))) {
				// IR_MESSAGE_REPEAT 
				sendEventMessage(STATE_STOP, IR_MESSAGE_REPEAT, value, address);
    			generateEvent(IR_MESSAGE_REPEAT, value, address);
			} 
			else {
				// IR_MESSAGE_RECEIVE
				value = (int)(decodeData);
				address = (int)(decodeData >> 16);
				sendEventMessage(STATE_STOP, IR_MESSAGE_RECEIVE, value, address);
    			generateEvent(IR_MESSAGE_RECEIVE, value, address);
			} 
		}
		else { // None of above protocol works, possibly IR interference
			//sendEventMessage(STATE_STOP, IR_MESSAGE_INTERFERE, INVALID_VALUE, INVALID_ADDRESS);
    		//generateEvent(IR_MESSAGE_RECEIVE, INVALID_VALUE, INVALID_ADDRESS);
		}

		// Start to listen to new IR message
		iParameter.bufferIndex = 0;
		iParameter.state = STATE_IDLE;
  		iParameter.stateMachineFlag = false;
	}

}

/************************** Private Class Functions *************************/

bool RoboTerraIRReceiver::isIntervalMatched(int measuredTicks, int desiredMicrosecs) {
	int ticksLow = (int)(desiredMicrosecs * LTOL / USECPERTICK);
	int ticksHigh = (int)(desiredMicrosecs * UTOL / USECPERTICK + 1);
	if ((measuredTicks >= ticksLow) && (measuredTicks <= ticksHigh)) {
		return true;
	}
	else {
		return false;
	}
}

bool RoboTerraIRReceiver::decodeModifiedNEC() {
	rawMessageLength = iParameter.bufferIndex;
	rawMessage = iParameter.rawBuffer;

	if (rawMessageLength < 2 * MESSAGE_BITS + 4) { // Not long enough
		return false;
	}

	int index = 1; // Skit first space
	// Check initial mark 
	if (!isIntervalMatched(rawMessage[index], NEC_HDR_MARK + MARK_EXCESS)) {
		return false;
	}
	index++;
	// Check initial space
	if (!isIntervalMatched(rawMessage[index], NEC_HDR_SPACE - MARK_EXCESS)) {
		return false;
	}
	index++;
	long data = 0;
	for (int i = 0; i < MESSAGE_BITS; ++i) {
		// Check data mark
		if (!isIntervalMatched(rawMessage[index], NEC_BIT_MARK + MARK_EXCESS)) {
			return false;
		}
		index++;
		// MSB first
		if (isIntervalMatched(rawMessage[index], NEC_ONE_SPACE - MARK_EXCESS)) {
			data = (data << 1) | 1; // Add 1 to the right
		}
		else if (isIntervalMatched(rawMessage[index], NEC_ZERO_SPACE - MARK_EXCESS)) {
			data <<= 1;	// Add 0 to the right
		}
		else {
			return false;
		}
		index++;
	}
	// If got here, then means success decode
	decodeData = data;
	return true;	
}

bool RoboTerraIRReceiver::decodeRC5() {
	long data = 0;
	int used = 0;
	int offset = 1;  // Skip the first gap space

	rawMessageLength = iParameter.bufferIndex;
	rawMessage = iParameter.rawBuffer;

	if (rawMessageLength < MIN_RC5_SAMPLES + 2) {
		return false;
	}  

	// Check starter bits 
	if (getRCLevel(&offset, &used, RC5_T1) != MARK) {
		return false;
	}
	if (getRCLevel(&offset, &used, RC5_T1) != SPACE) {
		return false;
	}  
	if (getRCLevel(&offset, &used, RC5_T1) != MARK) {
		return false;
	}   

	for (int i = 0; offset < rawMessageLength; i++) {
		int levelA = getRCLevel(&offset, &used, RC5_T1);
		int levelB = getRCLevel(&offset, &used, RC5_T1);

		if ((levelA == SPACE) && (levelB == MARK )) {
			data = (data << 1) | 1;
		}
		else if ((levelA == MARK ) && (levelB == SPACE)) {
			data = (data << 1) | 0;
		}  
		else {
			return false;
		}                                          
	}
	// If get here, it means success decode
	decodeData = data;
	return true;
}

int RoboTerraIRReceiver::getRCLevel(int *offset, int *used, int t1) {
	int width;
	int val;
	int correction;
	int avail;

	if (*offset >= rawMessageLength) {
		return SPACE;  // After end of recorded buffer, assume SPACE.
	}  

	width = rawMessage[*offset];
	val = ((*offset) % 2) ? MARK : SPACE;
	correction = (val == MARK) ? MARK_EXCESS : - MARK_EXCESS;

	if (isIntervalMatched(width, t1 + correction)) {
		avail = 1;
	}
	else if (isIntervalMatched(width, 2 * t1 + correction)) {
		avail = 2;
	}
	else if (isIntervalMatched(width, 3 * t1 + correction)) {
		avail = 3;
	}
	else {
		return -1;
	}                                         

	(*used)++;
	if (*used >= avail) {
		*used = 0;
		(*offset)++;
	}
	return val;
}

void RoboTerraIRReceiver::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend) {
    uint8_t eventMessageLength = 6 + MSG_LENGTH;
    uint8_t eventMessage[eventMessageLength]; // EVENT Message
    
    eventMessage[0] = 0xF0;                   // EVENT Message Begin
    eventMessage[1] = 0x01;                   // EVENT Count
    eventMessage[2] = (uint8_t)DEVICE_ID;     // EVENT Source Device ID
    eventMessage[3] = (uint8_t)iParameter.pin;// EVENT Source Port
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

void RoboTerraIRReceiver::generateEvent(RoboTerraEventType type, int firstData, int secondData) {
    RoboTerraEvent newEvent(this, type, firstData);
    newEvent.setEventData(secondData, 1);
    sourceEventQueue->enqueue(newEvent);
}

/*****************************************************************
 Description
 Timer 2 (8-bit) Output Compare Match A interrupt Service Routine

*****************************************************************/

ISR(TIMER2_COMPA_vect) {
	char sample = digitalRead(iParameter.pin); // Sample every 50 us
	iParameter.tickCount++; // Add one more 50 us tick
	
	if (iParameter.bufferIndex >= MAX_RAW_BUFFER_LENGTH) {
		iParameter.bufferIndex = 0; // Decode return false due to not long enough
		iParameter.state = STATE_STOP; // Buffer overflow
	} 

	switch(iParameter.state) {
		case STATE_IDLE: // In the middle of a gap
			if (sample == MARK) {
				if (iParameter.tickCount < GAP_TICKS)  {  // Not big enough to be a gap
					iParameter.tickCount = 0; // Restart counting
				} else { // Gap just ended; Record duration; Start recording transmission
					iParameter.bufferIndex = 0;
					iParameter.rawBuffer[iParameter.bufferIndex++] = iParameter.tickCount;
					iParameter.state = STATE_MARK;
					iParameter.tickCount = 0;
				}
			}
			break;
		case STATE_MARK: // Timing Mark
			if (sample == SPACE) { // Mark ended; Record time
				iParameter.rawBuffer[iParameter.bufferIndex++] = iParameter.tickCount;
				iParameter.state = STATE_SPACE;
				iParameter.tickCount = 0;
			}
			break;
		case STATE_SPACE:  // Timing Space
			if (sample == MARK) {  // Space just ended; Record time
				iParameter.rawBuffer[iParameter.bufferIndex++] = iParameter.tickCount;
				iParameter.state = STATE_MARK;
				iParameter.tickCount = 0;
			} 
			else { // Space
				if (iParameter.tickCount > GAP_TICKS) { 
					// A long Space, indicating a gap between IR signals
					// Keep counting ticks on Space
					// Switch to STATE_STOP indicating ready for decoding
					iParameter.state = STATE_STOP;
				}
				// Set stateMachineFlag so that Kernal would call runStateMachine()
				iParameter.stateMachineFlag = true;
			}
			break;
		case STATE_STOP: // Waiting
		 	if (sample == MARK) {
		 		iParameter.tickCount = 0;  // Reset gap tick counter
		 	} 
		 	break;
	}
}