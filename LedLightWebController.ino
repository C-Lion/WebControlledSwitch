
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_HTTP.h>
#include <AzureIoTHubClient.h>
#include <dummy.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "ConfigurationManager.h"
#include "ArduinoLoopManager.h"
#include "PubSub.h"
#include "AzureIoTHubHttpClient.h"
#include "Singleton.h"
#include "OnOffRelayManager.h"
#include "PulseRelayManager.h"
#include "Util.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "Logger.h"
#include "MementaryPushButtonManager.h"
#include "TogglePushButtonManager.h"
#include "AzureIoTHubManager.h"
#include <memory>
#include "Configuration.h"

using namespace std;

//Board configuration
int pushButton = 5; //D1
int redLed = 4; //D2
int greenLed = 15; //D8
int relay = 2; //D4


void SwitchRelayState(int state);
void Reset();
void ResetToAccessPointMode();
LoggerPtr_t logger;
WiFiManagerPtr_t wifiManager;
RelayManagerPtr_t relayManager;
PushButtonManagerPtr_t pushButtonManager;
ArduinoLoopManager_t loopManager;
ConfigurationManager_t configurationManger;

template<typename T>
void SubscribeRemoteCommands(std::shared_ptr<T> server)
{
	server->Register([](const String &commandName, int id) {logger->OnCommand(commandName, id); });
	server->Register([](const String &commandName, int id) {relayManager->OnCommand(commandName, id); });
}


AzureIoTHubManagerPtr_t azureIoTHubManager;

void SetupAzureIoTHubManager()
{
	azureIoTHubManager = AzureIoTHubManager::Create(wifiManager, logger, azureIoTHubDeviceConnectionString);
	SubscribeRemoteCommands(azureIoTHubManager);
}

class WebCommand : public IWebCommand, public enable_shared_from_this<WebCommand>
{
private:
	static int s_id;
	const String _menuEnrty;
	const String _commandName;
	const String _resultHtml;
	const int _id = ++s_id;
	weak_ptr<WebServer> _webServer;

public:
	WebCommand(String menuEntry, String commandName, WebServerPtr_t webServer) : _menuEnrty(menuEntry), _commandName(commandName),
		_resultHtml(String("Processing ") + _commandName + " Command"), _webServer(webServer)
	{
	}

	void Register()
	{
		_webServer.lock()->RegisterCommand(shared_from_this());
	}

	const String& MenuEntry() const override
	{
		return _menuEnrty;
	}

	const String& Name() const override
	{
		return _commandName;
	}

	const String& ResultHTML() const override
	{
		return _resultHtml;
	}
	const String& TriggerUrl() const override
	{
		return _commandName;
	}

	int Id() const override
	{
		return _id;
	}
};

int WebCommand::s_id = 0;
WebServerPtr_t webServer;


void SetupWebServer()
{
	auto deviceSettings = unique_ptr<DeviceSettings>(new DeviceSettings());
	deviceSettings->isFactoryReset = false;
	deviceSettings->ssidName = configurationManger->GetSSID();
	deviceSettings->accessPointPassword = configurationManger->GetAccessPointPassword();
	deviceSettings->shouldUseAzureIoT = configurationManger->ShouldUseAzureIoTHub();
	deviceSettings->azureIoTHubConnectionString = configurationManger->GetAzureIoTConnectionString();
	deviceSettings->AzureIoTDeviceId = configurationManger->GetIoTHubDeviceId();
	deviceSettings->longButtonPeriod = configurationManger->GetLongPeriodButonPressTimesMilliSeconds();
	deviceSettings->veryLongButtonPeriod = configurationManger->GetVeryLongPeriodButonPressTimesMilliSeconds();
	deviceSettings->PulseActivationPeriod = configurationManger->GetPulseActivationPeriodTimesMilliSeconds();
	deviceSettings->PBBehavior = configurationManger->GetRelayMode() == RelayMode::Pulse ? PushButtonBehaviour::Pulse : PushButtonBehaviour::Toggle;

	webServer = WebServer::Create(wifiManager, 80, appKey, std::move(deviceSettings), []() { return relayManager->State(); });
	webServer->SetWebSiteHeader(String(webSiteHeader));
	webServer->SetUpdateConfiguration([](const DeviceSettings& deviceSettings)
	{
		if (deviceSettings.isFactoryReset)
		{
			configurationManger->FacrotyReset();
			return; //never reach this line, the device should reset itself
		}

		configurationManger->SetWiFiCredentials(deviceSettings.ssidName, deviceSettings.accessPointPassword);
		if (deviceSettings.shouldUseAzureIoT)
		{
			Serial.println("deviceSettings.shouldUseAzureIoT");
			configurationManger->SetAzureIoTHubInformation(deviceSettings.azureIoTHubConnectionString, deviceSettings.AzureIoTDeviceId);
		}
		else
		{
			configurationManger->SetWebServerMode();
		}
		configurationManger->SetButonPressTimesMilliSeconds(deviceSettings.longButtonPeriod, deviceSettings.veryLongButtonPeriod, deviceSettings.PulseActivationPeriod);
		configurationManger->SetRelayMode(deviceSettings.PBBehavior == PushButtonBehaviour::Pulse ? RelayMode::Pulse : RelayMode::OnOFF);
		configurationManger->FlashEEProm();
	});

	SubscribeRemoteCommands(webServer);
	if (configurationManger->GetRelayMode() == RelayMode::Pulse)
	{
		make_shared<WebCommand>(pulseMenuEntry, "Activate", webServer)->Register();
	}
	else
	{
		make_shared<WebCommand>(turnOnMenuEntry, "On", webServer)->Register();
		make_shared<WebCommand>(turnOffMenuEntry, "Off", webServer)->Register();
	}
}



