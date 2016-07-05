
#include "RelayManager.h"
#include "Util.h"
#include "WebServer.h"
#include "Logger.h"
#include "PushButtonManager.h"
#include <memory>

using namespace std;

//Board configuration
int pushButton = 5;
int redLed = 4;
int greenLed = 15;
int relay = 2;

//Wifi Configuration
const char *SSID = "[your wifi access point ssid]";
const char *password = "[your wifi access point password]";
const char *webSiteHeader = "Welcome to the web light switch Server";
const char *appKey = "[your secret app key]";


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
	
}

void loop()
{
	server->Loop(relayManager->State());
	logger->Loop();
	pushButtonManager->Loop();
}

void SwitchRelayState(int state)
{
	relayManager->Set(state);
	string message("Relay has been ");
	message += state == LOW ? "deactivated" : "activated";
	logger->WriteMessage(message);
}

void Reset()
{
	Util::software_Reboot();
}