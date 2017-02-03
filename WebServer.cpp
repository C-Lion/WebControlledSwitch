#include "WebServer.h"
#include "Util.h"
#include "WebSettings.h"
#include <map>
#include <algorithm>


using namespace std;
/*static*/ char WebServer::_setupHtmlBuffer[3072]; //for setup html result
WebServer::WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<bool()> relayStateUpdater) :
	_deviceSettings(move(deviceSettings)),
	_server(port), 
	_authorizedUrl(String("/") + appKey),
	_relayStateUpdater(relayStateUpdater)
{
	_server.on("/", [this]() { HandleError(); });
	_server.on((_authorizedUrl + "/setup").c_str(), [this]() { HandleSetup(); });
	_server.on((_authorizedUrl + "/setconfiguration").c_str(),HTTP_POST, [this]() { HandleSetConfiguration(); });
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
	html += String(R"(<br/><p><a href=")") + CreateUrl("resetaccesspoint") + String(R"(">Factory Reset!</a></p>)");
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
	//Serial.printf(html.c_str());
	std::map<String, String> templateValuesMap =
	{
		{"AccessPointList" , html},
		{"WiFiPassword", _deviceSettings->accessPointPassword},
		{"DeviceId", _deviceSettings->AzureIoTDeviceId},
		{"IoTHubConnectionString", _deviceSettings->azureIoTHubConnectionString},
		{"PBLongPress", String(_deviceSettings->longButtonPeriod).c_str()},
		{"PBVeryLongPress", String(_deviceSettings->veryLongButtonPeriod).c_str()},
		{"PBPulseActivationPeriod", String(_deviceSettings->PulseActivationPeriod).c_str()}
	};

	const String checked = R"(checked="checked")";
	if (_deviceSettings->shouldUseAzureIoT)
	{
		templateValuesMap.insert(pair<String, String>{ "AzureIoTHub", checked});
		templateValuesMap.insert(pair<String, String>{ "WebServer", ""});
	}
	else
	{
		templateValuesMap.insert(pair<String, String>{ "WebServer", checked});
		templateValuesMap.insert(pair<String, String>{ "AzureIoTHub", ""});
	}

	if (_deviceSettings->PBBehavior == PushButtonBehaviour::Toggle)
	{
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourToggle", checked});
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourPulse", ""});
	}
	else
	{
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourPulse", checked});
		templateValuesMap.insert(pair<String, String>{ "PBBehaviourToggle", ""});
	}

	PopulateHTMLSetupFromTemplate(WebSettingHtmlTemplate, templateValuesMap);
	_server.send(200, "text/html", _setupHtmlBuffer);
}

void WebServer::PopulateHTMLSetupFromTemplate(const String& htmlTemplate, const std::map<String, String>& map) const
{
	int index = 0;
	int bufferIndex = 0;
	int end = -1;
	do
	{
		int beginVariable = htmlTemplate.indexOf('%', index); //search <%= by searching %
		int endVariable = -1;
		if (beginVariable >= 0) //only if beginVariable didn't reach the end of html
			endVariable = htmlTemplate.indexOf('%', beginVariable + 1);

		if (beginVariable < 0 || endVariable < 0) //no more variables
		{
			auto rest = htmlTemplate.substring(index); //add the template end
			memcpy(_setupHtmlBuffer + bufferIndex, rest.c_str(), rest.length() + 1); //copy the template tail
			bufferIndex += rest.length() + 1;
			break;
		}

		if (htmlTemplate[beginVariable - 1] != '<' || htmlTemplate[beginVariable + 1] != '=' || htmlTemplate[endVariable + 1] != '>') //not <%= ... %>
		{
			_setupHtmlBuffer[bufferIndex++] = htmlTemplate[index];
			++index;
			continue;
		}
		auto variableName = htmlTemplate.substring(beginVariable + 2, endVariable);
		String replacedValue = map.at(variableName); //extract only the variable name and replace it
		String htmlUntilVariable = htmlTemplate.substring(index, beginVariable - 1);

		//Add all text before the variable and the replacement
		memcpy(_setupHtmlBuffer + bufferIndex, htmlUntilVariable.c_str(), htmlUntilVariable.length());
		bufferIndex += htmlUntilVariable.length();
		memcpy(_setupHtmlBuffer + bufferIndex, replacedValue.c_str(), replacedValue.length());
		bufferIndex += replacedValue.length();
		_setupHtmlBuffer[bufferIndex] = 0;
		index = endVariable + 2;
	} while (index != end);
}

void WebServer::HandleSetConfiguration()
{
	_deviceSettings->ssidName = _server.arg("ap").c_str();
	_deviceSettings->accessPointPassword = _server.arg("WiFiPassword").c_str();
	_deviceSettings->AzureIoTDeviceId = _server.arg("DeviceId").c_str();
	_deviceSettings->azureIoTHubConnectionString = _server.arg("IoTHubConnectionString").c_str();
	_deviceSettings->longButtonPeriod = atoi(_server.arg("PBLongPress").c_str());
	_deviceSettings->veryLongButtonPeriod = atoi(_server.arg("PBVeryLongPress").c_str());
	_deviceSettings->PulseActivationPeriod = atoi(_server.arg("PBPulseActivationPeriod").c_str());
	_deviceSettings->shouldUseAzureIoT = _server.arg("WebServerOrAzureIoTHub") == "AzureIoTHub";
	_deviceSettings->PBBehavior = _server.arg("PBBehaviour") == "PBBehaviourToggle" ? PushButtonBehaviour::Toggle : PushButtonBehaviour::Pulse; 
	printf("Server arguments:\n");
	for (int i = 0; i < _server.args(); ++i)
	{
		Serial.printf("%s=%s\n", _server.argName(i).c_str(), _server.arg(_server.argName(i)).c_str());
	}

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
