// 
// 
// 

#include "RelayManager.h"

RelayManager::RelayManager(int pin) : _pin(pin), _state(LOW)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

/* virtual */ void RelayManager::Set(int value)
{
	_state = value;
	digitalWrite(_pin, _state);
}

