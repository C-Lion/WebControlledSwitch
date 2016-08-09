#include "TogglePushButtonManager.h"
using namespace std;

int TogglePushButtonManager::StateOnTrigger() const
{
	return State() == LOW ? HIGH : LOW; //invert state
}