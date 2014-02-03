#include "CmPrerequisites.h"
#include "CmGameObject.h"
#include "CmGameObjectHandle.h"
#include "CmException.h"
#include "CmGameObjectHandleRTTI.h"

namespace CamelotFramework
{
	GameObjectHandleBase::GameObjectHandleBase(const std::shared_ptr<GameObjectHandleData>& data)
		:mData(data)
	{ }

	GameObjectHandleBase::GameObjectHandleBase(const std::shared_ptr<GameObject> ptr)
	{
		mData = cm_shared_ptr<GameObjectHandleData, PoolAlloc>(ptr->mInstanceData);
	}

	GameObjectHandleBase::GameObjectHandleBase(std::nullptr_t ptr)
	{
		mData->mPtr = nullptr;
	}

	GameObjectHandleBase::GameObjectHandleBase()
	{ }

	void GameObjectHandleBase::throwIfDestroyed() const
	{
		if(isDestroyed()) 
		{
			CM_EXCEPT(InternalErrorException, "Trying to access an object that has been destroyed.");
		}
	}

	RTTITypeBase* GameObjectHandleBase::getRTTIStatic()
	{
		return GameObjectHandleRTTI::instance();
	}

	RTTITypeBase* GameObjectHandleBase::getRTTI() const
	{
		return ResourceHandleBase::getRTTIStatic();
	}
}