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


WebServer::WebServer(int port, const char *ssid, const char *password, const char *appKey) : 
	_server(port), 
	_authorizedUrl(string("/") + appKey)
{
	_server.on("/", [this]() { HandleError(); });
	_server.on(_authorizedUrl.c_str(), [this]() { HandleMain(); });
	_server.on((_authorizedUrl + "/").c_str(), [this]() { HandleMain(); });
	_server.onNotFound([this]() { HandleError(); });

	WiFi.begin(ssid, password);
	_lastConnectionStatus = WiFi.status();
}

void WebServer::RegisterCommand(WebCommandPtr_t command)
{
	_webCommands.push_back(command);
	auto url = string(CreateUrl(command->TriggerUrl()));
	_server.on(url.c_str(), [=]() {  HandleCommand(command); });
}

void WebServer::SendBackHtml(const string &message)
{
	auto html = string("<html><body><h2>") + _header + "</h2>"
		+ message + "</body></html>";
	_server.send(200,
		"text/html",
		html.c_str());
}
void WebServer::HandleError()
{
	_server.send(401,
		"text/plain",
		"Unauthorized");
}

void WebServer::HandleMain() 
{
	auto html = string("<p><h3>The current switch status is ") +
	(_relayState ? "on" : "off") + "</h3></p>";
	for (auto webCommand : _webCommands)
	{
		html += string(R"(<p><a href=")") + CreateUrl(webCommand->TriggerUrl()) + string(R"(">)") + webCommand->MenuEntry() + string("</a></p>");
	}
	SendBackHtml(html);
}

void WebServer::HandleCommand(WebCommandPtr_t webCommand)
{
	auto html = string("<p><h3>") + webCommand->ResultHTML() +string("</h3></p>");
	Notify([=](WebNotificationPtr_t subscriber) {subscriber->OnCommand(webCommand->Name(), webCommand->Id()); });
	SendBackHtml(html);
}

void WebServer::Notify(function<void(WebNotificationPtr_t)> callBack)
{
	for (auto subscriber : _subscribers)
	{
		callBack(subscriber);
	}
}

void WebServer::Register(WebNotificationPtr_t subscriber)
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
		Notify([=](WebNotificationPtr_t subscriber)
		{
			subscriber->OnDisconnected(ConnectionStatus(currentStatus));
		});
		hasNotified = true;
	}

	if (_lastConnectionStatus != WL_CONNECTED && currentStatus == WL_CONNECTED) //new connection, notify once
	{
		MDNS.begin("esp8266");
		_server.begin();

		Notify([=](WebNotificationPtr_t subscriber)
		{
			subscriber->OnConnected(ConnectionStatus(currentStatus), WiFi.localIP());
		});
		hasNotified = true;
	}
	_lastConnectionStatus = currentStatus;

	if (hasNotified)
		return;
	//else error
	Notify([=](WebNotificationPtr_t subscriber)
	{
		subscriber->OnError(ConnectionStatus(currentStatus));
	});
}

std::string WebServer::CreateUrl(const std::string& s) const
{
	return _authorizedUrl + "/" + s;
}
