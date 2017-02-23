/****************************************************************************
 RoboTerraIRReceiver.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.
 	Inspired by Ken Shirriff's multi-protocol infrared remote 
 	library for the Arduino 

 Description
 	Header file for RoboTerraIRReceiver.cpp 

 ****************************************************************************/

#ifndef RoboTerraIRReceiver_h
#define RoboTerraIRReceiver_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class

#define MAX_RAW_BUFFER_LENGTH 100

// Information for the interrupt service routine
typedef struct {
    char pin; // IR receiver pin
    unsigned int tickCount; // tick count of 50uS
    unsigned int rawBuffer[MAX_RAW_BUFFER_LENGTH]; // raw data buffer
    char bufferIndex; // rawBuffer index

    char state;
    bool stateMachineFlag;
} 
iParameter_t;

/************************* Actual Class Body ********************/

class RoboTerraIRReceiver : public RoboTerraElectronics {

public:
    // API Functions released to clients
    void activate();
    void deactivate();

    // Test Function
    void showRawBuffer();

protected:  
    // Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private:
    int address;
    int value;
    long decodeData;
    int rawMessageLength;
    volatile unsigned int *rawMessage;
    
    bool isIntervalMatched(int measuredTicks, int desiredMicrosecs);
    bool decodeModifiedNEC();
    bool decodeRC5();
    int getRCLevel(int *offset, int *used, int t1);

    // Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData, int secondData);  
};

#endif