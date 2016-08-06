// TogglePushButtonManager.h

#ifndef _TOGGLEPUSHBUTTONMANAGER_h
#define _TOGGLEPUSHBUTTONMANAGER_h

#include "PushButtonManager.h"


class TogglePushButtonManager : public PushButtonManager
{
private:
	virtual int StateOnTrigger()  const override;

public:
	using PushButtonManager::PushButtonManager;
};

#endif

