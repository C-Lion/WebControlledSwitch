
#include "WiFiManager.h"

using namespace std;

std::map<int, WiFiStatus> ConnectionStatus::_statusMap =
{
	{ WL_CONNECTED, WiFiStatus::connected },
	{ WL_NO_SHIELD, WiFiStatus::noShield },
	{ WL_IDLE_STATUS, WiFiStatus::idle },
	{ WL_NO_SSID_AVAIL, WiFiStatus::noSSID },
	{ WL_SCAN_COMPLETED, WiFiStatus::scanCompleted },
	{ WL_CONNECT_FAILED, WiFiStatus::connectFailed },
	{ WL_CONNECTION_LOST, WiFiStatus::connectionLost },
	{ WL_DISCONNECTED, WiFiStatus::dissconnected }
};

std::array<string, 8> ConnectionStatus::_messageMap =
{
	"Connected to a WiFi network",
	"No WiFi shield is present",
	"WiFi.begin(): Trying to connect to a WiFi network",
	"No SSID are available",
	"Scan networks is completed",
	"The connection fails for all the attempts",
	"The connection is lost",
	"Disconnected from a network"
};

WiFiManager::WiFiManager(const char *ssid, const char *password)
{
	WiFi.begin(ssid, password);
	_lastConnectionStatus = WiFi.status();
}

void WiFiManager::RegisterClient(wifiNotificarionFunc_t notification)
{
	_subscribers.push_back(notification);
}

void WiFiManager::NotifyAll(ConnectionStatus status) const
{
	for (auto subscriber : _subscribers)
	{
		subscriber(status);
	}
}

bool WiFiManager::IsConnected() const
{
	return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::Loop()
{
	UpdateStatus();
}

void WiFiManager::UpdateStatus()
{
	auto currentStatus = WiFi.status();
	if (_lastConnectionStatus == currentStatus) //no change
		return;
	
	bool justConnected = _lastConnectionStatus != WL_CONNECTED &&  currentStatus == WL_CONNECTED;
	bool justDissconnected = _lastConnectionStatus == WL_CONNECTED && currentStatus != WL_CONNECTED;
	
	_lastConnectionStatus = currentStatus;
	NotifyAll(ConnectionStatus(WiFi.status(), WiFi.localIP(), justConnected, justDissconnected));
}

ConnectionStatus WiFiManager::GetStatus() const
{
	return ConnectionStatus(WiFi.status(), WiFi.localIP());
}
