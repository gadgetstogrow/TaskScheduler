
/**********************************************************************************
 *  Project:	Arduino_Task_Scheduler_Interrupt_Example.ino
 *  Contact:	Kevin Gagnon @GadgetsToGrow
 *  URL:		https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
 *  Date:		3-31-2019
 *  
 *	Purpose:	Simple example to demonstrate how external interrupts can be handled within 
 *				the Task Scheduler. Runs in cooperation with a (optional) debugger task and the
 *				blinker task.
 *  Source:		Download latest code from:
 *				URL: https://github.com/gadgetstogrow/Arduino-Task-Scheduler/
 *  
 *  Pinout Arduino Uno:
 *				PIN   PURPOSE								TASK TYPE
 *				--------------------------------------------------------------------
 *				Pin 2:  External Interrupt Pin (INT0_vect)		
 *				Pin 3:  Turn on LED on Interrupt			-Simple Task
 *				Pin 13: Blink built-in LED					-Timed Task
 *				*USB: Serial Monitor Output	(optional)		-Simple Task							
 *				Pin 11: Timer 2 OCRA output of 1ms pulse for interrupt triggering
 *		
 **********************************************************************************/

//#include <Arduino.h>
#include <avr/io.h>

// ***
// *** Include Task Scheduler library
// *** (should be located in same folder as this .ino file)
// ***
#include "Task.h"
#include "TaskScheduler.h"


// ***
// *** Conditional Debug Info to Serial Monitor
// *** by commenting this line, a debugger will not be instantiated,
// *** or added to the TaskScheduler's tasks list. All ptrDebugger->debugWrite() 
// *** calls will be ignored.
// ***
#define DEBUG 1



// ***
// *** Pin Definitions
// ***
#define LED_BLINKER						13
#define INTERRUPT_INPUT_PULLUP_PIN		2
#define LED_INTERRUPT_RECEIVED			3
#define TIMER2_OUTPUT_PIN				11

// ***
// *** Utility Definitions
// ***
#define RATE_BLINKER_BLINK				1000

// ***
// *** Volatile variable for Interrupt/Task cooperation
// ***
volatile bool interruptTriggered;

/*****************************************************************************************
*	Class:		Debugger
*	Task Type:	Task (Optional: Comment "#define DEBUG 1" at beginning of code to disable)
*	Purpose:	Other tasks use a pointer to the debugger object to output simple debug 
*				messages while this example executes.
*				Tasks that use the debugger object are passed a reference to &debugger
*				in their respective constructors.
*
*				For example: Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger);
*
*				To output debug information use: ptrDebugger->debugWrite("debug info");
*
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
* Class:		Blinker
* Task Type:	TimedTask (runs when a specific time interval is reached)
* Purpose:		Blinks the on-board Arduino LED (LED_BLINKER: Pin 13) at a specific rate
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
	// Note: The Debugger pointer is defaulted to null to handle:
	// debugging preprocessor directives (#if DEBUG) throughout code.
	Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger = NULL);
	virtual void run(uint32_t now);
	private:
	uint8_t pin;			// LED pin.
	uint32_t rate;			// Blink rate.
	bool on;				// Current state of the LED.
	Debugger *ptrDebugger;	// Pointer to debugger
};

// ***
// *** Blinker Constructor(s)
// ***
Blinker::Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger)
	: TimedTask(millis()),
	pin(_pin),
	rate(_rate),
	on(false),
	ptrDebugger(_ptrDebugger)
	{
		pinMode(pin, OUTPUT);     // Set pin for output.
	}

// ***
// *** Blinker::run()
// ***
void Blinker::run(uint32_t now)
{
	// If the LED is on, turn it off and remember the state.
	if (on) {
		digitalWrite(pin, LOW);
		on = false;
		//Set the output message of the debugger
		#ifdef DEBUG
		ptrDebugger->debugWrite("Blinker LED: OFF");
		#endif
	}
	else 
	{
		digitalWrite(pin, HIGH);
		on = true;
		//Set output message of the Debugger
		#ifdef DEBUG
		ptrDebugger->debugWrite("Blinker LED: ON");
		#endif
	}

	// Run again in the specified number of milliseconds.
	incRunTime(rate);
	
}

/*****************************************************************************************
*	Class:		Interrupt Handler
*	Task Type:	Task
*	Purpose:	When the Button is pressed on pin 2, this routine will run to handle the associated task.
*				The volatile variable isButtonPressed is returned during the canRun() check by the 
*				Task Scheduler. This will turn the LED attached to pin 3 On or Off.
*
******************************************************************************************/

