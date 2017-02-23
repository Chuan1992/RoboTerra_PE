/****************************************************************************
 RoboTerraJoystick.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraJoystick.cpp 

 ****************************************************************************/

#ifndef RoboTerraJoystick_h
#define RoboTerraJoystick_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class   

/************************* Actual Class Body ********************/

class RoboTerraJoystick : public RoboTerraElectronics {

public:
	// API Functions released to clients
	void activate();
	void deactivate();

protected:
	// Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portIDX, RoboCorePortID portIDY)
    void attach(int portIDX, int portIDY);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private: 
	char pinX, pinY;
	unsigned long lastDebounceMillis;
	int xValue;	
	int yValue;
	int lastXValue;
	int lastYValue;

	char state;
	bool stateMachineFlag;

	// Mapping the raw data to -5 to 5
	int handleRawAnalogValue(int valueInput);

	// Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend);
    void sendEventMessageHelper(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, char pin);
    void generateEvent(RoboTerraEventType type, int firstData);

};

#endif