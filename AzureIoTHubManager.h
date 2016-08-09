// AzureIoTHubManager.h

#ifndef _AZUREIOTHUBMANAGER_h
#define _AZUREIOTHUBMANAGER_h

#include "Arduino.h"
#include "AzureIoTHubHttpClient.h"
#include "AzureIoTHub.h"
#include <vector>
#include <functional>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"
#include "Singleton.h"

class IAzureIoTHubCommandNotification 
{
public:
	virtual void OnCommand(const std::string & commandName, int commandId) = 0;

	virtual ~IAzureIoTHubCommandNotification() {	}
};
typedef std::shared_ptr<IAzureIoTHubCommandNotification> IAzureIoTHubCommandNotificationPtr_t;

class AzureIoTHubManager : public Singleton<AzureIoTHubManager>
{
	friend class Singleton<AzureIoTHubManager>;
private:
	WiFiClientSecure _sslWiFiClient;
	AzureIoTHubClient _iotHubClient;
	bool _isTimeInitiated = false;
	bool _isIotHubClientInitiated = false;
	std::vector<IAzureIoTHubCommandNotificationPtr_t> _subscribers;
	bool _relayState = false;
	LoggerPtr_t _logger;
	void UpdateStatus(ConnectionStatus status);
	AzureIoTHubManager(WiFiManagerPtr_t wifiManager, LoggerPtr_t logger, const char* connectionString);
	
 public:
	 
	void RegisterCommand(IAzureIoTHubCommandNotificationPtr_t command);
	void HandleCommand(const std::string & commandName, int commandId, const char *logInfo) const;
	void NotifyAll(std::function<void(IAzureIoTHubCommandNotificationPtr_t)> callBack) const;
	bool CheckTimeInitiated();
	bool CheckIoTHubClientInitiated();
	void Loop(int relayState);
	void OnActivate(const char* logInfo) const;
	void OnTurnOn(const char* logInfo) const;
	void OnTurnOff(const char* logInfo) const;
};


typedef std::shared_ptr<AzureIoTHubManager> AzureIoTHubManagerPtr_t;
#endif