class PushButtonActions final : public IPushButtonActions
{
private:
	void OnStateChanged(int state) override { SwitchRelayState(state);	}
	int GetLongPressPeriod() override { return 5000; } //5 seconds
	void OnLongPressDetected() override { logger->OnLongButtonPressDetection(); }
	void OnLongPress() override { Reset(); }
	int GetVeryLongPressPeriod() override { return 20000; }//20 seconds
	void OnVeryLongPressDetected() override { logger->OnVeryLongButtonPressDetection(); }
	void OnVeryLongPress() override { ResetToAccessPointMode(); }
};

void setup()
{
	configurationManger = ConfigurationManager::Create();

	auto storedSSID = configurationManger->GetSSID();
	auto storedPassword = configurationManger->GetAccessPointPassword();

	logger = Logger::Create(redLed, greenLed, 115200);

	if (!configurationManger->IsAccessPointMode())
	{
		Serial.println("Try to connect to WiFi Access Point");
		Serial.print("Stored SSID is:");
		Serial.println(storedSSID.c_str());
		
		wifiManager = WiFiManager::Create(storedSSID, storedPassword, false);
	}
	else //Set access point mode
	{
		Serial.println("Create Access Point Mode");
		Serial.print("Stored SSID is:");
		Serial.println(storedSSID.c_str());

		wifiManager = WiFiManager::Create(SSID, password, true);
	}

	wifiManager->RegisterClient([](const ConnectionStatus &status) { logger->OnWiFiStatusChanged(status); });

	if (configurationManger->ShouldUseAzureIoTHub())
	{
		SetupAzureIoTHubManager();
	}
	else
	{
		SetupWebServer();
	}

	if (configurationManger->GetRelayMode() == RelayMode::Pulse)
	{
		pushButtonManager = MementaryPushButtonManager::Create(pushButton, make_shared<PushButtonActions>());
		relayManager = PulseRelayManager::Create(relay, 1000, [=](const String &message) { logger->WriteMessage(message); });
	}
	else
	{
		pushButtonManager = TogglePushButtonManager::Create(pushButton, make_shared<PushButtonActions>());
		relayManager = OnOffRelayManager::Create(relay, [=](const String &message) { logger->WriteMessage(message); });
	}

	if (configurationManger->ShouldUseAzureIoTHub())
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{ logger, wifiManager, pushButtonManager, relayManager, azureIoTHubManager });
	}
	else
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{logger, wifiManager, pushButtonManager, relayManager, webServer });
	}
	logger->TestLeds();
	configurationManger->DumpEEPromInfo();
}



void loop()
{
	loopManager->Loop();
}

void SwitchRelayState(int state)
{
	relayManager->Set(state);
	if (configurationManger->ShouldUseAzureIoTHub())
	{
		azureIoTHubManager->UpdateRelayState(deviceId, state);
	}
}

void ResetToAccessPointMode()
{
	configurationManger->FacrotyReset(); //switch to access point mode and reset the device
}

void Reset()
{
	Util::software_Reboot();
}