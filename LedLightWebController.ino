
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
#include "Util.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "WaterSensorManager.h"
#include "Logger.h"
#include "TogglePushButtonManager.h"
#include "AzureIoTHubManager.h"
#include <memory>
#include "Configuration.h"

using namespace std;




void Reset();
void ResetToAccessPointMode();
LoggerPtr_t logger;
WiFiManagerPtr_t wifiManager;
WaterSensorManagerPtr_t waterSensorManager;
PushButtonManagerPtr_t pushButtonManager;
ArduinoLoopManager_t loopManager;
ConfigurationManager_t configurationManger;
AzureIoTHubManagerPtr_t azureIoTHubManager;

void SetupAzureIoTHubManager()
{
	char *connectionString = strdup(configurationManger->GetAzureIoTConnectionString().c_str()); //use only once, so not really a memory leak
	azureIoTHubManager = AzureIoTHubManager::Create(wifiManager, logger, connectionString);
}

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

	webServer = WebServer::Create(wifiManager, 80, appKey, std::move(deviceSettings), []() { return waterSensorManager->State(); });
	 
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
		configurationManger->SetButonPressTimesMilliSeconds(deviceSettings.longButtonPeriod, deviceSettings.veryLongButtonPeriod);
		configurationManger->FlashEEProm();
	});
}



class PushButtonActions final : public IPushButtonActions
{
private:
	void OnStateChanged(int state) override {	}
	int GetLongPressPeriod() override { return 5000; } //5 seconds
	void OnLongPressDetected() override { logger->OnLongButtonPressDetection(); }
	void OnLongPress() override { Reset(); }
	int GetVeryLongPressPeriod() override { return 20000; }//20 seconds
	void OnVeryLongPressDetected() override { logger->OnVeryLongButtonPressDetection(); }
	void OnVeryLongPress() override { ResetToAccessPointMode(); }
};

void ReportWaterStatus(bool state)
{
	if (azureIoTHubManager)
		azureIoTHubManager->ReportWaterStatus(configurationManger->GetIoTHubDeviceId().c_str(), state ? "Water Alarm" : "No Alarm");
}

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
	
	waterSensorManager = WaterSensorManager::Create(WaterSensor, BlueLed, WaterStateReportInterval, [=](bool state) { ReportWaterStatus(state); });
	pushButtonManager = TogglePushButtonManager::Create(pushButton, make_shared<PushButtonActions>());

	if (configurationManger->ShouldUseAzureIoTHub())
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{ logger, wifiManager, pushButtonManager, waterSensorManager, azureIoTHubManager });
	}
	else
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{logger, wifiManager, pushButtonManager, waterSensorManager, webServer });
	}
	logger->TestLeds();
	configurationManger->DumpEEPromInfo();
}



void loop()
{
	loopManager->Loop();
}


void ResetToAccessPointMode()
{
	configurationManger->FacrotyReset(); //switch to access point mode and reset the device
}

void Reset()
{
	Util::software_Reboot();
}