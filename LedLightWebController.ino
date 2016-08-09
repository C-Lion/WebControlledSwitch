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

	const int Id() const override
	{
		return _id;
	}
};

int WebCommand::s_id = 0;

void SwitchRelayState(int state);
void Reset();

LoggerPtr_t logger;
WiFiManagerPtr_t wifiManager;
WebServerPtr_t server;
RelayManagerPtr_t relayManager;
PushButtonManagerPtr_t pushButtonManager;
AzureIoTHubManagerPtr_t azureIoTHubManager;

void setup()
{
	logger = Logger::Create(redLed, greenLed, 115200);
	wifiManager = WiFiManager::Create(SSID, password);
	server = WebServer::Create(wifiManager, 80, appKey);
	server->SetWebSiteHeader(string(webSiteHeader));
	server->Register(logger);
	//TODO: use the connection string from the ctor 
	azureIoTHubManager = AzureIoTHubManager::Create(wifiManager, logger, azureIoTHubDeviceConnectionString);


#ifdef PULSE_COMMAND
	pushButtonManager = MementaryPushButtonManager::Create(pushButton, &SwitchRelayState, &Reset);
	relayManager = PulseRelayManager::Create(relay, 1000, [=](const string &message) { logger->WriteMessage(message); });
	make_shared<WebCommand>(pulseMenuEntry, "Activate", server)->Register();
#else
	pushButtonManager =TogglePushButtonManager::Create(pushButton, &SwitchRelayState, &Reset);
	relayManager = OnOffRelayManager::Create(relay, [=](const string &message) { logger->WriteMessage(message); });
	make_shared<WebCommand>(turnOnMenuEntry, "On", server)->Register();
	make_shared<WebCommand>(turnOffMenuEntry, "Off", server)->Register();
#endif

	server->Register(relayManager);

	logger->TestLeds();
}

void loop()
{
	wifiManager->Loop();
	auto relayState = relayManager->State();
	server->Loop(relayState);
	azureIoTHubManager->Loop(relayState);
	logger->Loop();
	pushButtonManager->Loop();
	relayManager->Loop();
}

void SwitchRelayState(int state)
{
	relayManager->Set(state);
}

void Reset()
{
	Util::software_Reboot();
}