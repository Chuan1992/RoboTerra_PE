/****************************************************************************
 RoboTerraEvent.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.1

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 11/16/2015   Bai Chen      1.0         Initially created   
 01/01/2016   Bai Chen 	    1.1 		Event data becomes int array
 
 ****************************************************************************/
 
#include <RoboTerraEvent.h>

/************************** Class Member Functions *************************/ 

RoboTerraEvent::RoboTerraEvent() {
	eventSource = NULL;
	eventType = EVENT_NULL;
	eventData[0] = 0;
	eventData[1] = 0;
}

RoboTerraEvent::RoboTerraEvent(RoboTerraEventSource* source,
							   RoboTerraEventType type,   
                   			   int data) {
	eventSource = source;
	eventType = type;
	eventData[0] = data; 
	eventData[1] = 0;
}

void RoboTerraEvent::setEventData(int dataToSet, int index) {
	if (index < MAX_EVENT_DATA_NUM) {
		eventData[index] = dataToSet;
	}
}

bool RoboTerraEvent::isType(RoboTerraEventType typeToCheck) {
	return (eventType == typeToCheck);
}

bool RoboTerraEvent::isFrom(RoboTerraEventSource &source) {
	return (eventSource == &source);
}

int RoboTerraEvent::getData() {
	return eventData[0];
}

int RoboTerraEvent::getData(int index) {
	if (index < MAX_EVENT_DATA_NUM) {
		return eventData[index];
	}
}

RoboTerraEventType RoboTerraEvent::type() {
	return eventType;
}