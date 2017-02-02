#include "RelayManager.h"
using namespace std;

RelayManager::RelayManager(int pin, std::function<void(const String &)> logger) : _pin(pin), _state(LOW), _logger(logger)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	_logger("Rellay has been initially deactivated");
}

/* virtual */ void RelayManager::Set(int value)
{
	_state = value;
	digitalWrite(_pin, _state);

	String message("Relay has been ");
	message += _state == LOW ? "deactivated" : "activated";
	_logger(message);
}

