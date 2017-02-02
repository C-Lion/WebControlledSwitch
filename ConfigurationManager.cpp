#include "ConfigurationManager.h"
#include <EEPROM.h>
#include "Util.h"
#include "Configuration.h"

ConfigurationManager::ConfigurationManager()
{
	EEPROM.begin(512);
	delay(10);
	if (CheckMagicNumber())
	{
		ReadEEPROMInfo();
	}
	else //factory reset
	{
		Util::String2Array(SSID, _eepromInformationBlock.SSIDName);
		Util::String2Array(password, _eepromInformationBlock.AccessPointPassword);
		_eepromInformationBlock.bUseAzureIoTHub = false; //use WebServer mode in Access Point mode
		_eepromInformationBlock.AzureIoTHubConnectionString[0] = 0;
		_eepromInformationBlock.IoTHubDeviceId[0] = 0;
		_eepromInformationBlock.milliSecondsButonLongTimePeriod = defaultButtonLongTimePeriod;
		_eepromInformationBlock.milliSecondsButonVeryLongTimePeriod= defaultButtonVeryLongTimePeriod;
		_eepromInformationBlock.milliSecondsPulseActivationTimePeriod = defaultPulseActivationTimePeriod;
#ifdef PULSE_COMMAND
		_eepromInformationBlock.bPulseRelay = true;
#else
		_eepromInformationBlock.bPulseRelay = false;
#endif
	}
}

void ConfigurationManager::FacrotyReset()
{
	ClearMagicNumber();
	EEPROM.commit();
	ESP.restart();
}

void ConfigurationManager::FlashEEProm()
{
	FlashEEPROMInfo();
}

bool ConfigurationManager::CheckMagicNumber() const
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		if (EEPROM.read(i) != _magicNumber[i])
			return false;
	return true;
}

void ConfigurationManager::WriteMagicNumber()
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		EEPROM.write(i, _magicNumber[i]);
}

void ConfigurationManager::ClearMagicNumber() //used to reset password
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		EEPROM.write(i, 0);
}

void ConfigurationManager::ReadEEPROMInfo()
{
	for (int i = 0; i < sizeof(_eepromInformationBlock); ++i)
		*(reinterpret_cast<char *>(&_eepromInformationBlock) + i) = EEPROM.read(i);
}

void ConfigurationManager::FlashEEPROMInfo()
{
	WriteMagicNumber();
	for (int i = sizeof(_eepromInformationBlock._magicNumber); i < sizeof(_eepromInformationBlock); ++i)
		EEPROM.write(i, *(reinterpret_cast<char *>(&_eepromInformationBlock) + i));
	EEPROM.commit();
}


std::string ConfigurationManager::GetSSID() const
{
	return std::string(_eepromInformationBlock.SSIDName);
}

std::string ConfigurationManager::GetAccessPointPassword() const
{
	return std::string(_eepromInformationBlock.AccessPointPassword);
}

void ConfigurationManager::SetWiFiCredentials(const std::string& SSID, const std::string& password)
{
	Util::String2Array(SSID, _eepromInformationBlock.SSIDName);
	Util::String2Array(password, _eepromInformationBlock.AccessPointPassword);
}

void ConfigurationManager::SetAzureIoTHubInformation(const std::string& azureIoTConnectionString, const std::string& iotHubDeviceId)
{
	_eepromInformationBlock.bUseAzureIoTHub = true;
	Util::String2Array(azureIoTConnectionString, _eepromInformationBlock.AzureIoTHubConnectionString);
	Util::String2Array(iotHubDeviceId, _eepromInformationBlock.IoTHubDeviceId);
}

void ConfigurationManager::SetWebServerMode()
{
	_eepromInformationBlock.bUseAzureIoTHub = false;
}

std::string ConfigurationManager::GetAzureIoTConnectionString()
{
	return std::string(_eepromInformationBlock.AzureIoTHubConnectionString);
}

std::string ConfigurationManager::GetIoTHubDeviceId() const
{
	return std::string(_eepromInformationBlock.IoTHubDeviceId);
}

void ConfigurationManager::SetButonPressTimesMilliSeconds(unsigned longPeriod, unsigned veryLongPeriod, unsigned int pulseActivationPeriod)
{
	_eepromInformationBlock.milliSecondsButonLongTimePeriod = longPeriod;
	_eepromInformationBlock.milliSecondsButonVeryLongTimePeriod = veryLongPeriod;
	_eepromInformationBlock.milliSecondsPulseActivationTimePeriod = pulseActivationPeriod;
}


