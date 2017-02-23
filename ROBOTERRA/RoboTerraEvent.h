/****************************************************************************
 RoboTerraEvent.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraEvent.cpp 

 ****************************************************************************/

#ifndef RoboTerraEvent_h
#define RoboTerraEvent_h

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraEventSource.h>
#include <RoboTerraShareData.h>
#include <Arduino.h> // NULL pointer

/************************* Defined Constant ********************/

#define MAX_EVENT_DATA_NUM 2

/************************* Actual Class Body ********************/

class RoboTerraEvent {

public:  
    // Functions called by Framework, NOT clients!
    RoboTerraEvent();
    RoboTerraEvent(RoboTerraEventSource* source,
                   RoboTerraEventType type,  
                   int data);
    void setEventData(int dataToSet, int index);

    // API Functions released to clients
    bool isType(RoboTerraEventType typeToCheck);
    bool isFrom(RoboTerraEventSource &sourceToCheck);
    int getData();
    int getData(int index);
    RoboTerraEventType type();

private:
    RoboTerraEventType eventType;
    RoboTerraEventSource* eventSource;
    int eventData[MAX_EVENT_DATA_NUM];
};

#endif