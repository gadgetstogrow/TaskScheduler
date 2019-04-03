/*************************************************************************************************
 * Debugger.h
 * 
 * Inherits from the Abstract Task Class
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


#ifndef DEBUGGER_H_
#define DEBUGGER_H_

// ***
// *** Conditional Debug Info to Serial Monitor. Enabled by Default.
// *** By commenting this line, the debugger task will not be instantiated
// *** or added to the TaskScheduler's tasks list. All ptrDebugger->debugWrite()
// *** calls will be ignored. Make sure you comment this line before final release.
// ***

#define DEBUG 1

/*****************************************************************************************
* Class:    Debugger
* Task Type:  Task 
* Purpose:  Other tasks use a pointer to the debugger object to output simple debug
*       messages while this example executes.
*       Tasks that use the debugger object are passed a reference to &debugger
*       in their respective constructors.
*
*       For example: Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger);
*
*       To output debug information use: ptrDebugger->debugWrite("debug info");
*
******************************************************************************************/

// ***
// *** Define the Debugger Class as type Task
// ***
class Debugger : public Task
{
	public:
	Debugger();
	void debugWrite(String debugMsg); //Used for simple debugging of other tasks
	virtual void run(uint32_t now);   //Override the run() method
	virtual bool canRun(uint32_t now);  //Override the canRun() method
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
// *** Debugger::canRun()
// ***
bool Debugger::canRun(uint32_t now)
{
	return Serial.available() > 0;
}

// ***
// *** Debugger::run()
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

#endif /* DEBUGGER_H_ */