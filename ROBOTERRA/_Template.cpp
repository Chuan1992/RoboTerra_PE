#include "ROBOTERRA.h"

/*--------------------- STEP 1 ---------------------*/
// Name your RoboCore and all electronics attached it.
// These names are considered as eventSource of EVENT. 

RoboTerraRoboCore tom;

/*--------------------- STEP 2 ---------------------*/
// Attach electronics to physical ports on RoboCore.   

void attachRoboTerraElectronics() {

}

/*--------------------- STEP 3 ---------------------*/
/* Design your robot algorithm by handling each EVENT. 
 * Based on the source, type and data of each EVENT, 
 * you can organize your loigc and form program flow.
 * Call Command Functions of instances you defined in
 * STEP 1 for your robot to take physical actions. 
 * Calling Command Functions can result in new EVENT. 
 */

void handleRoboTerraEvent() {
    if (EVENT.isType(ROBOCORE_LAUNCH)) {
        // Initialize robot by calling Comand Function  
    
    }
    
    // Your robot code goes below


}