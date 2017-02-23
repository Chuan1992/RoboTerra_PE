/****************************************************************************
 RoboTerraRobot.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.2

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 10/02/2015   Bai Chen      1.0         Initially created  
 11/17/2015   Bai chen 		1.1			Rewrite for event-driven implementation
 12/30/2015   Bai chen      1.2         Reorganize framework structure
 ****************************************************************************/

#include <RoboTerraRobot.h>

/************************** Class Member Functions *************************/ 

RoboTerraRobot::RoboTerraRobot() {
    // Allocate memomry for RoboTerraEventQueue
    eventQueue = new RoboTerraEventQueue; 
}

RoboTerraRobot::~RoboTerraRobot() {
	eventQueue->clear();
}

void RoboTerraRobot::equip(RoboTerraRoboCore *controller) {
	robotController = controller;
}

RoboTerraRoboCore* RoboTerraRobot::getRobotController() {
	return robotController;
}

RoboTerraEventQueue* RoboTerraRobot::getEventQueue() {
	return eventQueue;
}