
/**********************************************************************************
 *  Project:  Arduino_Task_Scheduler_with_Sleep.ino
 *  Contact:  Kevin Gagnon @GadgetsToGrow
 *  URL:	  https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
 *  Date:     3-31-2019
 *  
 *  Purpose:	Simple example to demonstrate how a TaskScheduler running on Arduino Uno can be put to 
 *				sleep and awoken via an interrupt. 
 *			
 *  Source:		Download latest code from:
 *				URL: https://github.com/gadgetstogrow/Arduino-Task-Scheduler/
 *  
 *  Pinout (Arduino Uno):
 *
 *	PIN		PURPOSE											                  TASK TYPE
 *	--------------------------------------------------------------------
 *	Pin 3:	External Interrupt Pin (PCINT2_vect)    
 *	Pin 5:	Fade LED										                -Timed Task
 *	Pin 6:	Blink LED										                -Timed Task
 *	Pins 0/1:	(USB) Debugger - Serial Monitor Output		- Task    
 *
 *	NOTE:
 *	The Debugger is enabled by default.
 *	Disable DEBUGGING: Open the Debugger.h file and comment out the first line:
 *	"#define DEBUG 1". Nothing else needs to be done. The debugger task will not be
 *  instantiated and all calls to ptrDebugger->debugWrite will be ignored. Make
 *	sure you comment this line out before final your release.       
 *    
 **********************************************************************************/

//#include <Arduino.h>
#include <avr/sleep.h>

// ***
// *** Task Scheduler library
// ***
#include "Task.h"
#include "TaskScheduler.h"

// ***
// *** Common Tasks
// ***
// ***
#include "Debugger.h"
#include "Blinker.h"
#include "Config.h"

// ***
// *** Application specific tasks
// ***
#include "ArcReactor.h" //Don't laugh! Pulsing LED to be used as a "Iron Man" style pendant 
						            //for an ATTiny85 based "pet necklace"
						            // (coming soon as a Hackster.io project near you)

// -------------------------------------------------------------------------------------
// Define the AppManager Class as type: TimedTask
//
// Reads the ledCycleCount variable (incremented by ISR(PCINT2_vect) every 100ms (1/10 sec)
// based on the switch attached to Pin 3. If the value is greater than zero
// (activity sensed) the AppManager Task performs its tasks normally. If ledCycleCount
// is zero (no activity), it puts the Arduino to sleep and waits to be interrupted again. 
//
// -------------------------------------------------------------------------------------
class AppManager : public TimedTask
{
	public:
		AppManager( uint16_t _readRate, 
					ArcReactor *_ptrArcReactor,
					Blinker *_ptrBliner, 
					Debugger *_ptrDebugger = NULL);
					
		virtual void run(uint32_t now);
	
	private:
		uint8_t readRate;								// Rate to read the Tilt Sensor
		void initPinChangeInterrupt();					// Enables the Pin Change Interrupt
		void enableGlobalInterrupts(bool _enabled);		// Enables/Disables Global Interrupts sei/cli
		void enableADC(bool _enabled);	
		uint16_t getCycleRate(uint8_t _ledCycleCount);	// Enables/Disables the Analog/Digital Converter
		ArcReactor *ptrArcReactor;
		Blinker *ptrBlinker;
		Debugger *ptrDebugger;							
};

AppManager::AppManager( uint16_t _readRate, 
						ArcReactor *_ptrArcReactor,
						Blinker *_ptrBlinker,
						Debugger *_ptrDebugger)
	
	: TimedTask(millis()),
	readRate(_readRate),
	ptrArcReactor(_ptrArcReactor),
	ptrBlinker(_ptrBlinker),
	ptrDebugger(_ptrDebugger)

{
	// ***
	// *** Enable Tilt Sensor Pin Change Interrupt on D3
	// *** Pin Change Interrupt Request 2 (pins D0 to D7)  (PCINT2_vect)
	// ***
	initPinChangeInterrupt();

	// ***
	// *** Enable Global Interrupts - sei();
	// ***
	enableGlobalInterrupts(true);
	
}

