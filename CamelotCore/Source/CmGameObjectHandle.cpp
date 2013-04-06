#include "CmPrerequisites.h"
#include "CmGameObjectHandle.h"
#include "CmException.h"

namespace CamelotEngine
{
	GameObjectHandleBase::GameObjectHandleBase()
		:mData(new GameObjectHandleData())
	{ }

	GameObjectHandleBase::GameObjectHandleBase(const std::shared_ptr<GameObjectHandleData>& data)
		:mData(data)
	{ }

	void GameObjectHandleBase::throwIfDestroyed() const
	{
		if(isDestroyed()) 
		{
			CM_EXCEPT(InternalErrorException, "Trying to access an object that has been destroyed.");
		}
	}
}