/****************************************************************************
 RoboTerraLED.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboterraLED.cpp 

 ****************************************************************************/

#ifndef RoboTerraLED_h
#define RoboTerraLED_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class

/************************* Actual Class Body ********************/

class RoboTerraLED : public RoboTerraElectronics {

public:
	// API Functions released to clients
    void activate();
    void deactivate();
	void turnOn();
	void turnOff();
	void toggle();
	void slowBlink();
	void fastBlink();
	void slowBlink(int num);
	void fastBlink(int num);
    void stopBlink();

protected:
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private:
	char pin;
	int blinkInterval;
	bool isBlinkFinite;
	int intervalCount;
	int blinkTimes; // User specified times of LED blink
	unsigned long lastMillis;

    char state;
    char lastState;
    bool stateMachineFlag;  

    // Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData);
};

#endif