#include "PubSub.h"
#include "AzureIoTHubHttpClient.h"
#include "Singleton.h"
#include <iot_logging.h>
#include <AzureIoTHubClient.h>
#include <AzureIoTHub.h>
#include <Arduino.h>
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
int pushButton = 5;
int redLed = 4;
int greenLed = 15;
int relay = 2;


void SwitchRelayState(int state);
void Reset();
LoggerPtr_t logger;
WiFiManagerPtr_t wifiManager;
RelayManagerPtr_t relayManager;
PushButtonManagerPtr_t pushButtonManager;

template<typename T>
void SubscribeRemoteCommands(std::shared_ptr<T> server)
{
	server->Register([](const string &commandName, int id) {logger->OnCommand(commandName, id); });
	server->Register([](const string &commandName, int id) {relayManager->OnCommand(commandName, id); });
}

#ifdef USEAZUREIOTHUB
AzureIoTHubManagerPtr_t azureIoTHubManager;

void SetupAzureIoTHubManager()
{
	azureIoTHubManager = AzureIoTHubManager::Create(wifiManager, logger, azureIoTHubDeviceConnectionString);
	SubscribeRemoteCommands(azureIoTHubManager);
#ifdef PULSE_COMMAND

#endif

}
#else
class WebCommand : public IWebCommand, public enable_shared_from_this<WebCommand>
{
private:
	static int s_id;
	const string _menuEnrty;
	const string _commandName;
	const string _resultHtml;
	const int _id = ++s_id;
	weak_ptr<WebServer> _webServer;

public:
	WebCommand(string menuEntry, string commandName, WebServerPtr_t webServer) : _menuEnrty(menuEntry), _commandName(commandName),
		_resultHtml(string("Processing ") + _commandName + " Command"), _webServer(webServer)
	{
	}

	void Register()
	{
		_webServer.lock()->RegisterCommand(shared_from_this());
	}

	const string& MenuEntry() const override
	{
		return _menuEnrty;
	}

	const string& Name() const override
	{
		return _commandName;
	}

	const string& ResultHTML() const override
	{
		return _resultHtml;
	}
	const string& TriggerUrl() const override
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
	webServer = WebServer::Create(wifiManager, 80, appKey);
	webServer->SetWebSiteHeader(string(webSiteHeader));
	SubscribeRemoteCommands(webServer);

#ifdef PULSE_COMMAND
	make_shared<WebCommand>(pulseMenuEntry, "Activate", webServer)->Register();
#else
	make_shared<WebCommand>(turnOnMenuEntry, "On", webServer)->Register();
	make_shared<WebCommand>(turnOffMenuEntry, "Off", webServer)->Register();
#endif
}
#endif


void setup()
{
	logger = Logger::Create(redLed, greenLed, 115200);
	wifiManager = WiFiManager::Create(SSID, password);
	wifiManager->RegisterClient([](const ConnectionStatus &status) { logger->OnWiFiStatusChanged(status); });

#ifdef USEAZUREIOTHUB
	SetupAzureIoTHubManager();
#else
	SetupWebServer();
#endif	


#ifdef PULSE_COMMAND
	pushButtonManager = MementaryPushButtonManager::Create(pushButton, &SwitchRelayState, &Reset);
	relayManager = PulseRelayManager::Create(relay, 1000, [=](const string &message) { logger->WriteMessage(message); });
#else
	pushButtonManager =TogglePushButtonManager::Create(pushButton, &SwitchRelayState, &Reset);
	relayManager = OnOffRelayManager::Create(relay, [=](const string &message) { logger->WriteMessage(message); });
#endif

	logger->TestLeds();
}

void loop()
{
	logger->Loop();
	wifiManager->Loop();
	pushButtonManager->Loop();
	relayManager->Loop();
#ifdef USEAZUREIOTHUB
	azureIoTHubManager->Loop();
#else
	auto relayState = relayManager->State();
	webServer->Loop(relayState);
#endif
}

void SwitchRelayState(int state)
{
	relayManager->Set(state);
#ifdef USEAZUREIOTHUB
	azureIoTHubManager->UpdateRelayState(deviceId, state);
#endif
}

void Reset()
{
	Util::software_Reboot();
}