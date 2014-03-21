#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "CmDebug.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent(const CM::HSceneObject& parent, const CM::String& ns, const CM::String& typeName)
		:mManagedInstance(nullptr), mNamespace(ns), mTypeName(typeName)
	{
		MonoClass* managedClass = MonoManager::instance().findClass(mNamespace, mTypeName);
		if(managedClass == nullptr)
		{
			LOGWRN("Cannot create managed component: " + mNamespace + "." + mTypeName + " because that type doesn't exist.");
			return;
		}

		construct(managedClass->createInstance());
	}

	void ManagedComponent::construct(MonoObject* object)
	{
		mFullTypeName = mNamespace + "." + mTypeName;

		mManagedInstance = object;
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

	CM::RTTITypeBase* ManagedComponent::getRTTIStatic()
	{
		return ManagedComponentRTTI::instance();
	}

	CM::RTTITypeBase* ManagedComponent::getRTTI() const
	{
		return ManagedComponent::getRTTIStatic();
	}
}