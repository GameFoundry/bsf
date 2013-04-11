#include "CmGameObject.h"
#include "CmGameObjectRTTI.h"

namespace CamelotEngine
{
	RTTITypeBase* GameObject::getRTTIStatic()
	{
		return GameObjectRTTI::instance();
	}

	RTTITypeBase* GameObject::getRTTI() const
	{
		return GameObject::getRTTIStatic();
	}
}