/*************************************************************************************************
 * Timer2_PWM_Generator.h
 * 
 * Purpose: Creates variable PWM for use by the Task Scheduler for Interrupt testing.
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


#ifndef TIMER2_PWM_GENERATOR_H_
#define TIMER2_PWM_GENERATOR_H_



#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#endif

#define T2_CONTROL_REGISTER_A	TCCR2A
#define T2_CONTROL_REGISTER_B	TCCR2B

// ***
// *** Compare Output Registers - double underscore "__" denotes &= (and equals) operation when setting bits,
// *** otherwise it's an |= (or equals) operation.
// ***

//Timer/Counter 2 Compare Output Register A
#define T2_OUTPUT_A_MODE_OFF__				~(1 << COM2A1) & ~(1 << COM2A0)
#define T2_OUTPUT_A_MODE_TOGGLE_WGM22__		~(1 << COM2A1) | (1 << COM2A0)	//WGM22 = 1 for this mode
#define T2_OUTPUT_A_MODE_NON_INVERT			(1 << COM2A1) & ~(1 << COM2A0)
#define T2_OUTPUT_A_MODE_INVERT				(1 << COM2A1) | (1 << COM2A0)
//Timer/Counter 2 Compare Output Register B
#define T2_OUTPUT_B_MODE_OFF__				~(1 << COM2B1) & ~(1 << COM2B0)
#define T2_OUTPUT_B_MODE_NON_INVERT			(1 << COM2B1) & ~(1 << COM2B0)
#define T2_OUTPUT_B_MODE_INVERT				(1 << COM2B1) | (1 << COM2B0) 

//Timer/Counter 2 Output Compare Pins
#define OCR2A_Pin		11
#define OCR2B_Pin		3

//Timer/Counter 2 Compare Register A
#define T2_OUTPUT_COMPARE_A		OCR2A
#define T2_OUTPUT_COMPARE_B		OCR2B

//16MHz Arduino UNO
#define DUTY_25			64
#define DUTY_50			128
#define DUTY_75			191


#define PRESCALE_STOP__			~(1 << CS22) & ~(1 << CS21) & ~(1 <<CS20)	//Stops Timer/Counter 2
#define PRESCALE_NONE__			~(1 << CS22) & ~(1 << CS21) | (1 << CS20)	//No Prescale - MCU Clock Speed
#define PRESCALE_8_8_KHZ__		~(1 << CS22) | (1 << CS21) | (1 << CS20)	//7.8KHz
#define PRESCALE_32_2_KHZ__		~(1 << CS22) | (1 << CS21) | (1 << CS20)	//1.95KHz		
#define PRESCALE_64_977_HZ		(1 << CS22) & ~(1 << CS21) & ~(1 << CS20)	//976.56Hz
#define PRESCALE_128_488_HZ		(1 << CS22) & ~(1 << CS21) | (1 << CS20)	//488.28Hz
#define PRESCALE_256_244_HZ		(1 << CS22) | (1 << CS21) & ~(1 << CS20)	//244.14Hz
#define PRESCALE_1024_61_HZ		(1 << CS22) | (1 << CS21) | (1 << CS20)		//61.04Hz

// Timer/Counter 2 WaveForm Generation Mode 
// TCCR2A - WGM21-WGM20 Control Register A (CRA)
// TCCR2B - WGM22 Control Register B (CRB)
#define T2_CRA_WAVEFORM_NORMAL__			~(1 << WGM21) & ~(1 << WGM20)
#define T2_CRA_WAVEFORM_PWM_PHASE_CORR__	~(1 << WGM21) | (1 << WGM20)
#define T2_CRA_WAVEFORM_CTC					(1 << WGM21) & ~(1 << WGM20)
#define T2_CRA_WAVEFORM_FAST_PWM			(1 << WGM21) | (1 << WGM20)
#define T2_CRB_WAVEFORM_SET_WGM22			(1 << WGM22)
#define T2_CRB_WAVEFORM_CLEAR_WGM22__		~(1 << WGM22)			




// ***
// *** Define the Timer1_Signal_Generator Class
// ***
class Timer2_PWM_Generator
{
	public:
	Timer2_PWM_Generator(uint8_t _pin, uint8_t _duty);
	
	private:
	uint8_t pin;
	uint8_t freq;
	uint8_t duty;

};

// ***
// *** Timer1_Signal_Generator Constructor
// ***
Timer2_PWM_Generator::Timer2_PWM_Generator(uint8_t _pin, uint8_t _duty)
	:
	pin(_pin),
	duty(_duty)

	{
		  pinMode(pin, OUTPUT);
	}



#endif /* TIMER1_SIGNAL_GENERATOR_H_ */