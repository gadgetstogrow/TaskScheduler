/**********************************************************************************
 *Project:  Arduino_Task_Scheduler.ino, Copyright 2019, Kevin Gagnon
 *Contact:  Kevin Gagnon @GadgetsToGrow
 *URL:      https://www.hackster.io/GadgetsToGrow
 *Date:     3-17-2019
 *Credit:   Based on Alan Burlison's Task Scheduler Library
 *          The following is Copyright Alan Burlison, 2011
 *          Original Source Code:	http://bleaklow.com/files/2010/Task.tar.gz
 *          Original Reference:		http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html
 *Purpose:  Evaluate simple task scheduling on the Arduino Uno. This example covers
 *          the following types of tasks:
 *
 *          - Task - Runs repeatedly
 *          - Timed Task - Runs at a specified rate (100ms, 1000ms, etc.)
 *          - TriggeredTask	- Runs when triggered by an external source
 *
 *
 *          This code can be modified (pins mostly) to work with ATTiny devices 
 *          but without the Debugger class (ATTiny doesn't have a UART). Future 
 *          examples will demonstrate.
 *
 *Source:	https://github.com/gadgetstogrow/TaskScheduler
 *
 *
 *Tutorial:	https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
 *	
 *Pinout Arduino Uno:
 *			PIN		  PURPOSE									TASK TYPE
 *			--------------------------------------------------------------------
 *			A0:			Photocell Light Sensor					-Timed Task
 *			Pin 3:		Fader LED (PWM)							-Timed Task
 *			Pin 4:		Light Level Threshold Alarm				-Triggered Task
 *			Pin 5:		Light Level OK							-Triggered Task
 *			Pin 13:		Blink built-in LED						-Timed Task
 *
 *			USB Port:	TX/RX Debugger data to Serial Monitor	-Task 	
 **********************************************************************************/

// ***
// *** Used with Atmel Studio 
// *** (comment out if using Arduino IDE)
// ***
//#include <Arduino.h>

// ***
// *** Include Task Scheduler library 
// *** (should be located in same folder as this .ino file)
// ***
#include "Task.h"
#include "TaskScheduler.h"

// ***
// *** Pinout for Arduino Uno
// ***
#define PHOTOCELL_PIN			A0	
#define LED_BLINKER				13
#define LED_FADER				3
#define LED_LIGHTLEVEL_ALARM	4
#define LED_LIGHTLEVEL_OK		5

// ***
// *** Timed Task intervals 
// ***
#define RATE_PHOTOCELL_READING	3000	//Read Photocell	- Timed Task
#define RATE_BLINKER_BLINK		500		//Blink LED_BLINKER	- Timed Task
#define RATE_FADER_FADE			50		//Fade LED_FADER	- Timed Task

// ***
// *** Utility defines
// ***
#define INCREMENT_FADER_STEP			5	//Value used to increment/decrement LED_FADER
#define LIGHT_LEVEL_LOWER_THRESHOLD		300	//Value used to determine if LightLevelAlarm is triggered


/*****************************************************************************************
*	Class:		Debugger
*	Task Type:	Task (always runs)
*	Purpose:	This expands on Alan Burlison's original example code which demonstrates
*				a task that reads from the serial port and echoes to the Serial Monitor.
*				I've expanded it so that other classes use a pointer to the debugger object
*				to output simple debug messages while this example executes.
*				Classes that use the debugger object are passed a reference to &debugger
*				in their respective constructors.
*
*				For example: Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger);
*
*				To output debug information use: ptrDebugger->debugWrite("debug info");
*
*	Notes:		Yeah, I lazily used the String() function in this demonstration. Suedfbvbvfbfvvvvvvvb  me.						
******************************************************************************************/

// ***
// *** Define the Debugger Class as type Task
// ***
class Debugger : public Task
{
public:
	Debugger();
	void debugWrite(String debugMsg);	//Used for simple debugging of other tasks
	virtual void run(uint32_t now);		//Override the run() method
	virtual bool canRun(uint32_t now);	//Override the canRun() method
};

// ***
// *** Debugger Constructor
// ***
Debugger::Debugger()
	: Task()
	{
		Serial.begin(57600);
	}

// ***
// *** Debugger::canRun() <--checked by TaskScheduler
// ***
bool Debugger::canRun(uint32_t now)
{
	return Serial.available() > 0;
}

