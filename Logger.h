// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#include "arduino.h"
#include "LedsLogger.h"
#include "WebServer.h"
#include <memory>
#include <string>

class Logger : public IWebCommands
{
private:
	LedsLogger _ledLogger;

public:
	Logger(int redLedPin, int greenLedPin, int baudRate = 115200);

	void Loop() { _ledLogger.Loop(); }
	void OnTurnOn() override;
	void OnTurnOff() override;
	void OnConnected(ConnectionStatus status, IPAddress ipAddress) override;
	void OnDisconnected(ConnectionStatus status) override;
	void OnError(ConnectionStatus status) override;
	void WriteMessage(const std::string& message);
};

typedef std::shared_ptr<Logger> LoggerPtr_t;

#endif

