// RelayManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include "WebServer.h"

class RelayManager : public IWebNotifications
{
private:
	int _pin;
	int _state;
	unsigned long _startTime = 0;
	const unsigned long _pulseDelay;

 public:
	RelayManager(int pin, unsigned long pulseDelay = 500);
	void Set(int value, bool isPulse = false);
	int State() const { return _state; }

	void OnCommand(const std::string & commandName, int commandId) override;
	void OnConnected(ConnectionStatus status, IPAddress ipAddress) override {}
	void OnDisconnected(ConnectionStatus status) override {}
	void OnError(ConnectionStatus status) override {}
	void Loop();
};

typedef std::shared_ptr<RelayManager> RelayManagerPtr_t;
#endif

