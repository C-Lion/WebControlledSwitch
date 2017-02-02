// RelayManager.h

#ifndef _PULSERELAYMANAGER_h
#define _PULSERELAYMANAGER_h
#include "RelayManager.h"
#include "Singleton.h"

class PulseRelayManager : public RelayManager, public Singleton<PulseRelayManager>
{
	friend class Singleton<PulseRelayManager>;
private:
	unsigned long _startTime = 0;
	const unsigned long _pulseDelay;
	PulseRelayManager(int pin, unsigned long pulseDelay, std::function<void(const String &)> logger);

 public:
	void Set(int value) override;

	void OnCommand(const String & commandName, int commandId) override;
	void Loop() override;
};

#endif

