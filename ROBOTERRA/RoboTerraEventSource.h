/****************************************************************************
 RoboTerraEventSource.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraEventSource.cpp 

 ****************************************************************************/

#ifndef RoboTerraEventSource_h
#define RoboTerraEventSource_h

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraShareData.h>

/************************* Forward Declared Dependencies ********************/ 

class RoboTerraEventQueue;

/************************* Actual Class Body ********************/

class RoboTerraEventSource {

public:  
	// Called by RoboTerraRoboCore::handlePeripheralEvents()
    RoboTerraEventQueue* getEventQueue();

protected:
	RoboTerraEventQueue* sourceEventQueue; // Used by grandson class

	virtual void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend);
	virtual void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend);
    virtual void generateEvent(RoboTerraEventType type, int firstData);
    virtual void generateEvent(RoboTerraEventType type, int firstData, int secondData);
    
private:
    
};

#endif