// 
// 
// 

#include "TogglePushButtonManager.h"

int TogglePushButtonManager::StateOnTrigger() const
{
	return State() == LOW ? HIGH : LOW; //invert state
}