// PushButtonManager.h

#ifndef _PUSHBUTTONMANAGER_h
#define _PUSHBUTTONMANAGER_h

#include "arduino.h"
#include <functional>
#include <memory>

class PushButtonManager
{
private:
	const int _pin;
	int _state = LOW;
	int _previousButtonState = LOW;
	int _pressStartTime = 0;
	int _longPressStartTime = 0;
	std::function<void(int)> _onStateChanged;
	std::function<void()> _onLongPress;
public:
	PushButtonManager(int pin, std::function<void (int)> onStateChanged, std::function<void()> onLongPress);
	void Loop();
	bool State() const { return _state; }
};

typedef std::shared_ptr<PushButtonManager> PushButtonManagerPtr_t;


#endif

