/*
 * ArcReactor.h
 *
 * Created: 4/2/2019 8:40:29 PM
 *  Author: miste
 */ 

#ifndef ARCREACTOR_H_
#define ARCREACTOR_H_
class ArcReactor : public TimedTask
{
	public:
	ArcReactor(uint8_t _outputPin, uint16_t _cycleRate, Debugger *_ptrDebugger = NULL);
	virtual void run(uint32_t now);
	void enableLED(bool _enabled);
	void setCycleRate(uint16_t _newRate);
	private:
	uint8_t outputPin;			// LED pin.
	bool isEnabled;				// Is the Output Pin Enabled?
	uint16_t cycleRate;			// Update rate
	uint8_t brightness;			// Fader brightness
	uint8_t fadeAmount;			// Amount to fade in each step
	bool on;					// Current state of the LED.
	Debugger *ptrDebugger;		// Pointer to debugger
};

// ***
// *** ArcReactor Constructor
// ***
ArcReactor::ArcReactor(uint8_t _outputPin, uint16_t _cycleRate, Debugger *_ptrDebugger)
: TimedTask(millis()),
outputPin(_outputPin),
cycleRate(_cycleRate),
brightness(5),
fadeAmount(5),
on(false),
ptrDebugger(_ptrDebugger)
{
	pinMode(outputPin, OUTPUT);     // Set pin for output.
	isEnabled = true;
}

// ***
// *** ArcReactor::run()
// ***
void ArcReactor::run(uint32_t now)
{
	//Basic Fader
	
	analogWrite(outputPin, brightness);
	brightness += fadeAmount;
	
	if((brightness <= 5) || (brightness >= 250))
	{
		fadeAmount = -fadeAmount;
	}


	// Run again in the specified number of milliseconds.
	incRunTime(cycleRate);
	
}

// ***
// *** ArcReactor::setCycleRate - Allows AppManager access to change the blink rate.
// ***
void ArcReactor::setCycleRate(uint16_t _newRate)
{
	cycleRate = _newRate;
}

// ***
// *** ArcReactor::setCycleRate - Allows AppManager access to change the blink rate.
// ***
void ArcReactor::enableLED(bool _enable)
{
	if(_enable)
	{
		DDRD |= (1 << outputPin);	//Enable Output
		isEnabled = true;
		} else {
		PORTD &= ~(1 << outputPin); //Write a zero
		DDRD &= ~(1 << outputPin);  //Disable Output
		isEnabled = false;
	}
}


#endif /* ARCREACTOR_H_ */