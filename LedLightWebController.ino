#include <Arduino.h>
#include "RelayManager.h"
#include "Util.h"
#include "WebServer.h"
#include "Logger.h"
#include "PushButtonManager.h"
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
WebServerPtr_t server;
RelayManagerPtr_t relayManager;
PushButtonManagerPtr_t pushButtonManager;

void setup()
{
	logger = make_shared<Logger>(redLed, greenLed);
	server = make_shared<WebServer>(80, SSID, password, appKey);
	server->SetWebSiteHeader(string(webSiteHeader));
	relayManager = make_shared<RelayManager>(relay);
	pushButtonManager = make_shared<PushButtonManager>(pushButton, &SwitchRelayState, &Reset);
	server->Register(logger);
	server->Register(relayManager);

#ifdef PULSE_COMMAND
	make_shared<WebCommand>(pulseMenuEntry, "Activate", server)->Register();
#else
	make_shared<WebCommand>(turnOnMenuEntry, "On", server)->Register();
	make_shared<WebCommand>(turnOffMenuEntry, "Off", server)->Register();
#endif
}

void loop()
{
	server->Loop(relayManager->State());
	logger->Loop();
	pushButtonManager->Loop();
	relayManager->Loop();
}

void SwitchRelayState(int state)
{
#ifdef PULSE_COMMAND
	relayManager->Set(state, true);
#else
	relayManager->Set(state);
#endif
	string message("Relay has been ");
	message += state == LOW ? "deactivated" : "activated";
	logger->WriteMessage(message);
}

void Reset()
{
	//Util::software_Reboot();
}