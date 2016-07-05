// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#include "Arduino.h"
#include <ESP8266WebServer.h>
#undef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <functional>

enum class WiFiStatus
{
	connected = 0,			// assigned when connected to a WiFi network;
	noShield = 1,			// assigned when no WiFi shield is present;
	idle = 2,				// it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
	noSSID = 3,				// assigned when no SSID are available;
	scanCompleted = 4,		// assigned when the scan networks is completed;
	connectFailed = 5,		// assigned when the connection fails for all the attempts;
	connectionLost = 6,		// assigned when the connection is lost;
	dissconnected = 7,		// assigned when disconnected from a network;
};

class ConnectionStatus
{
	friend class WebServer;
private:
	const int _status;
	static std::map<int, WiFiStatus> _statusMap;
	static std::array<std::string, 8> _messageMap;

	ConnectionStatus(int status) : _status(status) {}

public:
	int WifiCode() const { return _status; }
	WiFiStatus Code() const { return _statusMap[_status]; }
	const std::string &Message() const {return _messageMap[_status];}
};


class IWebCommands
{
public:
	virtual ~IWebCommands()
	{
	}

	virtual void OnTurnOn() = 0;
	virtual void OnTurnOff() = 0;
	virtual void OnConnected(ConnectionStatus status, IPAddress ipAddress) = 0;
	virtual void OnDisconnected(ConnectionStatus status) = 0;
	virtual void OnError(ConnectionStatus status) = 0;
};

typedef std::shared_ptr<IWebCommands> WebCommandPtr_t;

class WebServer
{
private:
	ESP8266WebServer _server;
	std::vector<WebCommandPtr_t> _subscribers;
	int _lastConnectionStatus;
	bool _relayState = false;
	std::string _header;
	const std::string _authorizedUrl;
	const std::string _onUrl;
	const std::string _offUrl;

	void SendBackHtml(const std::string &message);
	void UpdateStatus(); //update every seconds
 public:
	WebServer(int port, const char *ssid, const char *password, const char *appKey);
	
	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	void HandleMain();
	void HandleError();
	void Notify(std::function<void (WebCommandPtr_t)> callBack);
	void HandleOn();
	void HandleOff();
	void Register(WebCommandPtr_t subscriber);
	bool IsConnected() const;
	void Loop(int relayState);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

