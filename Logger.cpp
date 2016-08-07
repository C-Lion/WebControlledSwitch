#include "Logger.h"
using namespace std;

Logger::Logger(int redLedPin, int greenLedPin, int baudRate /*= 115200*/): _ledLogger(redLedPin, greenLedPin)
{
	Serial.begin(baudRate);
}

void Logger::OnCommand(const std::string & commandName, int commandId)
{
	string log(commandName + " command recieved");
	Serial.println(log.c_str());
	_ledLogger.BlinkGreen(commandId, 250);
}

void Logger::OnConnected(ConnectionStatus status)
{
	Serial.println(status.Message().c_str());
	Serial.print("IP address: ");
	Serial.println(status.LocalIP());
	_ledLogger.SetGreen(HIGH);
	_ledLogger.SetRed(LOW);
}

void Logger::OnDisconnected(ConnectionStatus status)
{
	Serial.println(status.Message().c_str());
	_ledLogger.SetGreen(LOW);
	_ledLogger.SetRed(HIGH);
	_ledLogger.BlinkRed(3 + status.WifiCode(), 250);
}

void Logger::OnError(ConnectionStatus status)
{
	OnDisconnected(status);
}

void Logger::WriteMessage(const std::string& message)
{
	Serial.println(message.c_str());
}

void Logger::TestLeds()
{
	_ledLogger.BlinkGreen(3, 100);
	_ledLogger.BlinkRed(3, 100);
}
