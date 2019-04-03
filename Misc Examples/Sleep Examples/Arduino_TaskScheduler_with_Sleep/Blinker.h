/*************************************************************************************************
 * Blinker.h
 * 
 * Inherits from the Abstract TimedTask Class
 *
 * Purpose: Export functionality to separate .h files from main code
 *
 * Copyright © 2016 Kevin Gagnon. All Rights Reserved.
 *
 * This file is part of the Task Scheduler Demo:
 * https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Task Scheduler example code. If not,
 * see http://www.gnu.org/licenses/.
 ************************************************************************************************/ 

#ifndef BLINKER_H_
#define BLINKER_H_

/*********************************************************************************
* Class:		Blinker : Inherits from Abstract TimedTask
* Task Type:	TimedTask (runs when a specific time interval is reached)
*
* Purpose:		Designed mainly for Sanity Check/Demonstration purposes.
*				Blinks the on-board Arduino LED (LED_BLINKER: Pin 13) at a specific rate
*				(RATE_BLINKER_BLINK) and (optionally) utilizes the pointer ptrDebugger
*				to print simple debug messages to the Serial Monitor:
*
*				(e.g. "BLINKER: ON" and "BLINKER_OFF")
*
*				Note: To disable the Debugger class, comment out the ' #define DEBUG 1 ' line in
*				the main .ino code.
*
*
*********************************************************************************/
// ***
// *** Define the Blinker Class as type TimedTask
// ***
class Blinker : public TimedTask
{
	public:
		Blinker(uint8_t _outputPin, uint32_t _cycleRate, Debugger *_ptrDebugger = NULL);
		virtual void run(uint32_t now);
		void enableLED(bool _enabled);
		void setCycleRate(uint16_t _newRate);
	private:
		uint8_t outputPin;      // LED pin.
		uint32_t cycleRate;     // Blink rate.
		bool on;				// Current state of the LED.
		Debugger *ptrDebugger;  // Pointer to debugger

};

// ***
// *** Blinker Constructor(s)
// ***
Blinker::Blinker(uint8_t _outputPin, uint32_t _cycleRate, Debugger *_ptrDebugger)
	: TimedTask(millis()),
	outputPin(_outputPin),
	cycleRate(_cycleRate),
	on(false),
	ptrDebugger(_ptrDebugger)

	{
		pinMode(outputPin, OUTPUT);
	}

// ***
// *** Blinker::run()
// ***
void Blinker::run(uint32_t now)
{
	
	// If the LED is on, turn it off and remember the state.
	if (on) {
		digitalWrite(outputPin, LOW);
		on = false;
		//Set the output message of the debugger
		#ifdef DEBUG
		ptrDebugger->debugWrite("Blinker LED: OFF");
		#endif
		
	} else {
		
		digitalWrite(outputPin, HIGH);
		on = true;
		//Set output message of the Debugger
		#ifdef DEBUG
		ptrDebugger->debugWrite("Blinker LED: ON");
		#endif
	}
	
	// Run again in the specified number of milliseconds.
	incRunTime(cycleRate);
	
}

// ***
// *** Blinker::setCycleRate - Allows AppManager access to change the blink rate.
// ***
void Blinker::setCycleRate(uint16_t _newRate)
{
	cycleRate = _newRate;
}

// ***
// *** ArcReactor::setCycleRate - Allows AppManager access to change the blink rate.
// ***
void Blinker::enableLED(bool _enable)
{
	if(_enable)
	{
		pinMode(outputPin, OUTPUT);
		
		} else {
		
		digitalWrite(outputPin, LOW);	//Write a zero
		pinMode(outputPin, INPUT);		//Disable Output
	}
}


#endif /* BLINKER_H_ */