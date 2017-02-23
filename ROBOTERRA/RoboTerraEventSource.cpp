/****************************************************************************
 RoboTerraEventSource.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.0

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 12/30/2015   Bai Chen      1.0         Initially created   
 
 ****************************************************************************/
 
#include <RoboTerraEventSource.h>

/************************** Class Member Functions *************************/ 

RoboTerraEventQueue* RoboTerraEventSource::getEventQueue() {
    return sourceEventQueue;
}

void RoboTerraEventSource::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend) {
	// Implementation in children class
}

void RoboTerraEventSource::sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend) {
	// Implementation in children class
}

void RoboTerraEventSource::generateEvent(RoboTerraEventType type, int firstData) {
	// Implementation in children class
}

void RoboTerraEventSource::generateEvent(RoboTerraEventType type, int firstData, int secondData) {
	// Implementation in children class
}