// ***
// *** Debugger::run() <--executed by TaskScheduler as a result of canRun() returning true.
// ***
void Debugger::run(uint32_t now)
{
	uint16_t byteCount = 0;
	
	Serial.println("-----------------");
	Serial.println("Input Received...");
	Serial.println("-----------------");
	while (Serial.available() > 0) {
		int byte = Serial.read();
		Serial.print("'") ;
		Serial.print(char(byte));
		Serial.print("' = ");
		Serial.print(byte, DEC);
		Serial.println(" ");
		if (byte == '\r') {
			Serial.print('\n', DEC);
		}
		
		byteCount++;
	}
	
	Serial.println("-----------------");
	Serial.print("Bytes Received: "); Serial.println(String(byteCount));
	Serial.println("-----------------");
	
}

// ***
// *** Debugger::debugWrite() <--provides basic debug info from other tasks
// ***
void Debugger::debugWrite(String debugMsg)
{
	Serial.println(debugMsg);
}


/*********************************************************************************
*	Class:		Blinker
*	Task Type:	TimedTask (runs when a specific time interval is reached)
*	Purpose:	Blinks the on-board Arduino LED (LED_BLINKER: Pin 13) at a specific rate
*				(RATE_BLINKER_BLINK) and utilizes the pointer ptrDebugger 
*				to print simple debug messages to the Serial Monitor 
*				
*				(e.g. "BLINKER: ON" and "BLINKER_OFF")
*				
*********************************************************************************/

// ***
// *** Define the Blinker Class as type TimedTask
// ***
class Blinker : public TimedTask
{
public:
	// Create a new blinker for the specified pin and rate.
	Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger);
	virtual void run(uint32_t now);
private:
	uint8_t pin;				// LED pin.
	uint32_t rate;				// Blink rate.
	bool on;					// Current state of the LED.
	Debugger *ptrDebugger;		// Pointer to debugger
};

// ***
// *** Blinker Constructor
// ***
Blinker::Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger)
	: TimedTask(millis(), RATE_BLINKER_BLINK),
	pin(_pin),
	rate(_rate),
	on(false),
	ptrDebugger(_ptrDebugger)
	{
		pinMode(pin, OUTPUT);     // Set pin for output.
	}

// ***
// *** Blinker::run() <--executed by TaskScheduler as a result of canRun() returning true.
// ***
void Blinker::run(uint32_t now)
{
	// If the LED is on, turn it off and remember the state.
	if (on) {
		digitalWrite(pin, LOW);
		on = false;
		ptrDebugger->debugWrite("BLINKER: OFF");
		// If the LED is off, turn it on and remember the state.
	} else {
		digitalWrite(pin, HIGH);
		on = true;
		//Send output to Serial Monitor via debugger
		ptrDebugger->debugWrite("BLINKER: ON");
	}
	// Run again in the specified number of milliseconds.
	setIntervalTime(rate);
}

/*****************************************************************************************
*	Class:		Fader
*	Task Type:	TimedTask (runs when a specific time interval is reached)
*	Purpose:	Fades the LED attached to (LED_FADER: Pin 3 ~PWM pin) at a specific rate
*				(RATE_FADER_FADE) whilst using the value INCREMENT_FADER_STEP to increase or decrease
*				the brightness of the LED with analogWrite(). It utilizes the pointer ptrDebugger
*				to print simple debug messages to the Serial Monitor when upper and lower ranges
*				are reached.
*
*				(e.g. "FADER BRIGHTNESS: 0" and "FADER BRIGHTNESS: 255")
*
******************************************************************************************/

// ***
// *** Define the Fader Class as type TimedTask
// ***
class Fader : public TimedTask
{
public:
	Fader(uint8_t _pin, uint8_t _increment, uint32_t _rate, Debugger *_ptrDebugger);
	virtual void run(uint32_t now);
private:
	uint8_t pin;
	uint8_t increment;
	uint32_t rate;
	uint8_t brightness;
	Debugger *ptrDebugger;
};

// ***
// *** Fader Constructor
// ***
Fader::Fader(uint8_t _pin, uint8_t _increment, uint32_t _rate, Debugger *_ptrDebugger)
	: TimedTask(millis(), RATE_FADER_FADE),
	pin(_pin),
	increment(_increment),
	rate(_rate),
	brightness(0),
	ptrDebugger(_ptrDebugger)
	{
		pinMode(pin, OUTPUT);     // Set pin for output.
	}

// ***
// *** Fader::run() <--executed by TaskScheduler as a result of canRun() returning true.
// ***
void Fader::run(uint32_t now)
{
	// set the brightness of pin 9:
	analogWrite(pin, brightness);

	// change the brightness for next time through the loop:
	brightness = brightness + increment;

	// reverse the direction of the fading at the ends of the fade:
	if (brightness <= 0 || brightness >= 255) {
		increment = -increment;
	}
	
	// ***
	// *** Is the brightness 0 or 255?
	// *** Write to the Debugger task
	// ***
	if(brightness == 0)
	{
		ptrDebugger->debugWrite("FADER LEVEL: 0");
	}
	else if(brightness == 255)
	{
		ptrDebugger->debugWrite("FADER LEVEL: 255");
	}
	
	// Run again in the specified number of milliseconds.
	setIntervalTime(rate);
}

