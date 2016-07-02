// Logger.h

#ifndef _LEDSLOGGER_h
#define _LEDSLOGGER_h
#include "arduino.h"
#include <memory>

class LedsLogger
{
 private:
	 class Led
	 {
	 private:
		 const int _ledPin;
		 int _ledValue = 0;
		 int _times = 0;
		 int _blinkDelay = 0;
		 unsigned long _startTime = 0;
	 public:
		 Led(int ledPin);
		 void Update();
		 void Blink(int times, int delay);
		 void Set(int value);
	 };

	 Led _red;
	 Led _green;
	 
 public:
	 LedsLogger(int redLedPin, int greenLedPin) : _red(redLedPin), _green(greenLedPin) {}
	 void BlinkRed(int times, int delay) { _red.Blink(times, delay); }
	 void BlinkGreen(int times, int delay) { _green.Blink(times, delay); }
	 void SetRed(int value) { _red.Set(value); }
	 void SetGreen(int value) { _green.Set(value); }
	 void Loop() 
	 { //call this function in a tight interval
		 _red.Update();
		 _green.Update();
	 }
};

typedef std::shared_ptr<LedsLogger> LedsLoggerPtr_t;

#endif

