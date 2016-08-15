#include "AzureIoTHubManager.h"
#include <Arduino.h>

using namespace std;

WiFiClientSecure AzureIoTHubManager::_sslWiFiClient;
AzureIoTHubClient AzureIoTHubManager::_iotHubClient(_sslWiFiClient);

AzureIoTHubManager::AzureIoTHubManager(WiFiManagerPtr_t wifiManager, LoggerPtr_t logger, const char* connectionString) :  _logger(logger), _azureIoTHubDeviceConnectionString(connectionString)
{
	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
}

void AzureIoTHubManager::HandleCommand(const std::string & commandName, int commandId, const char *logInfo) const
{
	_logger->WriteMessage("Recieved command: ");
	_logger->WriteMessage(commandName);
	_logger->WriteMessage(" from Azure IoTHub with log information: ");
	_logger->WriteMessage(logInfo);
	_pubsub.NotifyAll(commandName, commandId);
}


bool AzureIoTHubManager::CheckTimeInitiated()
{
	if ( _isTimeInitiated)
		return true;
	
	if (_IsInitTime == false)
		return false;
	
	time_t epochTime;

	epochTime = time(nullptr);

	if (epochTime == 0)
	{
		_logger->WriteErrorMessage("Fetching NTP epoch time failed!", 5);
		delay(100);
		return false;
	}
	//else
	
	_logger->WriteMessage("Fetched NTP epoch time is: ");
	_logger->WriteMessage(epochTime);
	_isTimeInitiated = true;
	return true;
}

bool AzureIoTHubManager::CheckIoTHubClientInitiated()
{
	if (_isIotHubClientInitiated)
		return true;

	_iotHubClient.begin();

	if (!AzureIoTHubInit(_azureIoTHubDeviceConnectionString))
		return false;
	_isIotHubClientInitiated = true;
	return true;
}



void AzureIoTHubManager::Loop()
{
	if (CheckTimeInitiated() == false) //can't do anything before setting the time
		return;
	if (CheckIoTHubClientInitiated() == false)
		return;


	if ((millis() - _loopStartTime) < 500)
		return;
	_loopStartTime = millis();
	AzureIoTHubLoop();
}

void AzureIoTHubManager::UpdateRelayState( char *deviceId, int state) const
{
	if (_isIotHubClientInitiated)
		AzureIoTHubSendMessage(deviceId, state, 1);
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
	if (status.IsJustConnected() && !_IsInitTime) //new connection, only once
	{
		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		_IsInitTime = true;
	}
}


