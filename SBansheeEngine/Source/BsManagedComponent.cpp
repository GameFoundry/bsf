#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsMonoMethod.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent()
		:mUpdateThunk(nullptr)
	{ }

	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		:Component(parent), mManagedInstance(nullptr), mRuntimeType(runtimeType), mUpdateThunk(nullptr)
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

		construct(managedClass->createInstance(), runtimeType, managedClass);
	}

	void ManagedComponent::construct(MonoObject* object, MonoReflectionType* runtimeType, MonoClass* monoClass)
	{
		mFullTypeName = mNamespace + "." + mTypeName;

		mManagedInstance = object;
		mRuntimeType = runtimeType;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);

		if (monoClass != nullptr)
		{
			MonoMethod* updateMethod = monoClass->getMethod("Update", 0);
			if (updateMethod != nullptr)
				mUpdateThunk = (UpdateThunkDef)updateMethod->getThunk();
		}
	}

	void ManagedComponent::update()
	{
		if (mUpdateThunk != nullptr && mManagedInstance != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mUpdateThunk(mManagedInstance, &exception); 

			MonoUtil::throwIfException(exception);
		}
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