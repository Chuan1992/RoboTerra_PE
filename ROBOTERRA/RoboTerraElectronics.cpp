/****************************************************************************
 RoboTerraElectronics.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.3

 Description
 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 10/01/2015   Bai Chen      1.0         Initially created   
 11/17/2015   Bai Chen 		1.1			Rewrite for event-driven implementation
 01/02/2015   Bai Chen      1.2         Simplify EVENTs information flow
 04/07/2016   Zan Li		1.3			1. Add one more virtual function attach(portIDX, portIDY)
										2. Change pure virtual function attach(portID) to virtual function
 ****************************************************************************/

#include <RoboTerraElectronics.h>

void RoboTerraElectronics::activate() {
	isActive = true;
}

void RoboTerraElectronics::deactivate() {
	isActive = false;
}

void RoboTerraElectronics::attach(int portID) {
	// Implementation in children class
}

void RoboTerraElectronics::attach(int portIDX, int portIDY) {
	// Implementation in children class	
}