// 
// 
// 

#include "PulseRelayManager.h"

PulseRelayManager::PulseRelayManager(int pin, unsigned long pulseDelay /* = 500 */) : RelayManager(pin), _pulseDelay(pulseDelay)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

void PulseRelayManager::Set(int value)
{
	RelayManager::Set(value);
	_startTime = millis();
}

void PulseRelayManager::OnCommand(const std::string& commandName, int commandId)
{
	if (commandName == "Pulse")
		Set(HIGH);
}

void PulseRelayManager::Loop()
{
	if (_startTime != 0 && millis() - _startTime <= _pulseDelay)
		return;

	RelayManager::Set(LOW); //end pulse
	_startTime = 0;
}

