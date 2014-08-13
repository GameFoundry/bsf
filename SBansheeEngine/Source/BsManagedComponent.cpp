#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		:mManagedInstance(nullptr), mRuntimeType(runtimeType)
	{
		MonoType* monoType = mono_reflection_type_get_type(mRuntimeType);
		::MonoClass* monoClass = mono_type_get_class(monoType);

		mNamespace = mono_class_get_namespace(monoClass);
		mTypeName = mono_class_get_name(monoClass);

		MonoClass* managedClass = MonoManager::instance().findClass(mNamespace, mTypeName);
		if(managedClass == nullptr)
		{
			LOGWRN("Cannot create managed component: " + mNamespace + "." + mTypeName + " because that type doesn't exist.");
			return;
		}

		setName(mTypeName);

		construct(managedClass->createInstance(), runtimeType);
	}

	void ManagedComponent::construct(MonoObject* object, MonoReflectionType* runtimeType)
	{
		mFullTypeName = mNamespace + "." + mTypeName;

		mManagedInstance = object;
		mRuntimeType = runtimeType;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);
	}

	void ManagedComponent::onDestroyed()
	{
		if(mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}
	}

	RTTITypeBase* ManagedComponent::getRTTIStatic()
	{
		return ManagedComponentRTTI::instance();
	}

	RTTITypeBase* ManagedComponent::getRTTI() const
	{
		return ManagedComponent::getRTTIStatic();
	}
}