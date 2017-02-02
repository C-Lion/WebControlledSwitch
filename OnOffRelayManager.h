// RelayManager.h

#ifndef _ONOFFRELAYMANAGER_h
#define _ONOFFRELAYMANAGER_h
#include "RelayManager.h"
#include "Singleton.h"

class OnOffRelayManager : public RelayManager, public Singleton<OnOffRelayManager>
{
	friend class Singleton<OnOffRelayManager>;
private:
	using RelayManager::RelayManager;

	void OnCommand(const String & commandName, int commandId) override;

};

#endif

