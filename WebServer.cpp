#include "WebServer.h"
#include "Util.h"
#include "WebSettings.h"
#include <map>
#include <algorithm>


using namespace std;

WebServer::WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<bool()> relayStateUpdater) :
	_deviceSettings(move(deviceSettings)),
	_server(port), 
	_authorizedUrl(String("/") + appKey),
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
	auto url = String(CreateUrl(command->TriggerUrl()));
	_server.on(url.c_str(), [=]() {  HandleCommand(command); });
}

void WebServer::SendBackHtml(const String &message)
{
	auto html = String("<html><body><h2>") + _header + "</h2>"
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
	auto html = String("<p><h3>The current switch status is ") +
	(_relayState ? "on" : "off") + "</h3></p>";
	for (auto webCommand : _webCommands)
	{
		html += String(R"(<p><a href=")") + CreateUrl(webCommand->TriggerUrl()) + String(R"(">)") + webCommand->MenuEntry() + String("</a></p>");
	}
	html += String(R"(<br/><br/><p><a href=")") + CreateUrl("resetaccesspoint") + String(R"(">Reset stored access point name and password</a></p>)");
	SendBackHtml(html);
}


void WebServer::HandleSetup()
{
	String html;
	auto accessPointList = ConnectionStatus::GetAccessPoints();
	for (auto &&ap : accessPointList)
	{
		html += R"(<li><label>)";
		html += ap.SSID.c_str();
		html += "  Signal:";
		html += String(ap.RSSI).c_str();
		if (ap.isEncripted)
			html += " *";
		html += R"("</label> <input type = "radio" name = "ap" value = ")";
		html += ap.SSID.c_str();
		html += R"(" )";
		if (ap.SSID == _deviceSettings->ssidName)
			html += R"( checked="checked" )";
		html += R"("></li>)";
	}
	
	std::map<String, String> templateValuesMap =
	{
		{"AccessPointList" , html},
		{"WiFiPassword", _deviceSettings->accessPointPassword},
		{"DeviceId", _deviceSettings->AzureIoTDeviceId},
		{"PBLongPress", String(_deviceSettings->longButtonPeriod).c_str()},
		{"PBVeryLongPress", String(_deviceSettings->veryLongButtonPeriod).c_str()},
		{"PBPulseActivationPeriod", String(_deviceSettings->PulseActivationPeriod).c_str()}
	};

	const String checked = R"(checked="checked")";
	if (_deviceSettings->shouldUseAzureIoT)
	{
		templateValuesMap.insert(pair<String, String>{ "AzureIoTHub", checked});
	}
	else
	{
		templateValuesMap.insert(pair<String, String>{ "WebServer", checked});
	}

	if (_deviceSettings->PBBehavior == PushButtonBehaviour::Toggle)
	{
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourToggle", checked});
	}
	else
	{
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourPulse", checked});
	}

	html = Util::CreateHTMLFromTemplate(WebSettingHtmlTemplate, templateValuesMap);
	SendBackHtml(html);
}

void WebServer::HandleSetAccessPoint()
{
	_deviceSettings->ssidName = _server.arg("ap").c_str();
	_deviceSettings->accessPointPassword = _server.arg("password").c_str();
	String html =
		R"(<p><center><h3>The device will reboot and try to connect to:</h3></center></p><p>)";
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
	auto html = String("<p><h3>") + webCommand->ResultHTML() +String("</h3></p>");
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

String WebServer::CreateUrl(const String& s) const
{
	return _authorizedUrl + "/" + s;
}