/****************************************************************************************
*	Class:		LightLevelAlarm
*	Task Type:	TriggeredTask (normally dormant, triggered by PhotocellSensor via ptrAlarm->setRunnable())
*	Purpose:	This expands on Alan Burlison's original example code which didn't include
*				an example of TriggeredTask. I've expanded this code to allow the PhotocellSensor class 
*				to use the pointer to &ptrAlarm to access this class's setAlarm(), clearAlarm(), and
*				setRunnable() methods (similar to the Debugging pointer method).
*
*	Notes:		Similar to the Debugger class, this class needs to be defined/instantiated before the 
*				PhotocellSensor class in order to pass a pointer (*ptrAlarm) to the PhotocellSensor constructor.
*				I think a TimedTask would be more appropriate in a normal application since it only gets triggered
*				during a timed photocell reading, which means there could be a (in this case, 3 second) latency 
*				before updating the OK and ALARM LEDs. But c'est la vie, this should give you a good grasp on how 
*				to use a TriggeredTask in your own applications.
*****************************************************************************************/

// ***
// *** Define the LightLevelAlarm Class as type TriggeredTask
// ***
class LightLevelAlarm : public TriggeredTask
{
public:
	LightLevelAlarm(uint8_t _ok_pin, uint8_t _alarm_pin);
	virtual void run(uint32_t now);
	void setAlarmCondition(bool condition);		// alarmCondition = true/false via ptrAlarm (see PhotocellSensor constructor)
	
private:
	uint8_t ok_pin;								// Light level within threshold LED pin.
	uint8_t alarm_pin;							// Light level below threshold LED pin.
	bool alarmCondition;						// Accessed by setAlarmCondition() via ptrAlarm (see PhotocellSensor constructor)
};

// ***
// *** LightLevelAlarm Constructor
// ***
LightLevelAlarm::LightLevelAlarm(uint8_t _ok_pin, uint8_t _alarm_pin)
	:TriggeredTask(),
	ok_pin(_ok_pin),
	alarmCondition(false),
	alarm_pin(_alarm_pin)
	{
		pinMode(ok_pin, OUTPUT);    // Set ok pin for output.
		pinMode(alarm_pin, OUTPUT);	// Set alarm pin for output.
	}

// ***
// *** LightLevelAlarm::setAlarmCondition() <--updates alarmCondition=true via ptrAlarm->setAlarm()
// ***
void LightLevelAlarm::setAlarmCondition(bool _condition)
{
	alarmCondition = _condition;
}

// ***
// *** LightLevelAlarm::run() <--executed by TaskScheduler as a result of canRun() returning true,
// ***							 in this case, PhotcellSensor utilizing ptrAlarm->setRunnable()
// ***
void LightLevelAlarm::run(uint32_t now)
{
	// Set appropriate LED for alarm condition
	if(alarmCondition == false)
	{
		digitalWrite(ok_pin, HIGH);
		digitalWrite(alarm_pin, LOW);
	}
	else
	{
		digitalWrite(ok_pin, LOW);
		digitalWrite(alarm_pin, HIGH);
	}
	
	// ***
	// *** resetRunnable() IMPORTANT! IMPORTANT!
	// *** It's important to resetRunnable() after executing a TriggeredTask.
	// *** If bool runFlag in a TriggeredTask is not reset, the TriggeredTask will
	// *** continue to run indefinitely which defeats its purpose. It will stay dormant
	// *** and be ignored by the TaskScheduler until triggered again.
	// ***
	resetRunnable();
}

/***************************************************************************************
*	Class:		PhotocellSenor
*	Task Type:	TimedTask  (runs when a specific time interval is reached)
*	Purpose:	Measures the photocell attached to (PHOTOCELL_PIN: Pin A0) at a specific rate
*				(RATE_PHOTOCELL_READING) with analogRead(). It utilizes the pointer ptrDebugger
*				to print simple debug messages to the Serial Monitor. 
*
*				(e.g. "LIGHT LEVEL ALARM!" and/or "Light Level: 0-1023")		
*
*
****************************************************************************************/

// ***
// *** Define the PhotocellSensor Class as type TimedTask
// ***
class PhotocellSensor : public TimedTask
{
public:
	PhotocellSensor(uint8_t _pin, uint32_t _rate, LightLevelAlarm *_ptrAlarm, Debugger *_ptrDebugger);
	virtual void run(uint32_t now);
private:
	uint8_t pin;
	uint32_t rate;
	uint16_t lightLevel;
	LightLevelAlarm *ptrAlarm;
	Debugger *ptrDebugger;
};

