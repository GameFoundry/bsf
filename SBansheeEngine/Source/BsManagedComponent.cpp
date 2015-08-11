#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsMonoMethod.h"
#include "BsMemorySerializer.h"
#include "BsManagedSerializableObject.h"
#include "BsScriptGameObjectManager.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent()
		:mManagedInstance(nullptr), mUpdateThunk(nullptr), mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), 
		mOnResetThunk(nullptr), mMissingType(false), mRequiresReset(true), mOnEnabledThunk(nullptr), mOnDisabledThunk(nullptr)
	{ }

	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		: Component(parent), mManagedInstance(nullptr), mRuntimeType(runtimeType), mUpdateThunk(nullptr), 
		mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), mOnResetThunk(nullptr), mMissingType(false),
		mRequiresReset(true), mOnEnabledThunk(nullptr), mOnDisabledThunk(nullptr)
	{
		MonoType* monoType = mono_reflection_type_get_type(mRuntimeType);
		::MonoClass* monoClass = mono_type_get_class(monoType);

		MonoUtil::getClassName(monoClass, mNamespace, mTypeName);

		MonoClass* managedClass = MonoManager::instance().findClass(mNamespace, mTypeName);
		if(managedClass == nullptr)
		{
			LOGWRN("Cannot create managed component: " + mNamespace + "." + mTypeName + " because that type doesn't exist.");
			return;
		}

		setName(mTypeName);

		initialize(managedClass->createInstance());
	}

	ManagedComponent::~ManagedComponent()
	{

	}

	ComponentBackupData ManagedComponent::backup(bool clearExisting)
	{
		ComponentBackupData backupData;

		// If type is not missing read data from actual managed instance, instead just 
		// return the data we backed up before the type was lost
		if (!mMissingType)
		{
			ManagedSerializableObjectPtr serializableObject = ManagedSerializableObject::createFromExisting(mManagedInstance);

			// Serialize the object information and its fields. We cannot just serialize the entire object because
			// the managed instance had to be created in a previous step. So we handle creation of the top level object manually.
			
			if (serializableObject != nullptr)
			{
				MemorySerializer ms;

				backupData.size = 0;
				backupData.data = ms.encode(serializableObject.get(), backupData.size);
			}
			else
			{
				backupData.size = 0;
				backupData.data = nullptr;
			}
		}
		else
		{
			MemorySerializer ms;

			backupData.size = 0;

			if (mMissingTypeObjectData != nullptr)
				backupData.data = ms.encode(mMissingTypeObjectData.get(), backupData.size);
			else
				backupData.data = nullptr;
		}

		if (clearExisting)
		{
			if (mManagedInstance != nullptr)
			{
				mManagedInstance = nullptr;
				mono_gchandle_free(mManagedHandle);
				mManagedHandle = 0;
			}

			mRuntimeType = nullptr;
			mOnInitializedThunk = nullptr;
			mUpdateThunk = nullptr;
			mOnDestroyThunk = nullptr;
			mOnEnabledThunk = nullptr;
			mOnDisabledThunk = nullptr;
		}

		return backupData;
	}

	void ManagedComponent::restore(MonoObject* instance, const ComponentBackupData& data, bool missingType)
	{
		initialize(instance);

		if (instance != nullptr && data.data != nullptr)
		{
			MemorySerializer ms;

			GameObjectManager::instance().startDeserialization();
			ManagedSerializableObjectPtr serializableObject = std::static_pointer_cast<ManagedSerializableObject>(ms.decode(data.data, data.size));
			GameObjectManager::instance().endDeserialization();

			if (!missingType)
				serializableObject->deserialize();
			else
				mMissingTypeObjectData = serializableObject;
		}

		if (!missingType)
			mMissingTypeObjectData = nullptr;

		mMissingType = missingType;
		mRequiresReset = true;
	}

	void ManagedComponent::initialize(MonoObject* object)
	{
		mFullTypeName = mNamespace + "." + mTypeName;
		mManagedInstance = object;
		
		MonoClass* managedClass = nullptr;
		if (mManagedInstance != nullptr)
		{
			mManagedHandle = mono_gchandle_new(mManagedInstance, false);

			::MonoClass* monoClass = mono_object_get_class(object);
			MonoType* monoType = mono_class_get_type(monoClass);
			mRuntimeType = mono_type_get_object(MonoManager::instance().getDomain(), monoType);

			managedClass = MonoManager::instance().findClass(monoClass);
		}

		if (managedClass != nullptr)
		{
			MonoMethod* onInitializedMethod = managedClass->getMethod("OnInitialize", 0);
			if (onInitializedMethod != nullptr)
				mOnInitializedThunk = (OnInitializedThunkDef)onInitializedMethod->getThunk();

			MonoMethod* updateMethod = managedClass->getMethod("Update", 0);
			if (updateMethod != nullptr)
				mUpdateThunk = (UpdateThunkDef)updateMethod->getThunk();

			MonoMethod* onResetMethod = managedClass->getMethod("OnReset", 0);
			if (onResetMethod != nullptr)
				mOnResetThunk = (OnResetThunkDef)onResetMethod->getThunk();

			MonoMethod* onDestroyMethod = managedClass->getMethod("OnDestroy", 0);
			if (onDestroyMethod != nullptr)
				mOnDestroyThunk = (OnDestroyedThunkDef)onDestroyMethod->getThunk();

			MonoMethod* onDisableMethod = managedClass->getMethod("OnDisable", 0);
			if (onDisableMethod != nullptr)
				mOnDisabledThunk = (OnDisabledThunkDef)onDisableMethod->getThunk();

			MonoMethod* onEnableMethod = managedClass->getMethod("OnEnable", 0);
			if (onEnableMethod != nullptr)
				mOnEnabledThunk = (OnInitializedThunkDef)onEnableMethod->getThunk();
		}
	}

	void ManagedComponent::update()
	{
		if (mUpdateThunk != nullptr && mManagedInstance != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mUpdateThunk, mManagedInstance);
		}
	}

	void ManagedComponent::triggerOnReset()
	{
		if (mRequiresReset && mOnResetThunk != nullptr && mManagedInstance != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnResetThunk, mManagedInstance);
		}

		mRequiresReset = false;
	}

	void ManagedComponent::onInitialized()
	{
		assert(mManagedInstance != nullptr);

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
		ScriptComponent* nativeInstance = ScriptGameObjectManager::instance().createScriptComponent(mManagedInstance, componentHandle);

		if (mOnInitializedThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnInitializedThunk, mManagedInstance);
		}

		triggerOnReset();
	}

	void ManagedComponent::onDestroyed()
	{
		assert(mManagedInstance != nullptr);

		if (mOnDestroyThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnDestroyThunk, mManagedInstance);
		}

		if (mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}
	}

	void ManagedComponent::onEnabled()
	{
		assert(mManagedInstance != nullptr);

		if (mOnEnabledThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnEnabledThunk, mManagedInstance);
		}
	}

	void ManagedComponent::onDisabled()
	{
		assert(mManagedInstance != nullptr);

		if (mOnDisabledThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnDisabledThunk, mManagedInstance);
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