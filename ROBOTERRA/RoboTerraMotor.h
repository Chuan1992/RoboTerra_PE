/****************************************************************************
 RoboTerraMotor.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraMotor.cpp 

 ****************************************************************************/

#ifndef RoboTerraMotor_h
#define RoboTerraMotor_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class

/************************* Actual Class Body ********************/

class RoboTerraMotor : public RoboTerraElectronics {

public:
    // API Functions released to clients
    void activate();
    void deactivate();
    void rotate(int speedToSet);
    void reverse();
    void pause();
    void resume();

protected:	
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private:
    char pin;
    char motorSpeedPin;
  	char motorDirPin;
  	char speed;
  	bool direction;

  	char state;
    bool stateMachineFlag;  

    // Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData, int secondData);  
};

#endif