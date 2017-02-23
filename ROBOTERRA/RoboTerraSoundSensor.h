/****************************************************************************
 RoboTerraSoundSensor.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraSoundSensor.cpp 

 ****************************************************************************/

#ifndef RoboTerraSoundSensor_h
#define RoboTerraSoundSensor_h

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraElectronics.h> // Parent class

/************************* Actual Class Body ********************/

class RoboTerraSoundSensor : public RoboTerraElectronics {

public:
	// API Functions released to clients
    void activate();
    void deactivate();

protected:
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private:
	char pin;
	char lastLevel;
	unsigned long lastDebounceMillis;
    int count; // Counting for both sound begin and end
	
	char state;
	bool stateMachineFlag;

    // Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData);
};

#endif