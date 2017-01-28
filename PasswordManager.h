// PasswordManager.h

#ifndef _PASSWORDMANAGER_h
#define _PASSWORDMANAGER_h
#include <string>
#include "Singleton.h"


class PasswordManager : public Singleton<PasswordManager>
{
	friend class Singleton<PasswordManager>;
private:
	const unsigned char _magicNumber[6] = { 'M', 'A', 'G', 'I', 'C', 0 };
	const unsigned char _ssidMaxLength = 33;
	const unsigned int _ssidStartLocation = sizeof(_magicNumber);
	const unsigned char _passwordMaxLength = 65;
	const unsigned int _passwordStartLocation = _ssidStartLocation + _ssidMaxLength;

	bool CheckMagicNumber() const;
	void WriteMagicNumber();
	void ClearMagicNumber();

	std::string GetStringFromEEprom(int location, size_t maxSize) const;
	void WriteStringToEEprom(int location, size_t maxSize, const std::string& text);

	PasswordManager();

 public:
	 std::string GetSSID() const;
	 std::string GetWiFiPassword() const;
	 void WriteWiFiCredentials(const std::string& SSID, const std::string &password);
};

typedef std::shared_ptr<PasswordManager> PasswordManager_t;
#endif

