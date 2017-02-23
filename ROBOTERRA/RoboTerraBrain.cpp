/****************************************************************************
 RoboTerraBrain.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.0

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 10/4/2015    Bai Chen      1.0         Initially created   
 
 ****************************************************************************/

#include <RoboTerraBrain.h>

/************************** Class Member Functions *************************/

void RoboTerraBrain::launch() {
	state = STATE_OPERATE;
}

void RoboTerraBrain::terminate() {
	state = STATE_TERMINATE;
}