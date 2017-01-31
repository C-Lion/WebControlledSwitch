// ConfigurationManager.h

#ifndef _CONFIGURATIONMANAGER_h
#define _CONFIGURATIONMANAGER_h
#include <string>
#include <algorithm>
#include "Singleton.h"
#include "WebServer.h"

enum class RelayMode
{
	OnOFF = 0,
	Pulse = 1
};

class ConfigurationManager : public Singleton<ConfigurationManager>
{
	friend class Singleton<ConfigurationManager>;
private:
	struct
	{
		unsigned char _magicNumber[6];
		char SSIDName[32];
		char AccessPointPassword[64];
		char AzureIoTHubConnectionString[256];
		char IoTHubDeviceId[64];
		unsigned int milliSecondsButonLongTimePeriod;
		unsigned int milliSecondsButonVeryLongTimePeriod;
		bool bUseAzureIoTHub;
		bool bPulseRelay;

	} _eepromInformationBlock;

	const unsigned char _magicNumber[6] = { 'M', 'A', 'G', 'I', 'C', 0 };
	bool CheckMagicNumber() const;
	void WriteMagicNumber();
	void ClearMagicNumber();
	void ReadEEPROMInfo();
	void FlashEEPROMInfo();

	ConfigurationManager();

 public:
	std::string GetSSID() const;
	std::string GetAccessPointPassword() const;
	void SetWiFiCredentials(const std::string& SSID, const std::string& password);
	void SetAzureIoTHubInformation(const std::string &azureIoTConnectionString, const std::string &iotHubDeviceId);
	void SetWebServerMode(); //don't use Azure IoT
	std::string GetAzureIoTConnectionString();
	std::string GetIoTHubDeviceId() const;
	void SetButonPressTimesMilliSeconds(unsigned int longPeriod, unsigned int veryLongPeriod);
	unsigned int GetLongPeriodButonPressTimesMilliSeconds() const { return _eepromInformationBlock.milliSecondsButonLongTimePeriod; }
	bool ShouldUseAzureIoTHub() { return _eepromInformationBlock.bUseAzureIoTHub; }
	unsigned int GetVeryLongPeriodButonPressTimesMilliSeconds() const { return _eepromInformationBlock.milliSecondsButonVeryLongTimePeriod; }
	void SetRelayMode(RelayMode mode) { _eepromInformationBlock.bPulseRelay = mode == RelayMode::Pulse; }
	RelayMode GetRelayMode() const { return _eepromInformationBlock.bPulseRelay ? RelayMode::Pulse : RelayMode::OnOFF; }
	void FacrotyReset();
	void FlashEEProm();
	bool IsAccessPointMode() const { return !CheckMagicNumber(); }
};

typedef std::shared_ptr<ConfigurationManager> ConfigurationManager_t;
#endif //_CONFIGURATIONMANAGER_h

