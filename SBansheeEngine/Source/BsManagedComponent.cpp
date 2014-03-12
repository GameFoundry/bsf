#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent(const CM::HSceneObject& parent)
		:mScriptComponent(nullptr)
	{

	}

	CM::RTTITypeBase* ManagedComponent::getRTTIStatic()
	{
		return ManagedComponentRTTI::instance();
	}

	CM::RTTITypeBase* ManagedComponent::getRTTI() const
	{
		return ManagedComponent::getRTTIStatic();
	}
}