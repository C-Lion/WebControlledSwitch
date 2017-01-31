// 
// 
// 

#include "PulseRelayManager.h"

PulseRelayManager::PulseRelayManager(int pin, unsigned long pulseDelay, std::function<void(const std::string &)> logger) : RelayManager(pin, logger), _pulseDelay(pulseDelay)
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
	Serial.print("Pulse relay manager received command:");
	Serial.print(commandName.c_str());
	if (commandName == "Activate")
		Set(HIGH);
}

void PulseRelayManager::Loop()
{
	if (_startTime == 0 || (_startTime != 0 && millis() - _startTime <= _pulseDelay))
		return;

	RelayManager::Set(LOW); //end pulse
	_startTime = 0;
}

