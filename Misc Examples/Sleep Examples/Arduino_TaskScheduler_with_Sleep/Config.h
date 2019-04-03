/*************************************************************************************************
 * Config.h
 * 
 * Purpose: Configuration Setting for this application
 * 
 * Copyright © 2016 Kevin Gagnon. All Rights Reserved.
 *
 * This file is part of a Task Scheduler Demo Series:
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

#ifndef CONFIG_H_
#define CONFIG_H_


//-------------------------------------------------------------------
// Volatile variable for Pin Change Interrupt PCINT2_vect ISR to increment
//-------------------------------------------------------------------
volatile uint8_t ledCycleCount = 0;


// ***
// *** Pin Definitions
// ***
#define LED_BLINKER_PIN			6		//Blinker
#define ARC_REACTOR_PIN			5		//Fade Up/Down LED
#define TILT_SENSOR_PIN			3		//Interrupt Pin

// ***
// *** Utility Definitions
// ***
#define BLINKER_RATE_MS			300
#define APP_MANAGER_RATE_MS		100		

//-------------------------------------------------------------------
// LED Thresholds/Cycles/Rates
//-------------------------------------------------------------------
#define CYCLE_RATE_THRESHOLD_FAST			90	//ledCycleCount 180-90
#define CYCLE_RATE_THRESHOLD_MEDIUM			30	//ledCycleCount 93-30
#define CYCLE_RATE_THRESHOLD_SLOW			0	//ledCycleCount 30-0

#define CYCLE_RATE_FAST_MS					100 //cycleRate in ms to blink LED
#define CYCLE_RATE_MEDIUM_MS				200 
#define CYCLE_RATE_SLOW_MS					300	

#define CYCLE_INCREMENT_BY_X				5	//The amount to increment the ledCycleCount.
#define CYCLE_COUNT_MAX						180	//Don't let the switch get out of hand! Set max limit.

#endif /* CONFIG_H_ */