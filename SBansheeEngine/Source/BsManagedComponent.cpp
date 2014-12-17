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
		:mUpdateThunk(nullptr), mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr)
	{ }

	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		: Component(parent), mManagedInstance(nullptr), mRuntimeType(runtimeType), mUpdateThunk(nullptr), 
		mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr)
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

		initialize(managedClass->createInstance(), runtimeType, managedClass);
	}

	ManagedComponent::~ManagedComponent()
	{
		if (mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}
	}

	void ManagedComponent::initialize(MonoObject* object, MonoReflectionType* runtimeType, MonoClass* monoClass)
	{
		mFullTypeName = mNamespace + "." + mTypeName;

		mManagedInstance = object;
		mRuntimeType = runtimeType;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);

		if (monoClass != nullptr)
		{
			MonoMethod* onInitializedMethod = monoClass->getMethod("OnInitialize", 0);
			if (onInitializedMethod != nullptr)
				mOnInitializedThunk = (OnInitializedThunkDef)onInitializedMethod->getThunk();

			MonoMethod* updateMethod = monoClass->getMethod("Update", 0);
			if (updateMethod != nullptr)
				mUpdateThunk = (UpdateThunkDef)updateMethod->getThunk();

			MonoMethod* onDestroyMethod = monoClass->getMethod("OnDestroy", 0);
			if (onDestroyMethod != nullptr)
				mOnDestroyThunk = (OnDestroyedThunkDef)onDestroyMethod->getThunk();
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

	void ManagedComponent::onInitialized()
	{
		assert(mManagedInstance != nullptr);

		ScriptComponent* nativeInstance = ScriptComponent::toNative(mManagedInstance);

		// Find handle to self
		HManagedComponent componentHandle;
		if (mParent != nullptr)
		{
			const Vector<HComponent>& components = mParent->getComponents();
			for (auto& component : components)
			{
				if (component.get() == this)
				{
					componentHandle = component;
					break;
				}
			}
		}

		assert(componentHandle != nullptr);
		ScriptGameObjectManager::instance().registerScriptComponent(nativeInstance, componentHandle);

		if (mOnInitializedThunk != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnInitializedThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}
	}

	void ManagedComponent::onDestroyed()
	{
		assert(mManagedInstance != nullptr);

		if (mOnDestroyThunk != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnDestroyThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
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