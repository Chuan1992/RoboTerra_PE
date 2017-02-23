#include <RoboTerraRobot.h>

extern RoboTerraRobot ROBOT;
extern RoboTerraEvent EVENT;

int main(void) {
	init();

#if defined(USBCON)
	USBDevice.attach();
#endif

	// Start the kernal
	Serial.begin(115200); // Communicate w/ app
	attachRoboTerraElectronics(); // Writen by client
	ROBOT.getRobotController()->launch();

	// Kernal Loop
	for (;;) {
		
		ROBOT.getRobotController()->handleRoboCoreEvents();
		ROBOT.getRobotController()->runPeripheralStateMachines();
		ROBOT.getRobotController()->handlePeripheralEvents();
		ROBOT.getRobotController()->checkRoboCoreTimer();
		
		while (ROBOT.getEventQueue()->isEmpty() == false) {
			EVENT = ROBOT.getEventQueue()->dequeue();
			handleRoboTerraEvent(); // Writen by client
		}
		
		// USB Program event
		if (serialEventRun) serialEventRun();
	}
	return 0;
}