// PushButtonManager.h

#ifndef _MEMENTARYPUSHBUTTONMANAGER_h
#define _MEMENTARYPUSHBUTTONMANAGER_h

#include "PushButtonManager.h"
#include "Singleton.h"

class MementaryPushButtonManager : public PushButtonManager, public Singleton<MementaryPushButtonManager>
{
	friend class Singleton<MementaryPushButtonManager>;

private:
	MementaryPushButtonManager() = default;
	virtual int StateOnTrigger()  const override;
public:
	using PushButtonManager::PushButtonManager;
};

#endif

