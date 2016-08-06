// RelayManager.h

#ifndef _ONOFFRELAYMANAGER_h
#define _ONOFFRELAYMANAGER_h
#include "RelayManager.h"

class OnOffRelayManager : public RelayManager
{
private:
	using RelayManager::RelayManager;

	void OnCommand(const std::string & commandName, int commandId) override;

};

#endif

