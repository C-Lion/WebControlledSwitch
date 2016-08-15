// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#include "Arduino.h"
#include <ESP8266WebServer.h>
#undef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#include <ESP8266mDNS.h>
#include <string>
#include <memory>
#include "WiFiManager.h"
#include "Singleton.h"
#include "PubSub.h"
class IWebCommand
{
public:
	virtual const std::string& Name() const = 0;
	virtual int Id() const = 0;
	virtual const std::string& MenuEntry() const = 0; 
	virtual const std::string& ResultHTML() const = 0;
	virtual const std::string& TriggerUrl() const = 0;

	virtual ~IWebCommand() {}
};
typedef std::shared_ptr<IWebCommand> WebCommandPtr_t;
typedef std::function<void(const std::string&, int)> WebNotificationPtr_t;
class WebServer : public Singleton<WebServer>
{
	friend class Singleton<WebServer>;
private:
	ESP8266WebServer _server;
	PubSub<WebServer, const std::string&, int> _pubsub;
	bool _relayState = false;
	std::string _header;
	const std::string _authorizedUrl;
	std::vector<WebCommandPtr_t> _webCommands;
	bool _isInit = false;

	void SendBackHtml(const std::string &message);
	void UpdateStatus(ConnectionStatus status);
	std::string CreateUrl(const std::string &s) const;
	WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey);

 public:
	void RegisterCommand(WebCommandPtr_t command);

	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	void HandleMain();
	void HandleError();
	void HandleCommand(WebCommandPtr_t webCommand);
	void Register(WebNotificationPtr_t subscriber) { _pubsub.Register(subscriber); }
	bool IsConnected() const;
	void Loop(int relayState);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

