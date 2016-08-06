// PushButtonManager.h

#ifndef _MEMENTARYPUSHBUTTONMANAGER_h
#define _MEMENTARYPUSHBUTTONMANAGER_h

#include "PushButtonManager.h"

class MementaryPushButtonManager : public PushButtonManager
{
private:
	virtual int StateOnTrigger()  const override;
public:
	using PushButtonManager::PushButtonManager;
};

#endif

