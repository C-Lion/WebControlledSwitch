
#include "LedsLogger.h"

LedsLogger::Led::Led(int ledPin): _ledPin(ledPin)
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, _ledValue);
}

void LedsLogger::Led::Update()
{
	if (_times == 0 || millis() - _startTime < _blinkDelay)
		return;

	_startTime = millis();
	_ledValue = _ledValue == HIGH ? LOW : HIGH;
	digitalWrite(_ledPin, _ledValue);
	--_times;
	if (_times == 0) //at the end, set the led to the last value
		digitalWrite(_ledPin, _ledValue);
}

void LedsLogger::Led::Blink(int times, int delay)
{
	digitalWrite(_ledPin, LOW);
	_blinkDelay = delay;
	_times = times * 2; //each one is two: on and off
	_startTime = millis();
}

void LedsLogger::Led::Set(int value)
{
	_times = 0; //stop blinking
	_ledValue = value;
	digitalWrite(_ledPin, _ledValue);
}
