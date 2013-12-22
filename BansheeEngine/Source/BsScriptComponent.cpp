#include "BsScriptComponent.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptComponent::ScriptComponent(const CM::HSceneObject& parent, MonoObject* managedInstance)
		:Component(parent), mManagedInstance(managedInstance), mUpdateThunk(nullptr)
	{
		MonoClass* monoClass = mono_object_get_class(mManagedInstance);

		if(monoClass == nullptr)
			CM_EXCEPT(InternalErrorException, "Cannot retrieve class from MonoObject.");

		MonoMethod* updateMethod = mono_class_get_method_from_name(monoClass, "Update", 0);
		if(updateMethod != nullptr)
			mUpdateThunk = (void(*)())mono_method_get_unmanaged_thunk(updateMethod);
	}

	void ScriptComponent::update()
	{
		if(mUpdateThunk != nullptr)
			mUpdateThunk();
	}
}