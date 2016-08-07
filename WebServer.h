// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#include "Arduino.h"
#include <ESP8266WebServer.h>
#undef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#include <ESP8266mDNS.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "WiFiManager.h"

class IWebNotifications
{
public:
	virtual void OnCommand(const std::string & commandName, int commandId) = 0;
	virtual void OnConnected(ConnectionStatus status) = 0;
	virtual void OnDisconnected(ConnectionStatus status) = 0;
	virtual void OnError(ConnectionStatus status) = 0;

	virtual ~IWebNotifications() {	}
};
typedef std::shared_ptr<IWebNotifications> WebNotificationPtr_t;

class IWebCommand
{
public:
	virtual const std::string& Name() const = 0;
	virtual const int Id() const = 0;
	virtual const std::string& MenuEntry() const = 0; 
	virtual const std::string& ResultHTML() const = 0;
	virtual const std::string& TriggerUrl() const = 0;

	virtual ~IWebCommand() {}
};
typedef std::shared_ptr<IWebCommand> WebCommandPtr_t;

class WebServer
{
private:
	ESP8266WebServer _server;
	std::vector<WebNotificationPtr_t> _subscribers;
	bool _relayState = false;
	std::string _header;
	const std::string _authorizedUrl;
	std::vector<WebCommandPtr_t> _webCommands;
	int _lastConnectionStatus;

	void SendBackHtml(const std::string &message);
	void UpdateStatus(ConnectionStatus status);
	std::string CreateUrl(const std::string &s) const;

 public:
	WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey);
	void RegisterCommand(WebCommandPtr_t command);

	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	void HandleMain();
	void HandleError();
	void Notify(std::function<void (WebNotificationPtr_t)> callBack);
	void HandleCommand(WebCommandPtr_t webCommand);
	void Register(WebNotificationPtr_t subscriber);
	bool IsConnected() const;
	void Loop(int relayState);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

