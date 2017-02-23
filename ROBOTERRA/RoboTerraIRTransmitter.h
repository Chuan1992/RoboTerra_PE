/****************************************************************************
 RoboTerraIRTransmitter.h
  Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
  Header file for RoboTerraIRTransmitter.cpp 

 ****************************************************************************/

#ifndef RoboTerraIRTransmitter_h
#define RoboTerraIRTransmitter_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class

/************************* Actual Class Body ********************/

class RoboTerraIRTransmitter : public RoboTerraElectronics {

public:
    // API Functions released to clients
    void activate();
    void deactivate();
    void emit(int value, int address);

protected:
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();
    
private:
    char pin;

    char state;
    bool stateMachineFlag;

    void generateMark(int microseconds);
    void generateSpace(int microseconds);

    // Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData, int secondData);  
};

#endif