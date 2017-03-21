// WebServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#include "Arduino.h"
#include <ESP8266WebServer.h>
#undef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#include <ESP8266mDNS.h>
#include <String>
#include <memory>
#include "WiFiManager.h"
#include "Singleton.h"
#include "PubSub.h"
#include "ArduinoLoopManager.h"
#include "Util.h"


struct DeviceSettings
{
	bool isFactoryReset;
	String ssidName;
	String accessPointPassword;
	bool shouldUseAzureIoT;
	String azureIoTHubConnectionString;
	String AzureIoTDeviceId;
	unsigned int longButtonPeriod;
	unsigned int veryLongButtonPeriod;
};

typedef std::function<void(const String&, int)> WebNotificationPtr_t;
class WebServer : public Singleton<WebServer>, public IProcessor
{
	friend class Singleton<WebServer>;
private:
	char _setupHtmlBuffer[3300]; //for setup html result
	std::unique_ptr<DeviceSettings> _deviceSettings;
	ESP8266WebServer _server;
	PubSub<WebServer, const String&, int> _pubsub;
	bool _waterStatus = false;
	String _header;
	const String _authorizedUrl;
	bool _isInit = false;
	std::function<bool()> _waterStatusUpdater;
	std::function<void(const DeviceSettings&)> _configurationUpdater;
	//setup template processing variables
	int _templateIndex = 0;
	int _templateBufferIndex = 0;
	int _templateEnd = -1;
	Util::StringMap _templateValuesMap;
	bool _isHttpSetupRequestOn = false;

	void SendBackHtml(const String &message);
	void UpdateStatus(ConnectionStatus status);
	String CreateUrl(const String &s) const;
	bool PopulateHTMLSetupFromTemplate(const String& htmlTemplate, const Util::StringMap &map);
	WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<bool()> waterStatusUpdater);

	void HandleMain();
	void ProcessHTTPSetupRequest();
	void HandleSendAPScript();
	void HandleSetup();
	void HandleSendViewCSS();
	void HandleSendAPList();
	void HandleSetConfiguration();
	void HandleResetAccessPoint();
	void HandleError();

 public:
	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	bool IsConnected() const;
	void Loop();
	void SetUpdateConfiguration(std::function<void(const DeviceSettings&)> configurationUpdater);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

