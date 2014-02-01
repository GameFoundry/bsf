#include "CmPrerequisites.h"
#include "CmGameObject.h"
#include "CmGameObjectHandle.h"
#include "CmGameObjectManager.h"
#include "CmException.h"

namespace CamelotFramework
{
	GameObjectHandleBase::GameObjectHandleBase(std::shared_ptr<GameObjectHandleData> data)
		:mData(data)
	{ }

	GameObjectHandleBase::GameObjectHandleBase()
	{ }

	void GameObjectHandleBase::throwIfDestroyed() const
	{
		if(isDestroyed()) 
		{
			CM_EXCEPT(InternalErrorException, "Trying to access an object that has been destroyed.");
		}
	}

	void GameObjectHandleBase::registerWithManager(const GameObjectHandleBase& object)
	{
		object.get()->mInstanceId = GameObjectManager::instance().registerObject(object);
	}

	void GameObjectHandleBase::unregisterWithManager(const GameObjectHandleBase& object)
	{
		GameObjectManager::instance().unregisterObject(object);
	}
}