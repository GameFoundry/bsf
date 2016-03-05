//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGameObject.h"
#include "BsGameObjectRTTI.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	GameObject::GameObject()
		:mIsDestroyed(false), mLinkId((UINT32)-1)
	{ }

	GameObject::~GameObject()
	{ }

	void GameObject::initialize(const std::shared_ptr<GameObject>& object, UINT64 instanceId)
	{
		mInstanceData = bs_shared_ptr_new<GameObjectInstanceData>();
		mInstanceData->object = object;
		mInstanceData->mInstanceId = instanceId;
	}

	void GameObject::_setInstanceData(GameObjectInstanceDataPtr& other)
	{
		GameObjectPtr myPtr = mInstanceData->object;
		UINT64 oldId = mInstanceData->mInstanceId;

		mInstanceData = other;
		mInstanceData->object = myPtr;

		GameObjectManager::instance().remapId(oldId, mInstanceData->mInstanceId);
	}
	
	RTTITypeBase* GameObject::getRTTIStatic()
	{
		return GameObjectRTTI::instance();
	}

	RTTITypeBase* GameObject::getRTTI() const
	{
		return GameObject::getRTTIStatic();
	}
}