// ***
// *** PhotocellSensor Constructor
// *** Note the passing of the _ptrAlarm to allow access to setAlarmCondition() and setRunnable()
// ***
PhotocellSensor::PhotocellSensor(uint8_t _pin, uint32_t _rate, LightLevelAlarm *_ptrAlarm, Debugger *_ptrDebugger)
: TimedTask(millis(), RATE_PHOTOCELL_READING),
pin(_pin),
rate(_rate),
lightLevel(0),
ptrAlarm(_ptrAlarm),
ptrDebugger(_ptrDebugger)

{
	pinMode(pin, INPUT);     // Set pin for output.
}

// ***
// *** PhotocellSensor::run() <--executed by TaskScheduler as a result of canRun() returning true,
// ***
void PhotocellSensor::run(uint32_t now)
{
	// read the light level of PHOTOCELL_PIN A0
	lightLevel = analogRead(pin);
	
	//Ready Debugger for output
	ptrDebugger->debugWrite("-----------------");
	
	//// ***
	//// *** Is the lightLevel below the lower threshold?
	//// *** Write to the Debugger task
	//// *** Trigger LightLevelAlarm task
	//// ***
	if(lightLevel < LIGHT_LEVEL_LOWER_THRESHOLD)
	{
		//Set LightLevelAlarm::alarmCondition = true
		ptrAlarm->setAlarmCondition(true);
		ptrDebugger->debugWrite("LIGHT LEVEL ALARM!");
	}
	else
	{
		//Set LightLevelAlarm::alarmCondition = false
		ptrAlarm->setAlarmCondition(false);
	}
	
	//Finish Debugger update
	ptrDebugger->debugWrite("Light Level: " + String(lightLevel));
	ptrDebugger->debugWrite("-----------------");
	
	// ***
	// ***IMPORTANT! Enable the runFlag for the LightLevelAlarm TriggeredTask
	// ***
	ptrAlarm->setRunnable();
	
	// Run again in the specified number of milliseconds.
	setIntervalTime(rate);
}

/**************************************************************************************
*	Description:	void setup()
*	Purpose:		N/A
*
*	Notes: All processing/setup is handled by the classes above.  
*
**************************************************************************************/
void setup()
{
	//Nothing to do here
}

// Main Loop
void loop()
{
/***************************************************************************************
*	Description:	Instantiate the tasks and schedule task priority.
*	Purpose:		This is the heart of the program.  All of the needed code has been 
					encapsulated in the above Class definitions. The code below will
					create the task objects based on these classes and fill a task
					array for the TaskScheduler to manage.
*
*
*	Note:			Although this is located in the loop() routine, this will only
*					run once. TaskScheduler::run() never returns control to loop().
***************************************************************************************/

// ***
// *** Instantiate the task objects for use by the TaskScheduler
// ***
	Debugger			debugger;
	
	Blinker				blinker(LED_BLINKER, 
								RATE_BLINKER_BLINK, 
								&debugger);
	
	Fader				fader(LED_FADER, 
							  INCREMENT_FADER_STEP, 
							  RATE_FADER_FADE, 
							  &debugger);
	
	LightLevelAlarm		lightLevelAlarm(LED_LIGHTLEVEL_OK, 
										LED_LIGHTLEVEL_ALARM);
	
	PhotocellSensor		photocellSensor(PHOTOCELL_PIN, 
										RATE_PHOTOCELL_READING, 
										&lightLevelAlarm, 
										&debugger);
			
// ***
// *** Create an array of pointers (eek!) to the task objects we just instantiated.
// ***
// *** The order matters here.  When the TaskScheduler is running it finds the first task it can
// *** run--canRun(), runs the task--run(), then returns to the top of the list and starts the 
// *** process again. I've experimented with different orders, but couldn't find any astonishing
// *** differences; and considering this isn't a "real" application, this'll do.  One other note:
// *** of all the tasks, communicating with the Serial Monitor via the Debugger object is the by 
// *** far the biggest tax on the MCU, but can be removed fairly from your final release. Have fun.
// ***
	Task *tasks[] = {
		
		&debugger,		
		&blinker,
		&fader,
		&photocellSensor,
		&lightLevelAlarm
				
	};
	
	// ***
	// *** Instantiate the TaskScheduler and fill it with tasks.			
	// ***
	TaskScheduler scheduler(tasks, NUM_TASKS(tasks));
	
	// GO! Run the scheduler - it never returns.
	scheduler.runTasks();
}