//-------------------------------------------------------------------
// Main Routine
//-------------------------------------------------------------------
void AppManager::run(uint32_t now)
{
	if(ledCycleCount > 1) //Movement/Awake - performing application functions
	{
		ledCycleCount--;
		
		if(ledCycleCount > 0)
		{
			// ***
			// *** Get the Cycle Rate
			// ***
			uint16_t _newLEDCycleRate = getCycleRate(ledCycleCount);
			// ***
			// *** Update the LED cycle rates
			// ***
			ptrBlinker->setCycleRate(_newLEDCycleRate);
			
			//Debugger
			#ifdef DEBUG
			ptrDebugger->debugWrite("------------------");
			ptrDebugger->debugWrite("CycleCount: " + String(ledCycleCount));
			ptrDebugger->debugWrite("CycleRate: " + String(_newLEDCycleRate));
			ptrDebugger->debugWrite("------------------");
			#endif			
		
		//Debugger
		#ifdef DEBUG
		} else {
			ptrDebugger->debugWrite("Yawn...I'm bored, goodnight...ZZZzzzz");
		#endif
		}

	}
	else //Go to Sleep
	{
		
		//Disable LED Outputs (Power Management)
		ptrArcReactor->enableLED(false);
		ptrBlinker->enableLED(false);
		
		// ***
		// *** Disable Global Interrupts
		// ***
		enableGlobalInterrupts(false);

		// ***
		// *** Turn off the Analog To Digital Converter (ADC) 
		// ***
		enableADC(false);

		// ***
		// *** Sleep Mode: Deep Sleep
		// ***
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		// ***
		// *** Enable Global Interrupts -to wake up from sleep PCINT2 on TILT_SENSOR_PIN/PD3/PCINT19
		// ***
		enableGlobalInterrupts(true);
		
		// ***
		// *** Go to sleep
		// ***
		
		//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
		sleep_mode();
		//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
		
		// ***
		// *** Wake Up
		// ***
		sleep_disable();
		
		// ***
		// *** Turn On the Analog To Digital Converter (ADC)
		// *** (not in use for this project, so leave it off)
		// *** 
		//enableADC(true);

		//Re-enable LEDs
		ptrArcReactor->enableLED(true);
		ptrBlinker->enableLED(true);
				
		#ifdef DEBUG
		ptrDebugger->debugWrite("--Arduino Awake--");
		#endif

	}
	
	incRunTime(readRate);
}

//-------------------------------------------------------------------
// Adjust cycle rate proportional to movement. More Activity, Faster Rate.
//-------------------------------------------------------------------
uint16_t AppManager::getCycleRate(uint8_t _ledCycleCount)
{

	uint16_t newCycleRate = CYCLE_RATE_SLOW_MS;
	
	if ((_ledCycleCount < CYCLE_COUNT_MAX) && (_ledCycleCount > CYCLE_RATE_THRESHOLD_FAST)) 
	
	{			newCycleRate = CYCLE_RATE_FAST_MS;
			
	} else if ((_ledCycleCount <= CYCLE_RATE_THRESHOLD_FAST) && (_ledCycleCount > CYCLE_RATE_THRESHOLD_MEDIUM))
	
	{			newCycleRate = CYCLE_RATE_MEDIUM_MS;
			
	} else if ((_ledCycleCount <= CYCLE_RATE_THRESHOLD_MEDIUM) && (_ledCycleCount > CYCLE_RATE_THRESHOLD_SLOW))
	
	{
				newCycleRate = CYCLE_RATE_SLOW_MS; 
	}

	return newCycleRate;
}

// ---------------------------------------------------------------------
// Enable PCINT19 on D3 for Arduino UNO.
//		-Internal Pull up
//		-Pin Change
//		-Fires ISR(PCINT2_vect)
// ---------------------------------------------------------------------
void AppManager::initPinChangeInterrupt()
{
	//Set Input pin D3 with Internal Pull-up Resistor
	pinMode(TILT_SENSOR_PIN, INPUT_PULLUP);
	
	//Enable Pin Change Interrupt PCINT2 on TILT_SENSOR_PIN/PD3/PCINT19
	PCICR |= (1 << PCIE2);
	PCIFR |= (1 << PCIF2);
	PCMSK2 |= (1 << PCINT19);	
}

