#include "CmGameObject.h"
#include "CmGameObjectRTTI.h"
#include "CmGameObjectManager.h"

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