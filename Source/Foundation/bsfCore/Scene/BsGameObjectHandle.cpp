//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsCorePrerequisites.h"
#include "Scene/BsGameObject.h"
#include "Scene/BsGameObjectHandle.h"
#include "Error/BsException.h"
#include "Scene/BsGameObject.h"
#include "Private/RTTI/BsGameObjectHandleRTTI.h"

namespace bs
{
	GameObjectHandleBase::GameObjectHandleBase(SPtr<GameObjectHandleData> data)
		:mData(std::move(data))
	{ }

	GameObjectHandleBase::GameObjectHandleBase(const SPtr<GameObject>& ptr)
	{
		mData = bs_shared_ptr_new<GameObjectHandleData>(ptr->mInstanceData);
	}

	GameObjectHandleBase::GameObjectHandleBase(std::nullptr_t ptr)
	{
		mData = bs_shared_ptr_new<GameObjectHandleData>(nullptr);
	}

	GameObjectHandleBase::GameObjectHandleBase()
	{
		mData = bs_shared_ptr_new<GameObjectHandleData>(nullptr);
	}

	bool GameObjectHandleBase::isDestroyed(bool checkQueued) const
	{
		return mData->mPtr == nullptr || mData->mPtr->object == nullptr
			|| (checkQueued && mData->mPtr->object->_getIsDestroyed());
	}

	void GameObjectHandleBase::_resolve(const GameObjectHandleBase& object) 
	{ 
		mData->mPtr = object.mData->mPtr;
	}

	void GameObjectHandleBase::_setHandleData(const SPtr<GameObject>& object)
	{
		mData->mPtr = object->mInstanceData;
	}

	void GameObjectHandleBase::throwIfDestroyed() const
	{
		if(isDestroyed()) 
		{
			BS_EXCEPT(InternalErrorException, "Trying to access an object that has been destroyed.");
		}
	}

	RTTITypeBase* GameObjectHandleBase::getRTTIStatic()
	{
		return GameObjectHandleRTTI::instance();
	}

	RTTITypeBase* GameObjectHandleBase::getRTTI() const
	{
		return GameObjectHandleBase::getRTTIStatic();
	}
}