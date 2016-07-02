// RelayManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include "WebServer.h"

class RelayManager : public IWebCommands
{
private:
	int _pin;
	int _state;

 public:
	RelayManager(int pin);
	void Set(int value);
	int State() const { return _state; }

	void OnTurnOn() override { Set(HIGH); }
	void OnTurnOff() override { Set(LOW); }
	void OnConnected(ConnectionStatus status, IPAddress ipAddress) override {}
	void OnDisconnected(ConnectionStatus status) override {}
	void OnError(ConnectionStatus status) override {}
};

typedef std::shared_ptr<RelayManager> RelayManagerPtr_t;
#endif

