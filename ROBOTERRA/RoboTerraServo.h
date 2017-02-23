/****************************************************************************
  RoboTerraServo.h
  Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
  Header file for RoboTerraServo.cpp 

 ****************************************************************************/

#ifndef RoboTerraServo_h
#define RoboTerraServo_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h>
#include <RoboTerraElectronics.h> // Parent class

#define MAX_SERVO_NUMBER     4   // Servo A B C D 
#define INVALID_SERVO_INDEX  4   // Valid servo indice are 0, 1, 2, 3
#define INVALID_SERVO_ANGLE  181 // Valid servo angle 0 - 180

// Servo data structure
typedef struct {
	//bool isPinActive;
	bool isInterrupt;
	unsigned char pinNumber;
	unsigned int currentTicks;
	unsigned int targetTicks;
	unsigned char speed; // Incremental ticks
	unsigned int initialTicks; // Used only when activated
	bool isInitializing;
	char state;
    bool stateMachineFlag;
    RoboTerraEventType typeToSend;
} servo_t;

/************************* Actual Class Body ********************/

class RoboTerraServo : public RoboTerraElectronics {

public:
	// API Functions released to clients
	RoboTerraServo();
	void activate(int initialAngle);
	void deactivate(int finalAngle);
	void rotate(int finalAngle, int speed);
	void pause();
	void resume();

protected:
	// Called by RoboTerraRoboCore::attach(RoboTerraElectronics &electronics, RoboCorePortID portID)
    void attach(int portID);

    // Called by RoboTerraRoboCore::runPeripheralStateMachine()
    bool readStateMachineFlag();
    void runStateMachine();

private:
	unsigned char servoIndex;
	unsigned int speedTick;

	unsigned int angleToPulseWidth(int angle);
	int pulseWidthToAngle(unsigned int time);
	void startISR();

	// Virtual functions in RoboTerraEventSource
    void sendEventMessage(char stateToSend, RoboTerraEventType typeToSend, int firstDataToSend, int secondDataToSend);
    void generateEvent(RoboTerraEventType type, int firstData, int secondData);
};

#endif