void AppManager::enableADC(bool _enabled)
{
	if(_enabled)
	{

		ADCSRA |= (1 << ADEN);

	} else {
	
		ADCSRA &= ~(1 << ADEN);		
	
	}	
}

// ---------------------------------------------------------------------
// Enable Global Interrupts in SREG
// ---------------------------------------------------------------------
void AppManager::enableGlobalInterrupts(bool _enabled)
{
	if(_enabled)
	{
		sei();
	}
	else
	{
		cli();
	}
}


// ---------------------------------------------------------------------
// Interrupt Service Routine [PCINT2] for switch connected to Pin 3
// Increments the ledCycleCount to keep the led(s) enabled. When the 
// ledCycleCount reaches zero (0), the AppManager will put the 
// MCU into sleep mode to conserve power. This same interrupt will awaken
// the MCU.
//
// Note: There is no debounce logic for the application, but we restrain the
// ledCycleCount from exceeding the uint8_t range by applying a check against
// MAX_LED_CYCLE_COUNT. The AppManager will decrement this number
// on each of its Timed Tasks determined by APP_MANAGER_RATE_MS.
// 
// ---------------------------------------------------------------------
ISR(PCINT2_vect)
{
	//Don't exceed ~180 in ledCycleCount (no need to debounce)
	if(ledCycleCount < (CYCLE_COUNT_MAX - CYCLE_INCREMENT_BY_X))			
		ledCycleCount += CYCLE_INCREMENT_BY_X;
}


//SETUP
void setup() {}

//LOOP
void loop() 
{

/***************************************************************************************
* Description:  Instantiate the tasks and schedule task priority.
* Purpose:		This is the heart of the program.  All of the needed code has been 
*				encapsulated in the above Class definitions. The code below will
*				create the task objects based on these classes and fill a task
*				array for the TaskScheduler to manage.
*
*
* Note:     Although this is located in the loop() routine, this will only
*			run once. TaskScheduler::run() never returns control to loop().
*
***************************************************************************************/

// ***
// *** Instantiate the task objects for use by the TaskScheduler
// ***
// *** New: If #define DEBUG 1 is commented in Debugger.h, don't instantiate the debugger object. 
// ***
#ifdef DEBUG

	Debugger debugger;
	
	ArcReactor arcReactor(ARC_REACTOR_PIN, 15, &debugger);

	Blinker blinker(LED_BLINKER_PIN, BLINKER_RATE_MS, &debugger);

	AppManager appManager(APP_MANAGER_RATE_MS, &arcReactor, &blinker, &debugger);

#else

	ArcReactor arcReactor(ARC_REACTOR_PIN, 15);

	Blinker blinker(LED_BLINKER_PIN, BLINKER_RATE_MS);

	AppManager appManager(APP_MANAGER_RATE_MS, &arcReactor, &blinker);

#endif

 // ***
 // *** Create an array of reference addresses to the task objects we just instantiated.
 // ***
 // *** The order matters here.  When the TaskScheduler is running it finds the first task it can
 // *** run--canRun(), runs the task--run(), then returns to the top of the list and starts the
 // *** process again.
 // ***
 // *** New: If #define DEBUG 1 is commented in Debugger.h, the debugger object will not be added. 
 #ifdef DEBUG
 
 Task *tasks[] = {
	&arcReactor,
 	&debugger,  
	&blinker,
	&appManager

 };
 
#else

Task *tasks[] = {
	&arcReactor,
	&blinker,
	&appManager
  
};

#endif

 //***
 //*** Instantiate the TaskScheduler and fill it with task references.
 //***
 TaskScheduler scheduler(tasks, NUM_TASKS(tasks));
 
 //GO! Run the scheduler - it never returns.
 scheduler.runTasks();
  
}
