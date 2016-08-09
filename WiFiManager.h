// WiFiManager.h

#ifndef _WIFIMANAGER_h
#define _WIFIMANAGER_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <functional>
#include "Singleton.h"

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
	friend class WiFiManager;
private:
	const int _status;
	const IPAddress _localIP;
	static std::map<int, WiFiStatus> _statusMap;
	static std::array<std::string, 8> _messageMap;

	ConnectionStatus(int status, IPAddress localIP) : _status(status), _localIP(localIP) {}

public:
	int WifiCode() const { return _status; }
	WiFiStatus Code() const { return _statusMap[_status]; }
	const std::string &Message() const {return _messageMap[_status];}
	IPAddress LocalIP() const { return _localIP; }
};

typedef std::function<void (ConnectionStatus)> wifiNotificarionFunc_t;

class WiFiManager : public Singleton<WiFiManager>
{
	friend class Singleton<WiFiManager>;
private:

	std::vector<wifiNotificarionFunc_t> _subscribers;
	int _lastConnectionStatus;
	
	void UpdateStatus();
	WiFiManager(const char *ssid, const char *password);

 public:
	void RegisterClient(wifiNotificarionFunc_t notification);
	void NotifyAll(ConnectionStatus status) const;
	bool IsConnected() const;
	void Loop();
	ConnectionStatus GetStatus() const;
};

typedef std::shared_ptr<WiFiManager> WiFiManagerPtr_t;
#endif

