//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGameObject.h"
#include "BsGameObjectRTTI.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	GameObject::GameObject()
	{ }

	GameObject::~GameObject()
	{ }

	void GameObject::initialize(const std::shared_ptr<GameObject>& object, UINT64 instanceId)
	{
		mInstanceData = bs_shared_ptr<GameObjectInstanceData>();
		mInstanceData->object = object;
		mInstanceData->mInstanceId = instanceId;
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