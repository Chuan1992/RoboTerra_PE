/****************************************************************************
 RoboTerraRoboCore.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.4

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 10/04/2015   Bai Chen      1.0         Initially created   
 11/17/2015   Bai Chen 		1.1			Rewrite for event-driven implementation
 12/30/2015   Bai Chen      1.2         Add global variable RoboTerraRobot ROBOT
 01/02/2015   Bai Chen      1.3         1. Simplify EVENTs information flow
 										2. Add launch() and terminate()
 05/10/2016   Zan Li		1.4			Add the attach(RoboTerraElectronics &electronics, 
 										RoboCorePortID portIDX, RoboCorePortID portIDY) 
 										function for Joystick
 07/31/2016	  Bai Chen      1.5         1. Add time function
 										2. Add a couple of print functions 									
 ****************************************************************************/
 
#include <RoboTerraRoboCore.h>
#include <RoboTerraRobot.h> // Put here NOT in .h is to avoid circular #include

#define DEVICE_ID  1
#define MSG_LENGTH 4

/************************* Forward Declaration ********************/

extern RoboTerraRobot ROBOT; // Global variable

/************************** Class Member Functions *************************/ 

RoboTerraRoboCore::RoboTerraRoboCore() {
	state = STATE_COMMENCE;

	// Allocate memomry for RoboTerraEventQueue
    sourceEventQueue = new RoboTerraEventQueue; 

	numOfPortInUse = 0;
	ROBOT.equip(this); // Every instance constuctor would call
}

void RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID) {
	if (state == STATE_OPERATE || state == STATE_TERMINATE) {
		return;
	}

	electronics.attach(portID); // Pass by reference
	portsInUse[numOfPortInUse].ptToElectronicsOnPort = &electronics;
	portsInUse[numOfPortInUse].portID = portID;
	numOfPortInUse++;
}

void RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portIDX, RoboCorePortID portIDY) {
	if (state == STATE_OPERATE || state == STATE_TERMINATE) {
		return;
	}

	electronics.attach(portIDX, portIDY); // Pass by reference
	portsInUse[numOfPortInUse].ptToElectronicsOnPort = &electronics;
	portsInUse[numOfPortInUse].portID = portIDX;
	numOfPortInUse++;
	portsInUse[numOfPortInUse].ptToElectronicsOnPort = &electronics;
	portsInUse[numOfPortInUse].portID = portIDY;
	numOfPortInUse++;
}

void RoboTerraRoboCore::launch() {
	if (state == STATE_OPERATE || state == STATE_TERMINATE) {
		return;
	}
	RoboTerraBrain::launch();

	sendEventMessage(STATE_OPERATE, ROBOCORE_LAUNCH, numOfPortInUse);
	generateEvent(ROBOCORE_LAUNCH, numOfPortInUse);
}

void RoboTerraRoboCore::terminate() {
	if (state == STATE_COMMENCE || state == STATE_TERMINATE) {
		return;
	}
	RoboTerraBrain::terminate();

	sendEventMessage(STATE_TERMINATE, ROBOCORE_TERMINATE, 0);
	generateEvent(ROBOCORE_TERMINATE, 0);
}

void RoboTerraRoboCore::print(char *string) {
	if (state == STATE_COMMENCE || state == STATE_TERMINATE) {
		return;
	}
	if (strlen(string) > 50) {
		return;
	}

	Serial.write(0xF1); // Serial message begin marker
	Serial.write(strlen(string)); // Length of the char array
	Serial.print(string);
	Serial.write(0xFF); // End marker
}

void RoboTerraRoboCore::print(int num) {
	if (state == STATE_COMMENCE || state == STATE_TERMINATE) {
		return;
	}

	unsigned char digit;
	if (abs(num) > 9999) {
		digit = 5;
	}
	else if (abs(num) > 999) {
		digit = 4;
	}
	else if (abs(num) > 99) {
		digit = 3;
	}
	else if (abs(num) > 9) {
		digit = 2;
	}
	else {
		digit = 1;
	}
	if (num < 0) {
		digit++;
	}

	Serial.write(0xF1); // Serial message begin marker
	Serial.write(digit); // Length of the integer 
	Serial.print(num);
	Serial.write(0xFF); // End marker
}

