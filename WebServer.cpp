#include "WebServer.h"

using namespace std;

WebServer::WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey) :
	_server(port), 
	_authorizedUrl(string("/") + appKey)
{
	_server.on("/", [this]() { HandleError(); });
	_server.on(_authorizedUrl.c_str(), [this]() { HandleMain(); });
	_server.on((_authorizedUrl + "/").c_str(), [this]() { HandleMain(); });
	_server.onNotFound([this]() { HandleError(); });

	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
	
	_lastConnectionStatus = wifiManager->GetStatus().WifiCode();
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
	_server.handleClient();
}

void WebServer::UpdateStatus(ConnectionStatus status)
{
	bool hasNotified = false;

	//Connection lost, notify once
	if (_lastConnectionStatus == WL_CONNECTED && status.WifiCode() != WL_CONNECTED)
	{
		Notify([=](WebNotificationPtr_t subscriber)
		{
			subscriber->OnDisconnected(status);
		});
		hasNotified = true;
	}

	if (_lastConnectionStatus != WL_CONNECTED &&  status.WifiCode() == WL_CONNECTED) //new connection, notify once
	{
		MDNS.begin("esp8266");
		_server.begin();

		Notify([=](WebNotificationPtr_t subscriber)
		{
			subscriber->OnConnected(status);
		});
		hasNotified = true;
	}
	_lastConnectionStatus = status.WifiCode();

	if (hasNotified)
		return;
	//else error
	Notify([=](WebNotificationPtr_t subscriber)
	{
		subscriber->OnError(status);
	});
}

std::string WebServer::CreateUrl(const std::string& s) const
{
	return _authorizedUrl + "/" + s;
}
