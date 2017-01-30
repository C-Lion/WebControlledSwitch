// PushButtonManager.h

#ifndef _PUSHBUTTONMANAGER_h
#define _PUSHBUTTONMANAGER_h

#include "arduino.h"
#include <functional>
#include <memory>
#include "Singleton.h"
#include "ArduinoLoopManager.h"

class IPushButtonActions
{
public:
	virtual void OnStateChanged(int state) = 0; //called when the user pressed and stopped the button for short period
	virtual int GetLongPressPeriod() = 0; //set the time that will be considered as a long press
	virtual void OnLongPressDetected() = 0; //called when the user presses for long time and continues to hold the button
	virtual void OnLongPress() = 0; //called when the user pressed and stopped the button for long period
	virtual int GetVeryLongPressPeriod() = 0; //called when the user presses for very long time and continues to hold
	virtual void OnVeryLongPressDetected() = 0; //called when the user presses for very long time and continues to hold the button
	virtual void OnVeryLongPress() = 0; //called when the user pressed and stopped the button for very long period

	virtual ~IPushButtonActions() {}
};

typedef std::shared_ptr<IPushButtonActions> IPushButtonActionsPtr_t;

class PushButtonManager : public IProcessor
{
private:
	const int _pin;
	int _state = LOW;
	int _previousButtonState = LOW;
	int _pressStartTime = 0;
	IPushButtonActionsPtr_t _pushButtonActions;
	bool _bLongDetection = false;
	bool _bVeryLongDetection = false;

protected:
	virtual int StateOnTrigger() const = 0;

public:
	virtual ~PushButtonManager()
	{
	}

	PushButtonManager(int pin, IPushButtonActionsPtr_t pushButtonActions);
	void Loop() override;
	int State() const { return _state; }
};

typedef std::shared_ptr<PushButtonManager> PushButtonManagerPtr_t;


#endif

