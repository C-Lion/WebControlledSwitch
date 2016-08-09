// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#include "arduino.h"
#include "LedsLogger.h"
#include "WebServer.h"
#include <memory>
#include <string>
#include "Singleton.h"


class Logger : public IWebNotifications, public Singleton<Logger>
{
	friend class Singleton<Logger>;
private:
	LedsLoggerPtr_t _ledsLogger;
	Logger(int redLedPin, int greenLedPin, int baudRate);
public:
	void Loop() { _ledsLogger->Loop(); }
	void OnCommand(const std::string & commandName, int commandId) override;
	void OnConnected(ConnectionStatus status) override;
	void OnDisconnected(ConnectionStatus status) override;
	void OnError(ConnectionStatus status) override;
	void WriteErrorMessage(const std::string &message, int blinks);
	static void WriteMessage(const std::string& message);
	template<typename T>
	static void WriteMessage(const T& message) { Serial.println(message); }
	void TestLeds();
};

typedef std::shared_ptr<Logger> LoggerPtr_t;


#endif