void RoboTerraRoboCore::print(char *string, int num) {
	if (state == STATE_COMMENCE || state == STATE_TERMINATE) {
		return;
	}
	if (strlen(string) > 50) {
		return;
	}

	unsigned char digit;
	if (abs(num) > 9999) {
		digit = 5;
	}
	else if (abs(num) > 999) {
		digit = 4;
	}
	else if (abs(num) > 99) {
		digit = 3;
	}
	else if (abs(num) > 9) {
		digit = 2;
	}
	else {
		digit = 1;
	}
	if (num < 0) {
		digit++;
	}

	Serial.write(0xF1); // Serial message begin marker
	Serial.write(digit + strlen(string));
	Serial.print(string);
	Serial.print(num);
	Serial.write(0xFF); // End marker
}

void RoboTerraRoboCore::time(RoboTerraTimeUnit length) {
	if (state == STATE_COMMENCE || state == STATE_TERMINATE) {
		return;
	}
	if (!isTimerActive) {
		timerLength = (unsigned long)length;
		if (timerLength > TWO_MIN) {
			return;
		}
		nowMillis = millis();
		isTimerActive = true;
	}
}

void RoboTerraRoboCore::runPeripheralStateMachines() {
	if (state == STATE_OPERATE) {
		for (int i = 0; i < numOfPortInUse; i++) {
			RoboTerraElectronics* peripheral = portsInUse[i].ptToElectronicsOnPort;
			if (peripheral->readStateMachineFlag()) {
				peripheral->runStateMachine();
			}
		}
	}
}

void RoboTerraRoboCore::handlePeripheralEvents() {
	if (state == STATE_OPERATE) {
		for (int i = 0; i < numOfPortInUse; i++) {
			RoboTerraElectronics* peripheral = portsInUse[i].ptToElectronicsOnPort;
			while (peripheral->getEventQueue()->isEmpty() == false) {
				RoboTerraEvent event = peripheral->getEventQueue()->dequeue();
				ROBOT.getEventQueue()->enqueue(event);
			}
		}
	}
}

void RoboTerraRoboCore::handleRoboCoreEvents() {
	if (state == STATE_OPERATE) {
		while (sourceEventQueue->isEmpty() == false) {
				RoboTerraEvent event = sourceEventQueue->dequeue();
				ROBOT.getEventQueue()->enqueue(event);
		}
	}
}

void RoboTerraRoboCore::checkRoboCoreTimer() {
	if (state == STATE_OPERATE) {
		if (isTimerActive) {
			if ((millis() - nowMillis) > timerLength) {
				if (timerLength > 999) {
					timerLength = timerLength / 1000; // Convert to seconds
 				}
				sendEventMessage(STATE_OPERATE, ROBOCORE_TIME_UP, timerLength);
				generateEvent(ROBOCORE_TIME_UP, timerLength);
				isTimerActive = false;
			} 
		}
	}
}

/************************** Private Class Functions *************************/

void RoboTerraRoboCore::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
	uint8_t eventMessageLength = 6 + MSG_LENGTH;
    uint8_t eventMessage[eventMessageLength]; // EVENT Message
    
    eventMessage[0] = 0xF0;                   // EVENT Message Begin
    eventMessage[1] = 0x01;                   // EVENT Count
    eventMessage[2] = (uint8_t)DEVICE_ID;     // EVENT Source Device ID
    eventMessage[3] = 0;                      // EVENT Source Port
    eventMessage[4] = (uint8_t)MSG_LENGTH;    // Message Length
    eventMessage[5] = (uint8_t)stateToSend;
    eventMessage[6] = (uint8_t)typeToSend;
    eventMessage[7] = (uint8_t)firstDataToSend;
    eventMessage[8] = (uint8_t)(firstDataToSend >> 8);
    eventMessage[9] = 0xFF;                   // End marker

    Serial.write(eventMessage, eventMessageLength);
}

void RoboTerraRoboCore::generateEvent(RoboTerraEventType type, int firstData) {
	RoboTerraEvent newEvent(this, type, firstData);
    sourceEventQueue->enqueue(newEvent);
}
