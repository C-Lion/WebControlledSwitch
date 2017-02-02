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

class IWebCommand
{
public:
	virtual const String& Name() const = 0;
	virtual int Id() const = 0;
	virtual const String& MenuEntry() const = 0; 
	virtual const String& ResultHTML() const = 0;
	virtual const String& TriggerUrl() const = 0;

	virtual ~IWebCommand() {}
};

enum class PushButtonBehaviour
{
	Toggle,
	Pulse
};

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
	unsigned int PulseActivationPeriod;
	PushButtonBehaviour PBBehavior;
};

typedef std::shared_ptr<IWebCommand> WebCommandPtr_t;
typedef std::function<void(const String&, int)> WebNotificationPtr_t;
class WebServer : public Singleton<WebServer>, public IProcessor
{
	friend class Singleton<WebServer>;
private:
	std::unique_ptr<DeviceSettings> _deviceSettings;
	ESP8266WebServer _server;
	PubSub<WebServer, const String&, int> _pubsub;
	bool _relayState = false;
	String _header;
	const String _authorizedUrl;
	std::vector<WebCommandPtr_t> _webCommands;
	bool _isInit = false;
	std::function<bool()> _relayStateUpdater;
	std::function<void(const DeviceSettings&)> _configurationUpdater;
	void SendBackHtml(const String &message);
	void UpdateStatus(ConnectionStatus status);
	String CreateUrl(const String &s) const;
	WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<bool()> relayStateUpdater);

 public:
	
	void RegisterCommand(WebCommandPtr_t command);

	template<typename T>
	void SetWebSiteHeader(T header) { _header = std::forward<T>(header); }
	void HandleMain();
	void HandleSetup();
	void HandleSetAccessPoint();
	void HandleResetAccessPoint();
	void HandleError();
	void HandleCommand(WebCommandPtr_t webCommand);
	void Register(WebNotificationPtr_t subscriber) { _pubsub.Register(subscriber); }
	bool IsConnected() const;
	void Loop();
	void SetUpdateConfiguration(std::function<void(const DeviceSettings&)> configurationUpdater);
};

typedef std::shared_ptr<WebServer> WebServerPtr_t;
#endif

