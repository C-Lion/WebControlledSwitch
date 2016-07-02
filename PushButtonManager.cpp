// 
// 
// 

#include "PushButtonManager.h"


PushButtonManager::PushButtonManager(int pin, std::function<void(int)> onStateChanged, std::function<void()> onLongPress) 
	: _pin(pin), _onStateChanged(onStateChanged), _onLongPress(onLongPress)
{
	pinMode(pin, INPUT);
}

void PushButtonManager::Loop()
{
	auto currentState = digitalRead(_pin);
	if (currentState == HIGH && _previousButtonState == LOW) //Trigger
	{
		_pressStartTime = millis();
		_longPressStartTime = _pressStartTime;
	}

	if (currentState == HIGH && _pressStartTime != 0 && millis() - _pressStartTime > 100) //Long enough press, act
	{
		_pressStartTime = 0;
		_state = _state == LOW ? HIGH : LOW; //invert state
		_onStateChanged(_state); //Notify change
	}

	if (currentState == HIGH && _longPressStartTime != 0 && millis() - _longPressStartTime > 5000) //Long: 5 seconds, act
	{
		_longPressStartTime = 0;
		_onLongPress(); //Notify change
	}
	_previousButtonState = currentState;
}
