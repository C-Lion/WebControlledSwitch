
#include "WiFiManager.h"

using namespace std;

std::map<int, WiFiStatus>  ConnectionStatus::_statusMap =
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

std::array<String, 8>  ConnectionStatus::_messageMap =
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
std::list<AccessPointInfo>  ConnectionStatus::_accessPointList;

WiFiManager::WiFiManager(const String &ssid, const String &password, bool isAccessPointMode)
{
	if (isAccessPointMode)
	{
		WiFi.mode(WIFI_AP);
		WiFi.disconnect();
		delay(100);

		int n = WiFi.scanNetworks();
		Serial.println("scan done");
		if (n == 0)
			Serial.println("no networks found");
		else
		{
			Serial.print(n);
			Serial.println(" networks found");
			for (int i = 0; i < n; ++i)
			{
				ConnectionStatus::AddAccessPointInfo(AccessPointInfo{ String(WiFi.SSID(i).c_str()), WiFi.RSSI(i) ,WiFi.encryptionType(i) == ENC_TYPE_NONE });
			}
		}
		Serial.println("Setting access point mode to ap name: ");
		Serial.print(ssid.c_str());
		Serial.print("  ap password: ");
		Serial.println(password.c_str());

		WiFi.softAP(ssid.c_str(), password.c_str(), 6);
		Serial.println("softap up and running");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		_accessPointMode = true;
	}
	else
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid.c_str(), password.c_str());
		_lastConnectionStatus = WiFi.status();
	}
}

void  WiFiManager::RegisterClient(wifiNotificarionFunc_t notification)
{
	_subscribers.push_back(notification);
}

void  WiFiManager::NotifyAll(ConnectionStatus status) const
{
	for (auto subscriber : _subscribers)
	{
		subscriber(status);
	}
}

bool WiFiManager::IsConnected() const
{
	return _accessPointMode || WiFi.status() == WL_CONNECTED;
}

void WiFiManager::Loop()
{
	UpdateStatus();
}

void WiFiManager::UpdateStatus()
{
	if (_accessPointMode && !_accessPointModeHasBeenInit) //first time init access point mode
	{
		_accessPointModeHasBeenInit = true;
		NotifyAll(ConnectionStatus(WiFi.status(), WiFi.localIP(), true, false, true));
		return;
	}

	auto currentStatus = WiFi.status();
	if (_lastConnectionStatus == currentStatus) //no change, update every 20 seconds
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
