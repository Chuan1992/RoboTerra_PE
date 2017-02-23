/****************************************************************************
 RoboTerraBrain.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraBrain.cpp 

 ****************************************************************************/

#ifndef RoboTerraBrain_h
#define RoboTerraBrain_h

#define STATE_COMMENCE  0
#define STATE_OPERATE   1
#define STATE_TERMINATE 2

/************************* Incldued Dependencies ********************/ 

#include <RoboTerraEventSource.h> // Parent class 

/************************* Forward Declared Dependencies ********************/ 

/************************* Actual Class Body ********************/

class RoboTerraBrain : public RoboTerraEventSource {

public:

protected:
    char state;

    virtual void launch();
    virtual void terminate();
	
private:
    
};

#endif