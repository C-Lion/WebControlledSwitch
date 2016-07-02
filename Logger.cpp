// 
// 
// 

#include "Logger.h"


Logger::Logger(int redLedPin, int greenLedPin, int baudRate /*= 115200*/): _ledLogger(redLedPin, greenLedPin)
{
	Serial.begin(baudRate);
}

void Logger::OnTurnOn()
{
	Serial.println("Turn on command recieved");
	_ledLogger.BlinkGreen(2, 250);
}

void Logger::OnTurnOff()
{
	Serial.println("Turn off command recieved");
	_ledLogger.BlinkGreen(4, 250);
}

void Logger::OnConnected(ConnectionStatus status, IPAddress ipAddress)
{
	Serial.println(status.Message().c_str());
	Serial.print("IP address: ");
	Serial.println(ipAddress);
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
