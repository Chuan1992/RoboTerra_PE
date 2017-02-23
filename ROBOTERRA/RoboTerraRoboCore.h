/****************************************************************************
 RoboTerraRoboCore.h
 	Copyright (c) 2015 , Inc. All rights reserved.

 Description
 	Header file for RoboTerraRoboCore.cpp 

 ****************************************************************************/

#ifndef RoboTerraRoboCore_h
#define RoboTerraRoboCore_h

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraBrain.h> // Parent class
#include <RoboTerraElectronics.h>

/************************* Defined Constant ********************/

#define PORT_NUM 22 // A total of 18 ports on RoboCore V1.4

/************************* Actual Class Body ********************/

class RoboTerraRoboCore : public RoboTerraBrain {

public:
    // API Functions released to clients
    RoboTerraRoboCore();
    void attach(RoboTerraElectronics &electronis, RoboCorePortID portID);
    void attach(RoboTerraElectronics &electronis, RoboCorePortID portIDX, RoboCorePortID portIDY); // For Joystick & Accelerometer, 2 ports
    void launch();
    void terminate();
    void print(char *string);
    void print(int num);
    void print(char *string, int num);
    void time(RoboTerraTimeUnit length);

    // Called by Kernal Loop
    void runPeripheralStateMachines();
    void handlePeripheralEvents();
    void handleRoboCoreEvents();
    void checkRoboCoreTimer();

private:
    typedef struct {
        RoboTerraElectronics* ptToElectronicsOnPort; 
        RoboCorePortID portID;   
    } RoboCorePort;

    // Store information of all ports in use
  	RoboCorePort portsInUse[PORT_NUM]; // Allocate memory for max number of ports
  	int numOfPortInUse;

    unsigned long timerLength;
    unsigned long nowMillis;
    bool isTimerActive;

    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData);
};

#endif