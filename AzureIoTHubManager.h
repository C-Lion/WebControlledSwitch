// AzureIoTHubManager.h

#ifndef _AZUREIOTHUBMANAGER_h
#define _AZUREIOTHUBMANAGER_h

#include "Arduino.h"
#include "AzureIoTHubHttpClient.h"
#include "AzureIoTHub.h"
#include <functional>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"
#include "Singleton.h"
#include "ArduinoLoopManager.h"

class SwitchController;

typedef std::function<void(const std::string&, int)> IAzureIoTHubCommandNotificationPtr_t;
class AzureIoTHubManager : public Singleton<AzureIoTHubManager>, public IProcessor
{
	friend class Singleton<AzureIoTHubManager>;
	friend EXECUTE_COMMAND_RESULT Activate(SwitchController* device, char *logInfo);
	friend EXECUTE_COMMAND_RESULT TurnOn(SwitchController* device, char *logInfo);
	friend EXECUTE_COMMAND_RESULT TurnOff(SwitchController* device, char *logInfo);

private:
	static WiFiClientSecure _sslWiFiClient;
	static AzureIoTHubClient _iotHubClient;
	bool _isTimeInitiated = false;
	bool _isIotHubClientInitiated = false;
	PubSub<AzureIoTHubManager, const std::string&, int> _pubsub;
	bool _relayState = false;
	LoggerPtr_t _logger;
	bool _IsInitTime = false;
	int _loopStartTime = millis();
	const char* _azureIoTHubDeviceConnectionString;
	void UpdateStatus(ConnectionStatus status);
	bool CheckTimeInitiated();
	bool CheckIoTHubClientInitiated();
	void OnActivate(const char* logInfo) const;
	void OnTurnOn(const char* logInfo) const;
	void OnTurnOff(const char* logInfo) const;
	void HandleCommand(const std::string & commandName, int commandId, const char *logInfo) const;
	AzureIoTHubManager(WiFiManagerPtr_t wifiManager, LoggerPtr_t logger, const char* connectionString);
	
 public:
	void Register(WebNotificationPtr_t subscriber) { _pubsub.Register(subscriber); }
	void Loop() override;
	void UpdateRelayState(char *deviceId, int state) const;
};


typedef std::shared_ptr<AzureIoTHubManager> AzureIoTHubManagerPtr_t;
#endif

