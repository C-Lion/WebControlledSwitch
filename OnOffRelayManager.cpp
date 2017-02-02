// 
// 
// 

#include "OnOffRelayManager.h"

void OnOffRelayManager::OnCommand(const String &commandName, int commandId)
{
	if (commandName == "On")
		Set(HIGH);
	else if (commandName == "Off")
		Set(LOW);
}
