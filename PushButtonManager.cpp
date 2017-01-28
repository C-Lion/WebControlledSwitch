// 
// 
// 

#include "PushButtonManager.h"

PushButtonManager::PushButtonManager(int pin, IPushButtonActionsPtr_t pushButtonActions)
	: _pin(pin), _pushButtonActions(pushButtonActions)
{
	pinMode(pin, INPUT);
}

void PushButtonManager::Loop()
{
	auto currentState = digitalRead(_pin);
	if (currentState == HIGH && _previousButtonState == LOW) //Trigger
	{
		_pressStartTime = millis();
	}

	//handle detection callbacks
	if (currentState == HIGH && _previousButtonState == HIGH) //continue press
	{
		auto length = millis() - _pressStartTime;
		if (!_bLongDetection && _pushButtonActions->GetLongPressPeriod() < length && length < _pushButtonActions->GetVeryLongPressPeriod())
		{
			_bLongDetection = true;
			_pushButtonActions->OnLongPressDetected();
		}
		else if (!_bVeryLongDetection && length > _pushButtonActions->GetVeryLongPressPeriod())
		{
			_bVeryLongDetection = true;
			_pushButtonActions->OnVeryLongPressDetected();
		}
	}

	//Handle button press
	if (currentState == LOW && _previousButtonState == HIGH && _pressStartTime != 0 && millis() - _pressStartTime > 100) //long enough
	{
		auto length = millis() - _pressStartTime;
		if (length < _pushButtonActions->GetLongPressPeriod()) //less then 5 seconds, regular press
		{
			_state = StateOnTrigger();
			_pushButtonActions->OnStateChanged(_state); //Notify change
		}
		else if (length < _pushButtonActions->GetVeryLongPressPeriod()) //less the 20 seconds
		{
			_pushButtonActions->OnLongPress();
		}
		else //over 20 seconds
		{
			_pushButtonActions->OnVeryLongPress();
		}
		//reset all
		_pressStartTime = 0;
		_bLongDetection = false;
		_bVeryLongDetection = false;
	}
	_previousButtonState = currentState;
}