// ***
// *** Define the Interrupt Handler Class as type Task
// ***
class InterruptHandler : public Task
{
public:
	// Note: The Debugger pointer is defaulted to null to handle:
	// debugging preprocessor directives (#if DEBUG) throughout code.
	InterruptHandler(uint8_t _pin, Debugger *_ptrDebugger = NULL);
	virtual void run(uint32_t now);		//Override the run() method
	virtual bool canRun(uint32_t now);	//Override the canRun() method
private:
	uint8_t pin;				// LED pin.
	bool on;					// Current state of the LED.
	Debugger *ptrDebugger;		// Pointer to debugger	
};

// ***
// *** InterruptHandler Constructor
// ***
InterruptHandler::InterruptHandler(uint8_t _pin, Debugger *_ptrDebugger)
	: Task(),
	pin(_pin),
	ptrDebugger(_ptrDebugger)
	{
		pinMode(pin, OUTPUT);
	}

// ***
// *** InterruptHandler::canRun() Returns true/false state of the ISR
// ***
bool InterruptHandler::canRun(uint32_t now)
{
		return interruptTriggered;
}

// ***
// *** InterruptHandler::run()
// ***
void InterruptHandler::run(uint32_t now)
{
	// ***
	// *** IMPORTANT!!!
	// *** Reset the ISR Flag
	// *** 
	interruptTriggered = false;
	
	// *** Whoa...Slow things down
	// *** Blinks LED attached to pin 3 ~1 second intervals
	static uint32_t triggerCount = 0;
	triggerCount++;
	
	if (triggerCount % 1000 == 0)  //Every 1000 interrupt triggers
	{
		#ifdef DEBUG
		ptrDebugger->debugWrite("-------Trigger Count: " + String(triggerCount));
		#endif
		
		if(on)
		{
			digitalWrite(LED_INTERRUPT_RECEIVED, LOW);
			on = false;
			//Set output message of the Debugger
			#ifdef DEBUG
			ptrDebugger->debugWrite("-------Interrupt LED = OFF");
			#endif
		}
		else
		{
			digitalWrite(LED_INTERRUPT_RECEIVED, HIGH);
			on = true;
			//Set output message of the Debugger
			#ifdef DEBUG
			ptrDebugger->debugWrite("-------Interrupt LED = ON");
			#endif
		}
	}
}

// ***
// *** Interrupt Service Routine (ISR)
// ***
ISR (INT0_vect)
{
	interruptTriggered = true;
}


// ***
// *** IMPORTANT!!! - Connect a jumper wire between pin 11 (output of Timer 2 OCRA) to pin 2 Interrupt Pin:
// ***
// *** [Timer 2 signal falls on pin 11] --> 
// *** [Interrupt executes ISR] --> [interruptTriggered Flag = true] -->
// *** [InterruptHandler Task::canRun(interruptTriggered = true?)] --> 
// *** [InterruptHandler::run()] --> [LED on pin 3 blinks every 1000 interrupts (~1 second)]
// ***
void activateInterrupt0()
{
	// ***
	// *** Set Interrupt pin to pull-up, a (falling) level will trigger ISR (INT0_vect) above.
	// ***
	pinMode(INTERRUPT_INPUT_PULLUP_PIN, INPUT_PULLUP);
	
	// ***
	// *** Enable falling 
	// ***
	EICRA &= ~(bit(ISC00) | bit (ISC01));	// clear existing flags
	EICRA |=  bit (ISC01);					// set wanted flags (falling level interrupt)
	EIFR   =  bit (INTF0);					// clear flag for interrupt 0
	EIMSK |=  bit (INT0);					// Enable Interrupt 0	
}


// ***
// *** Disable Interrupt (not used in this example, here for reference)
// ***

