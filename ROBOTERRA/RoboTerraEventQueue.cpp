/****************************************************************************
 RoboTerraEventQueue.cpp
 Copyright (c) 2015 ROBOTERRA, Inc. All rights reserved.

 Current Revision
 1.0

 Description
 This is a part of RoboTerra robotics programming framework.
 The queue is implemented with a dynamically growing linked 
 list for the sake of saving memory on microcontroller. 

 History
 When         Who           Revision    What/Why            
 ---------    ----------    --------    ---------------
 9/26/2015    Bai Chen      1.0         Initially created   
 
 ****************************************************************************/

#include <RoboTerraEventQueue.h>

#define MAX_SIZE 255 // Max RoboTerraEvents allowed in queue to preserve memory

/************************** Class Member Functions *************************/ 

RoboTerraEventQueue::RoboTerraEventQueue() {
	count = 0;
	head = tail = NULL;
}

RoboTerraEventQueue::~RoboTerraEventQueue() {
	clear();
}

int RoboTerraEventQueue::getSize() {
	return count;
}

bool RoboTerraEventQueue::isEmpty() {
	return count == 0;
}

void RoboTerraEventQueue::clear() {
	while (count > 0) {
		dequeue();
	}
}

void RoboTerraEventQueue::enqueue(RoboTerraEvent event) {
	// Make sure queue size not exceed MAX_SIZE
	if (getSize() == MAX_SIZE) {
		return;
	}

	Cell *copy = new Cell;
	copy->eventInCell = event;
	copy->pointerToNext = NULL;
	if (head == NULL) { // Special case when first cell enqueued
		head = copy;
	}
	else {
		tail->pointerToNext = copy;
	}
	tail = copy;
	count++;
}

RoboTerraEvent RoboTerraEventQueue::dequeue() {
	RoboTerraEvent eventToReturn;
	if (isEmpty()) {
		return eventToReturn;
	}
	Cell *copy = head;
	eventToReturn = copy->eventInCell;
	head = copy->pointerToNext;
	if (head == NULL) { // Special case when last cell dequeued
		tail = NULL;
	}
	delete copy;
	count--;
	return eventToReturn;
}