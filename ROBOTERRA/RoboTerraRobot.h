/****************************************************************************
 RoboTerraRobot.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraRobot.cpp 

 ****************************************************************************/

#ifndef RoboTerraRobot_h
#define RoboTerraRobot_h

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraRoboCore.h>
#include <RoboTerraEventQueue.h>

/************************* Actual Class Body ********************/

class RoboTerraRobot {

public:   
    RoboTerraRobot();
    ~RoboTerraRobot();
    void equip(RoboTerraRoboCore *controller);
    RoboTerraRoboCore* getRobotController();
    RoboTerraEventQueue* getEventQueue();

private:
    RoboTerraRoboCore *robotController;
    RoboTerraEventQueue *eventQueue; 
};

#endif