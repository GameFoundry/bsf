#include "CmGameObject.h"
#include "CmGameObjectRTTI.h"

namespace CamelotFramework
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