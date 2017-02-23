/****************************************************************************
 RoboTerraEventQueue.h
 	Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Description
 	Header file for RoboTerraEventQueue.cpp 

 ****************************************************************************/

#ifndef RoboTerraEventQueue_h
#define RoboTerraEventQueue_h

/************************* Incldued Dependencies ********************/ 

#include <Arduino.h> // "NULL" defined here
#include <RoboTerraEvent.h>

/************************* Forward Declared Dependencies ********************/ 

/************************* Actual Class Body ********************/

class RoboTerraEventQueue {

public:
	RoboTerraEventQueue();
	~RoboTerraEventQueue();
	int getSize();
	bool isEmpty();
	void clear();
	void enqueue(RoboTerraEvent event);
	RoboTerraEvent dequeue();
    
private:
	// RoboTerraEventQueue implementation data structure
	struct linkedListCell {
	  	RoboTerraEvent eventInCell;
	  	struct linkedListCell *pointerToNext; // Linked list
	};
	typedef struct linkedListCell Cell;

    Cell *head; // Point to the cell to be dequeued
    Cell *tail; // Point to the cell that just enqueued 
    int count;  // Record how many cells in queue
};

#endif