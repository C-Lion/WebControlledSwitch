#include "WebServer.h"
#include "Util.h"
using namespace std;

WebServer::WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<bool()> relayStateUpdater) :
	_deviceSettings(move(deviceSettings)),
	_server(port), 
	_authorizedUrl(string("/") + appKey),
	_relayStateUpdater(relayStateUpdater)
{
	_server.on("/", [this]() { HandleError(); });
	_server.on((_authorizedUrl + "/setup").c_str(), [this]() { HandleSetup(); });
	_server.on((_authorizedUrl + "/setaccesspoint").c_str(),HTTP_POST, [this]() { HandleSetAccessPoint(); });
	_server.on((_authorizedUrl + "/resetaccesspoint").c_str(), [this]() { HandleResetAccessPoint(); });
	_server.on(_authorizedUrl.c_str(), [this]() { HandleMain(); });
	_server.on((_authorizedUrl + "/").c_str(), [this]() { HandleMain(); });
	_server.onNotFound([this]() { HandleError(); });

	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
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
	html += string(R"(<br/><br/><p><a href=")") + CreateUrl("resetaccesspoint") + string(R"(">Reset stored access point name and password</a></p>)");
	SendBackHtml(html);
}

void WebServer::HandleSetup()
{
	String html =
		R"(<p><center><h3>Set access point credentials:</h3></center><p>)
		<p><h4>Access Points in range:</h4>
		<form id="set-access-point" action="setaccesspoint" method="post">
			<ul>)";
	auto accessPointList = ConnectionStatus::GetAccessPoints();
	for (auto &&ap : accessPointList)
	{
		html += R"(<li><label for = ")";
		html += ap.SSID.c_str();
		html += R"(">)";
		html += ap.SSID.c_str();
		html += "   Signal:";
		html += String(ap.RSSI);
		if (ap.isEncripted)
			html += " *";
		html += R"("</label> <input type = "radio" name = "ap" id = ")";
		html += ap.SSID.c_str();
		html += R"(" value = ")";
		html += ap.SSID.c_str();
		html += R"("></li>)";
	}
	html += R"(</ul>)";
	html += R"(<fieldset>AccessPoint Password:<br/>)";
	html += R"(<input type="password" name="password"><br/>)";
	html += R"(<input type="submit" value="Set Password"></fieldset></form></p>)";
	SendBackHtml(html.c_str());
}

void WebServer::HandleSetAccessPoint()
{
	_deviceSettings->ssidName = _server.arg("ap").c_str();
	_deviceSettings->accessPointPassword = _server.arg("password").c_str();
	string html =
		R"(<p><center><h3>The device will reboot and try to connect to:</h3></center></p>)";
		html += R"(<p>Access Point Name:)";
		html += _deviceSettings->ssidName;
		html += "</p><br/>";
		html += "If after the reboot the two Leds are blinking or the green led is not turned on, do a factory reset by pressing the button for more than ";
		html += String(_deviceSettings->veryLongButtonPeriod / 1000).c_str();
		html += " seconds. The two leds should blinks very fast.";

		SendBackHtml(html.c_str());
		_configurationUpdater(*_deviceSettings.get());
		Util::software_Reboot();
}

void WebServer::HandleResetAccessPoint()
{
	_deviceSettings->isFactoryReset = true;
	String html =
		R"(<p><h3>Access point credentials has been reset.</h3></p><br/><p>Reset device to activate access point mode.</p><br/>)";
		html += R"(<p>Set new access point SSID information by surfing to )";
		html += String(R"(http://192.168.4.1/)") + CreateUrl("setup").c_str();
		html += "</p>";

	SendBackHtml(html.c_str());
	_configurationUpdater(*_deviceSettings.get()); //this will reset the device
}
void WebServer::HandleCommand(WebCommandPtr_t webCommand)
{
	auto html = string("<p><h3>") + webCommand->ResultHTML() +string("</h3></p>");
	_pubsub.NotifyAll(webCommand->Name(), webCommand->Id());
	SendBackHtml(html);
}

bool WebServer::IsConnected() const
{
	return WiFi.status() == WL_CONNECTED;
}

void WebServer::Loop()
{
	if (_relayStateUpdater)
		_relayState = _relayStateUpdater();

	_server.handleClient();
}

void WebServer::SetUpdateConfiguration(std::function<void(const DeviceSettings&)> configurationUpdater)
{
	_configurationUpdater = configurationUpdater;
}

void WebServer::UpdateStatus(ConnectionStatus status)
{
	if (!_isInit && status.IsJustConnected()) //new connection, only once
	{
		Serial.println("Web Server begins...");
		_isInit = true;
		MDNS.begin("esp8266");
		_server.begin();
	}
}

std::string WebServer::CreateUrl(const std::string& s) const
{
	return _authorizedUrl + "/" + s;
}
