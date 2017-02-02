#include "Logger.h"
using namespace std;

Logger::Logger(int redLedPin, int greenLedPin, int baudRate /*= 115200*/): _ledsLogger(LedsLogger::Create(redLedPin, greenLedPin))
{
	Serial.begin(baudRate);
	Serial.setDebugOutput(true);
}

void Logger::OnCommand(const std::string & commandName, int commandId) const
{
	string log(commandName + " command recieved");
	Serial.println(log.c_str());
	_ledsLogger->BlinkGreen(commandId, 250);
}

void Logger::WriteErrorMessage(const std::string& message, int blinks) const
{
	Serial.println(message.c_str());
	_ledsLogger->BlinkRed(blinks, 300);
}

void Logger::OnWiFiStatusChanged(const ConnectionStatus& status) const
{
	Serial.println(status.Message().c_str());

	if (status.IsAccessModeOn())
	{
		_ledsLogger->BlinkGreen(1000000, 100);
		_ledsLogger->BlinkRed(1000000, 100);
		return;
	}
	if (status.IsJustConnected())
	{
		Serial.print("IP address: ");
		Serial.println(status.LocalIP());
	}
	_ledsLogger->SetGreen(status.IsConnected() ? HIGH : LOW);
	_ledsLogger->SetRed(status.IsConnected() ? LOW : HIGH);

	if (status.IsConnected())
	{
		_ledsLogger->BlinkIpAddress(status.LocalIP());
	}
	else
	{
		_ledsLogger->BlinkRed(3 + status.WifiCode(), 250);
	}
	

}

void Logger::OnLongButtonPressDetection() const
{
	_ledsLogger->BlinkGreen(1000, 40);
	_ledsLogger->BlinkRed(1000, 40);
}

void Logger::OnVeryLongButtonPressDetection() const
{
	_ledsLogger->BlinkGreen(10000, 20);
	_ledsLogger->BlinkRed(10000, 20);
}

void Logger::WriteMessage(const std::string& message)
{
	Serial.println(message.c_str());
}

void Logger::TestLeds() const
{
	_ledsLogger->BlinkGreen(3, 100);
	_ledsLogger->BlinkRed(3, 100);
}
