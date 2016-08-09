// TogglePushButtonManager.h

#ifndef _TOGGLEPUSHBUTTONMANAGER_h
#define _TOGGLEPUSHBUTTONMANAGER_h

#include "PushButtonManager.h"
#include "Singleton.h"

class TogglePushButtonManager : public PushButtonManager, public Singleton<TogglePushButtonManager>
{
private:
	friend class Singleton<TogglePushButtonManager>;
	using PushButtonManager::PushButtonManager;
	virtual int StateOnTrigger()  const override;

};

#endif

