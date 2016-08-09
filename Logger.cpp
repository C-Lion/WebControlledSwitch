#include "Logger.h"
using namespace std;

Logger::Logger(int redLedPin, int greenLedPin, int baudRate /*= 115200*/): _ledsLogger(LedsLogger::Create(redLedPin, greenLedPin))
{
	Serial.begin(baudRate);
}

void Logger::OnCommand(const std::string & commandName, int commandId)
{
	string log(commandName + " command recieved");
	Serial.println(log.c_str());
	_ledsLogger->BlinkGreen(commandId, 250);
}

void Logger::OnConnected(ConnectionStatus status)
{
	Serial.println(status.Message().c_str());
	Serial.print("IP address: ");
	Serial.println(status.LocalIP());
	_ledsLogger->SetGreen(HIGH);
	_ledsLogger->SetRed(LOW);
}

void Logger::OnDisconnected(ConnectionStatus status)
{
	Serial.println(status.Message().c_str());
	_ledsLogger->SetGreen(LOW);
	_ledsLogger->SetRed(HIGH);
	_ledsLogger->BlinkRed(3 + status.WifiCode(), 250);
}

void Logger::OnError(ConnectionStatus status)
{
	OnDisconnected(status);
}

void Logger::WriteErrorMessage(const std::string& message, int blinks)
{
	Serial.println(message.c_str());
	_ledsLogger->BlinkRed(blinks, 300);
}

void Logger::WriteMessage(const std::string& message)
{
	Serial.println(message.c_str());
}

void Logger::TestLeds()
{
	_ledsLogger->BlinkGreen(3, 100);
	_ledsLogger->BlinkRed(3, 100);
}
