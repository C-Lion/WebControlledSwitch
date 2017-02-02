// RelayManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"

class RelayManager : public IProcessor
{
private:
	int _pin;
	int _state;
	std::function<void (const String &)> _logger;

protected:
	RelayManager(int pin, std::function<void(const String &)> logger);

 public:
	virtual ~RelayManager()	{}
	virtual void OnCommand(const String & commandName, int commandId) = 0;
	virtual void Set(int value);
	int State() const { return _state; }
	void Loop() override {}
};

typedef std::shared_ptr<RelayManager> RelayManagerPtr_t;
#endif

