#include "AzureIoTHubManager.h"
#include <Arduino.h>

using namespace std;

AzureIoTHubManager::AzureIoTHubManager(WiFiManagerPtr_t wifiManager, LoggerPtr_t logger, const char* connectionString) : _iotHubClient(_sslWiFiClient), _logger(logger)
{
	configTime(0, 0, "pool.ntp.org", "time.nist.gov");
	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
}


void AzureIoTHubManager::RegisterCommand(IAzureIoTHubCommandNotificationPtr_t command)
{
	_subscribers.push_back(command);
}


void AzureIoTHubManager::HandleCommand(const std::string & commandName, int commandId, const char *logInfo) const
{
	_logger->WriteMessage("Recieved command: ");
	_logger->WriteMessage(commandName);
	_logger->WriteMessage(" from Azure IoTHub with log information: ");
	_logger->WriteMessage(logInfo);
	NotifyAll([=](IAzureIoTHubCommandNotificationPtr_t subscriber) {subscriber->OnCommand(commandName, commandId); });
}

void AzureIoTHubManager::NotifyAll(function<void(IAzureIoTHubCommandNotificationPtr_t)> callBack) const
{
	for (auto subscriber : _subscribers)
	{
		callBack(subscriber);
	}
}

bool AzureIoTHubManager::CheckTimeInitiated()
{
	if (_isTimeInitiated)
		return true;

	time_t epochTime;

	epochTime = time(NULL);

	if (epochTime == 0)
	{
		_logger->WriteErrorMessage("Fetching NTP epoch time failed!", 5);
		delay(300);
	}
	else
	{
		_logger->WriteMessage("Fetched NTP epoch time is: ");
		_logger->WriteMessage(epochTime);
		_isTimeInitiated = true;

		_iotHubClient.begin(); //begins after setting the time
	}
}

bool AzureIoTHubManager::CheckIoTHubClientInitiated()
{
	if (_isIotHubClientInitiated)
		return true;

	if (!InitAzureIoTHubClient())
		return false;
	
	_isIotHubClientInitiated = true;
	return true;
}


void AzureIoTHubManager::Loop(int relayState)
{
	if (CheckTimeInitiated() == false) //can't do anything before setting the time
		return;
	if (CheckIoTHubClientInitiated() == false)
		return;

	_relayState = relayState;
	AzureIoTHubClientLoop(_relayState);
}

void AzureIoTHubManager::OnActivate(const char* logInfo) const
{
	HandleCommand("Activate", 1, logInfo);
}

void AzureIoTHubManager::OnTurnOn(const char* logInfo) const
{
	HandleCommand("TurnOn", 2, logInfo);
}

void AzureIoTHubManager::OnTurnOff(const char* logInfo) const
{
	HandleCommand("TurnOff", 3, logInfo);
}

void AzureIoTHubManager::UpdateStatus(ConnectionStatus status)
{
	//bool hasNotified = false;

	////Connection lost, notify once
	//if (_lastConnectionStatus == WL_CONNECTED && status.WifiCode() != WL_CONNECTED)
	//{
	//	Notify([=](WebNotificationPtr_t subscriber)
	//	{
	//		subscriber->OnDisconnected(status);
	//	});
	//	hasNotified = true;
	//}

	//if (_lastConnectionStatus != WL_CONNECTED &&  status.WifiCode() == WL_CONNECTED) //new connection, notify once
	//{
	//	_iotHubClient.begin();

	//	Notify([=](WebNotificationPtr_t subscriber)
	//	{
	//		subscriber->OnConnected(status);
	//	});
	//	hasNotified = true;
	//}
	//_lastConnectionStatus = status.WifiCode();

	//if (hasNotified)
	//	return;
	////else error
	//Notify([=](WebNotificationPtr_t subscriber)
	//{
	//	subscriber->OnError(status);
	//});
}


