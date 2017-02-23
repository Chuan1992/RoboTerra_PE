/****************************************************************************
 ROBOTERRA.h
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.1

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 10/04/2015   Bai Chen      1.0         Initially created 
 07/30/2016   Bai Chen 		1.1			1. Remove RoboTerraState.h
										2. Remove RoboTerraAccelerometer.h
 
 ****************************************************************************/

#ifndef ROBOTERRA_h
#define ROBOTERRA_h

#include <RoboTerraRoboCore.h>
#include <RoboTerraButton.h>
#include <RoboTerraTapeSensor.h>
#include <RoboTerraLightSensor.h>
#include <RoboTerraSoundSensor.h>
#include <RoboTerraLED.h>
#include <RoboTerraServo.h>
#include <RoboTerraMotor.h>
#include <RoboTerraIRReceiver.h>
#include <RoboTerraIRTransmitter.h>
#include <RoboTerraJoystick.h>

#include <RoboTerraRobot.h>
#include <RoboTerraShareData.h>

RoboTerraRobot ROBOT;
RoboTerraEvent EVENT;

#endif