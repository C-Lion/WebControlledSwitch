// 
// 
// 

#include "PasswordManager.h"
#include <EEPROM.h>

PasswordManager::PasswordManager()
{
	EEPROM.begin(512);
	delay(10);
}

bool PasswordManager::CheckMagicNumber() const
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		if (EEPROM.read(i) != _magicNumber[i])
			return false;
	return true;
}

void PasswordManager::WriteMagicNumber()
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		EEPROM.write(i, _magicNumber[i]);
}

void PasswordManager::ClearMagicNumber() //used to reset password
{
	for (int i = 0; i < sizeof(_magicNumber); ++i)
		EEPROM.write(i, 0);
}

std::string PasswordManager::GetStringFromEEprom(int location, size_t maxSize) const
{
	if (!CheckMagicNumber())
		return std::string();
	char ssid[maxSize];
	for (int i = 0; i < maxSize - 1; ++i)
		ssid[i] = EEPROM.read(location + i);
	ssid[maxSize - 1] = 0;
	return std::string(ssid);
}

void PasswordManager::WriteStringToEEprom(int location, size_t maxSize, const std::string& text) 
{
	auto length = min(maxSize - 1, text.length());
	for (int i = 0; i < length; ++i)
		EEPROM.write(location + i, text[i]);
}

std::string PasswordManager::GetSSID() const
{
	return GetStringFromEEprom(_ssidStartLocation, _ssidMaxLength);
}

std::string PasswordManager::GetWiFiPassword() const
{
	return GetStringFromEEprom(_passwordStartLocation, _passwordMaxLength);
}

void PasswordManager::WriteWiFiCredentials(const std::string& SSID, const std::string& password)
{
	if (SSID == std::string())
	{
		ClearMagicNumber();
	}
	else
	{
		WriteMagicNumber();
		WriteStringToEEprom(_ssidStartLocation, _ssidMaxLength, SSID);
		WriteStringToEEprom(_passwordStartLocation, _passwordMaxLength, password);
	}
	EEPROM.commit();
}
