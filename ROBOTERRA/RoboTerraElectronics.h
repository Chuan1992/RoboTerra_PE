/****************************************************************************
 RoboTerraElectronics.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraElectronics.cpp 

 ****************************************************************************/

#ifndef RoboTerraElectronics_h
#define RoboTerraElectronics_h

#define STATE_INACTIVE	0

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraEventSource.h> // Parent class 
#include <RoboTerraShareData.h>  
#include <RoboTerraEventQueue.h>  

/************************* Forward Declared Dependencies ********************/

/************************* Actual Class Body ********************/

class RoboTerraElectronics : public RoboTerraEventSource {

public:

    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    virtual void attach(int portID);
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portIDX, RoboCorePortID portIDY)
    virtual void attach(int portIDX, int portIDY);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    virtual bool readStateMachineFlag() = 0;
    virtual void runStateMachine() = 0; 

protected:
	bool isActive;

	virtual void activate();
    virtual void deactivate();
	
private:
  	
};

#endif