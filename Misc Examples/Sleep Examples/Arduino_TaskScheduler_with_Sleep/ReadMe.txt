/*************************************************************************************************
 * Arduino_TaskScheduler_with_Sleep.ino
 * 
 * Author: Kevin Gagnon
 * Date: April 3, 2019 (I joined the US Navy this day in 1986!)
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
 
 Purpose and History: 
 
 I'm re-working on an ATTiny85 "Pet Necklace" project I did last year to incorporate the 
 TaskScheduler. It uses an ATTiny85 powered with a CR2032 coin cell, so power management is a paramount concern. Before I 
 started going through my old code I wanted to make sure I could enable sleep() without screwing 
 up the TaskScheduler (specifically TimedTasks).  Good news: Because I use the SLEEP_MODE_PWR_DOWN
 all clocks are disabled, which in-turn, disables the millis() count that the TimedTask relies upon.
 So, as I expected, I'm good to go with ATTiny85 implementation. Stay tuned. Note: if you're familiar with my project:
 https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
 I've made a few changes to my original code including moving most of the tasks to .h files and making the Debugger Task
 an optional compile/runtime option. I've also offloaded the #defines to a Config.h file. 
 
 The code included with this project uses the following Tasks:
 
 Debugger (Conditional)		Task		-	Provides debug info to Serial Monitor.
 Blinker			TimedTask 	- 	Blink an LED on pin 6
 ArcReactor (don't laugh!)	TimedTask 	- 	LED on pin 5 that pulses/fades (Iron Man style) 
		
 AppManager 			TimedTask 	- 	Main Switchboard for the application. Runs at a rate
							of 100ms to read a volatile variable (ledCycleCount)
							set by ISR(PCINT2_vect) an interrupt activated by a 
							switch attached to pin 3 (pulled up). The AppManager
							decrements the ledCycleCount until it reaches zero
							(no activity on switch), when this occurs, the AppManager
							disables a few peripherals (more can be done later),
							disables pin outputs, then finally puts the Arduino to
							sleep until the next interrupt increments the ledCycleCount.
													
 Other files in the project:

		Config.h					Pin numbers, timing config, etc.
		Task.h						...
		Task.cpp					...
		TaskScheduler.h					...
		TaskScheduler.cpp				...
		Arduino_TaskScheduler_with_Sleep.ino		Arduino IDE code
		Fritzing Wiring Diagram in Fritzing Foler	Schematic
 
 Power Measurement Results with minimal power reduction in SLEEP_MODE_PWR_DOWN:
 
		Running LEDs:		80.5 - 85.3 mA
		Sleeping:		29.8 - 32.1 mA
		
		(Not bad, but there is more that could be done. My final project will be ATTiny85 based and I plan to get
		down to uA range without much trouble.)


		
Any Comments, Suggestions or Questions are welcome.

Regards,
Kevin

@GadgetsToGrow		
https://www.hackster.io/GadgetsToGrow/
gadgetstogrow -> gmail dot com