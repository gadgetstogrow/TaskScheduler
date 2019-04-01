
*********************************************************************************************
*********************************************************************************************
Author: Kevin Gagnon @GadgetsToGrow

Copyright © 2016 Kevin Gagnon. All Rights Reserved.

Tutorial URL: https://www.hackster.io/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe


GitHub: https://github.com/gadgetstogrow/TaskScheduler

Date: 3-31-2019
**********************************************************************************************
**********************************************************************************************

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
IMPORTANT: The .ino expects TaskScheduler.h, TaskScheduler.cpp, Task.h and Task.cpp to be included in the same folder.  If you don't have the latest libraries you can download them from the GitHub Link above.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Purpose: Simple solution based on a user request: How to use an interrupt in concert with the TaskScheduler.  

Theory: Intitializes Timer 2 on the Arduino Uno with Fast PWM output on pin 11 (OC2A) to simulate a pseudo RC servo signal input read on pin 2. The interrupt is on D2/INT0 and is handled first by ISR(INT0_vect) to set a volatile variable: interruptTriggered. The InterruptHandler Task uses this flag in its canRun() method to return true/false to the TaskScheduler during execution. The run() method counts the number of interrupts and will toggle LED connected to pin 3 when the count reaches multiples of 1000 (approx. 1 sec blink).

There is a Quick Reference sheet for Timer 2 setting in the References folder and a Fritzing diagram in the Fritzing folder.

If you have any questions please feel free to contact me @GadgetsToGrow or on my Hackster.io Profile. And most of all: have fun.

Regards,

Kevin


License Info:
/*************************************************************************************************

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
/*************************************************************************************************

