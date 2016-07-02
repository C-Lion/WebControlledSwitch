// 
// 
// 


#include "WebServer.h"

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


WebServer::WebServer(int port, const char *ssid, const char *password) : _server(port)
{
	_server.on("/", [this]() { HandleRoot(); });
	_server.on("/on", [this]() { HandleOn(); });
	_server.on("/off", [this]() { HandleOff(); });
	_server.onNotFound([this]() { HandleNotFound(); });
	//_server.on

	WiFi.begin(ssid, password);
	_lastConnectionStatus = WiFi.status();
}

void WebServer::SendBackHtml(const string &message)
{
	auto html = string("<html><body><h2>") + _header + "</h2>"
		+ message + "</body></html>";
	_server.send(200,
		"text/html",
		html.c_str());
}

void WebServer::HandleRoot() 
{
	auto html = string("<p><h3>The current switch status is ") +
	(_relayState ? "on" : "off") + "</h3></p>";
	SendBackHtml(html);
}

void WebServer::HandleNotFound() 
{
	auto html = string("<p><h3>Command not found</h3></p><p>Use /on or /off</p>");
	SendBackHtml(html);
}

void WebServer::HandleOn() 
{
	auto html = string("<p><h3>Turning On</h3></p>");
	Notify([this](WebCommandPtr_t subscriber) {subscriber->OnTurnOn(); });
	SendBackHtml(html);
}

void WebServer::HandleOff() 
{
	auto html = string("<p><h3>Turning Off</h3></p>");
	Notify([this](WebCommandPtr_t subscriber) {subscriber->OnTurnOff(); });
	SendBackHtml(html);
}

void WebServer::Notify(function<void(WebCommandPtr_t)> callBack)
{
	for (auto subscriber : _subscribers)
	{
		callBack(subscriber);
	}
}

void WebServer::Register(WebCommandPtr_t subscriber)
{
	_subscribers.push_back(subscriber);
}

bool WebServer::IsConnected() const
{
	return WiFi.status() == WL_CONNECTED;
}

void WebServer::Loop(int relayState)
{
	_relayState = relayState;

	if (millis() % 1000 == 0) //update every second
		UpdateStatus();

	_server.handleClient();
}

void WebServer::UpdateStatus()
{
	auto currentStatus = WiFi.status();
	if (_lastConnectionStatus == currentStatus) //no change
		return;

	bool hasNotified = false;

	//Connection lost, notify once
	if (_lastConnectionStatus == WL_CONNECTED && currentStatus != WL_CONNECTED)
	{
		Notify([=](WebCommandPtr_t subscriber)
		{
			subscriber->OnDisconnected(ConnectionStatus(currentStatus));
		});
		hasNotified = true;
	}

	if (_lastConnectionStatus != WL_CONNECTED && currentStatus == WL_CONNECTED) //new connection, notify once
	{
		MDNS.begin("esp8266");
		_server.begin();

		Notify([=](WebCommandPtr_t subscriber)
		{
			subscriber->OnConnected(ConnectionStatus(currentStatus), WiFi.localIP());
		});
		hasNotified = true;
	}
	_lastConnectionStatus = currentStatus;

	if (hasNotified)
		return;
	//else error
	Notify([=](WebCommandPtr_t subscriber)
	{
		subscriber->OnError(ConnectionStatus(currentStatus));
	});
}
