// 
// 
// 

#include "RelayManager.h"

RelayManager::RelayManager(int pin, unsigned long pulseDelay /* = 500 */) : _pin(pin), _state(LOW), _pulseDelay(pulseDelay)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

void RelayManager::Set(int value, bool isPulse /* = false*/)
{
	_state = value;
	digitalWrite(_pin, _state);

	if (isPulse)
		_startTime = millis();
}

void RelayManager::OnCommand(const std::string &commandName, int commandId)
{
	if (commandName == "On")
		Set(HIGH);
	else if (commandName == "Off")
		Set(LOW);
	else if (commandName == "Pulse")
	{
		Set(HIGH, true);
	}
}

void RelayManager::Loop()
{
	if (_startTime != 0 && millis() - _startTime <= _pulseDelay)
		return;
	Set(_state == HIGH ? LOW : HIGH); //end pulse
	_startTime = 0;
}