/*
void deactivateInterrupt0()
{
	EIMSK &= ~bit (INT0);
}
*/

// ***
// *** Setup Timer2 to output 1ms pulse on Pin 11
// *** Good Info on Timers here:
// *** https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
// *** And Here:
// *** https://www.gammon.com.au/timers
// ***
void activateTimer2() {
	
	// ***
	// *** Set Timer 2 Output Compare A pin to Output (pin 11)
	// ***
	pinMode(TIMER2_OUTPUT_PIN, OUTPUT);

	// ***
	// *** WAVEFORM GENERATION MODE = Fast PWM
	// *** Reference: ATMega328P Datasheet Table 18-8
	// ***
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	// ***
	// *** INVERT PWM PULSE
	// *** (Normally HIGH, will spike LOW to give us a good interrupt signal)
	// *** Reference: ATMega328P Datasheet Table 18-3
	// ***
	TCCR2A |= (1 << COM2A1) | (1 << COM2A0);
	// ***
	// *** FREQUENCY = MCU Clock/Prescaler(64)/ Max OCR2A Count(256)
	// *** Prescaler 64 = 976.5625KHz
	// *** Reference: ATMega328P Datasheet Table 18-9
	// ***
	TCCR2B |= (1 << CS22) & ~(1 << CS21) & ~(1 << CS20);
	// ***
	// *** DUTY CYCLE (Short Inverted Pulse every ~1ms)
	// *** (Now would be a good time to get an oscilloscope :)
	// ***
	OCR2A = 1;
	
}



void setup() {
	// ***
	// *** Enable Global Interrupts (probably not needed)
	// ***
	sei();
	
	// ***
	// *** Attach External Interrupt to Pin 2 (INT0_vect)
	// ***
	activateInterrupt0();
	// ***
	// *** Alternate Method to Enable External Interrupt (INT_0_vect) in Arduino IDE:
	// *** I chose to use the direct register control illustrated in activateInterrupt0() method 
	// ***
	//attachInterrupt(digitalPinToInterrupt(BUTTON_INTERRUPT_PIN), ISR_ButtonPressed, FALLING);
	

	
	// ***
	// *** Configure/Enable Timer 2 for Interrupt signals to be used on pin 2
	// ***
	activateTimer2();
}



void loop() {

/***************************************************************************************
* Description:  Instantiate the tasks and schedule task priority.
* Purpose:		This is the heart of the program.  All of the needed code has been 
				encapsulated in the above Class definitions. The code below will
				create the task objects based on these classes and fill a task
				array for the TaskScheduler to manage.
*
*
* Note:			Although this is located in the loop() routine, this will only
*				run once. TaskScheduler::run() never returns control to loop().
***************************************************************************************/

// ***
// *** Instantiate the task objects for use by the TaskScheduler
// ***
// *** New: If DEBUG is commented, don't instantiate the debugger object. 
// ***
#ifdef DEBUG
Debugger debugger;
  
Blinker	blinker(LED_BLINKER, RATE_BLINKER_BLINK, &debugger);
                
InterruptHandler interruptHandler(LED_INTERRUPT_RECEIVED, &debugger);

#else

InterruptHandler interruptHandler(LED_INTERRUPT_RECEIVED);

Blinker	blinker(LED_BLINKER, RATE_BLINKER_BLINK);

#endif

 // ***
 // *** Create an array of reference addresses to the task objects we just instantiated.
 // ***
 // *** The order matters here.  When the TaskScheduler is running it finds the first task it can
 // *** run--canRun(), runs the task--run(), then returns to the top of the list and starts the
 // *** process again.
 // ***
 // *** New: If DEBUG is commented, don't instantiate the debugger object. 
 #ifdef DEBUG
 Task *tasks[] = {
	 
	 &debugger,
	 &interruptHandler,
	 &blinker
 };
#else
Task *tasks[] = {
	&interruptHandler,
	&blinker
};
#endif

 //***
 //*** Instantiate the TaskScheduler and fill it with task references.
 //***
 TaskScheduler scheduler(tasks, NUM_TASKS(tasks));
 
 //GO! Run the scheduler - it never returns.
 scheduler.runTasks();
  
}
