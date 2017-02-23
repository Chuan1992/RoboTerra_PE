# Library Content #

## RoboTerraRoboCore class ##

**Public Member Functions**

* void attach(objectName, portID) // Attach electronics (sensors or actuators) to a specific port on RoboCore and activate it

* void terminate() // Terminate RoboCore so that it will not process any EVENT unless RoboCore is reset

## RoboTerraEvent class ##

**Public Member Functions**

* bool EVENT.isType(eventType) // Check if EVENT's type is eventType

* bool EVENT.isFrom(eventSource) // Check if EVENT's source is eventSource

* int EVENT.getData() // Get EVENT data integer at index 0

* int EVENT.getData(1) // Get EVENT data integer at index 1

* RoboTerraEventType EVENT.type() // Get eventType of EVENT

## RoboTerraState class ##

**Public Member Functions**

* void STATE.define(stateName) // Define stateName to be a valid STATE

* void STATE.enter(stateName) // Set current state to stateName

* bool STATE.isIn(stateName) // Check if the current state is stateName

* bool STATE.wasIn(stateName) // Check if the previous state was stateName

## RoboTerraButton class ##

**Public Member Functions**

* void activate() // Activate RoboTerraButton so that EVENT related to RoboTerraButton can be detected

* void deactivate() // Deactivate RoboTerraButton so that EVENT related to RoboTerraButton can no longer be detected

## RoboTerraLightSensor class ##

**Public Member Functions**

* void activate() // Activate RoboTerraLightSensor so that EVENT related to RoboTerraLightSensor can be detected

* void deactivate() // Deactivate RoboTerraLightSensor so that EVENT related to RoboTerraLightSensor can no longer be detected

## RoboTerraTapeSensor class ##

**Public Member Functions**

* void activate() // Activate RoboTerraTapeSensor so that EVENT related to RoboTerraTapeSensor can be detected

* void deactivate() // Deactivate RoboTerraTapeSensor so that EVENT related to RoboTerraTapeSensor can no longer be detected

## RoboTerraSoundSensor class ##

**Public Member Functions**

* void activate() // Activate RoboTerraSoundSensor so that EVENT related to RoboTerraSoundSensor can be detected

* void deactivate() // Deactivate RoboTerraSoundSensor so that EVENT related to RoboTerraSoundSensor can no longer be detected

## RoboTerraLED class ##

**Public Member Functions**

* void activate() // Activate RoboTerraLED so that Command Functions related to RoboTerraLED would take action once called

* void deactivate() // Deactivate RoboTerraLED so that Command Functions related to RoboTerraLED would not take action once called
 
* void turnOn() // Turn on RoboTerraLED or stop a blinking one and make it stay on

* void turnOff() // Turn off RoboTerraLED or stop a blinking one and make it stay off

* void slowblink(int times) // Make RoboTerraLED blink slowly (once per second) for a specified number of times

* void slowblink() // Make RoboTerraLED blink slowly (once per second) for an unlimited number of times or switch from fastBlink() to slowBlink()

* void fastBlink(int times) // Make RoboTerraLED blink fast (once per quarter second) for a specified number of times

* void fastBlink() // Make RoboTerraLED blink fast (once per quarter second) for an unlimited number of times or switch from slowBlink() to fastBlink()

* void toggle() // Turn on RoboTerraLED if it is already off or turn off RoboTerraLED if it is already on

* void stopBlink() // Stop RoboTerraLED from blinking and restore its state (either on or off) before blinking

## RoboTerraServo class ##

**Public Member Functions**

* void activate() // Activate RoboTerraServo so that Command Functions related to RoboTerraServo would take action once called

* void deactivate() // Deactivate RoboTerraServo so that Command Functions related to RoboTerraServo would not take action once called

* void rotate(int degree, int speed) // Rotate RoboTerraServo to a specified target angle at a specified speed

* void pause() // Pause RoboTerraServo during its rotation motion

* void resume() // Resume RoboTerraServo to reach the specified target angle before it is paused

## RoboTerraMotor class ##

**Public Member Functions**

* void activate() // Activate RoboTerraMotor so that EVENT related to RoboTerraMotor can be detected

* void deactivate() // Deactivate RoboTerraMotor so that EVENT related to RoboTerraMotor can no longer be detected

* void rotate(int speedToSet) // Rotate RoboTerraMotor at a specified speed

* void reverse() // Rotate RoboTerraMotor at the speed set in rotate(int speedToSet) reversely

* void pause() // Pause RoboTerraMotor during its rotation motion

* void resume() // Resume RoboTerraMotor to rotate at the specified speed and direction before it is paused

## RoboTerraIRTransmitter class ##

**Public Member Functions**

* void activate() // Activate RoboTerraIRTransmitter so that EVENT related to RoboTerraIRTransmitter can be detected

* void deactivate() // Deactivate RoboTerraIRTransmitter so that EVENT related to RoboTerraIRTransmitter can no longer be detected

* void emit(int address, int value) // Send a 16-bit integer as an address and a 16-bit integer as a value through IR communication

## RoboTerraIRReceiver class ##

**Public Member Functions**

* void activate() // Activate RoboTerraIRReceiver so that EVENT related to RoboTerraIRReceiver can be detected

* void deactivate() // Deactivate RoboTerraIRReceiver so that EVENT related to RoboTerraIRReceiver can no longer be detected

## RoboTerraJoystick class ##

**Public Member Functions**

* void activate() // Activate RoboTerraJoystick so that EVENT related to RoboTerraJoystick can be detected

* void deactivate() // Deactivate RoboTerraJoystick so that EVENT related to RoboTerraJoystick can no longer be detected

## RoboTerraAccelerometer class ##

**Public Member Functions**

* void activate() // Activate RoboTerraAccelerometer so that EVENT related to RoboTerraAccelerometer can be detected

* void deactivate() // Deactivate RoboTerraAccelerometer so that EVENT related to RoboTerraAccelerometer can no longer be detected