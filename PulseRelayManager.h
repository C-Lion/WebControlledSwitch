// RelayManager.h

#ifndef _PULSERELAYMANAGER_h
#define _PULSERELAYMANAGER_h
#include "RelayManager.h"

class PulseRelayManager : public RelayManager
{
private:
	unsigned long _startTime = 0;
	const unsigned long _pulseDelay;

 public:
	explicit PulseRelayManager(int pin, unsigned long pulseDelay = 1000);
	void Set(int value) override;

	void OnCommand(const std::string & commandName, int commandId) override;
	void Loop() override;
};

#endif

