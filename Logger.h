// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#include "arduino.h"
#include "LedsLogger.h"
#include "WebServer.h"
#include <memory>
#include <string>
#include "Singleton.h"


class Logger : public Singleton<Logger>
{
	friend class Singleton<Logger>;
private:
	LedsLoggerPtr_t _ledsLogger;
	Logger(int redLedPin, int greenLedPin, int baudRate);
public:
	void Loop() const{ _ledsLogger->Loop(); }
	void OnCommand(const std::string & commandName, int commandId) const;
	void WriteErrorMessage(const std::string &message, int blinks) const;
	void OnWiFiStatusChanged(const ConnectionStatus& status) const;
	static void WriteMessage(const std::string& message);
	template<typename T>
	static void WriteMessage(const T& message) { Serial.println(message); }
	void TestLeds() const;
};

typedef std::shared_ptr<Logger> LoggerPtr_t;


#endif

