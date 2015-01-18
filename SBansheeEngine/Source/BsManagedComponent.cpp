#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsMonoMethod.h"
#include "BsMemorySerializer.h"
#include "BsManagedSerializableField.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableObjectData.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent()
		:mManagedInstance(nullptr), mUpdateThunk(nullptr), mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), 
		mOnResetThunk(nullptr), mMissingType(false)
	{ }

	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		: Component(parent), mManagedInstance(nullptr), mRuntimeType(runtimeType), mUpdateThunk(nullptr), 
		mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), mOnResetThunk(nullptr), mMissingType(false)
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
		if (mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}
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
				ManagedSerializableObjectInfoPtr objectInfo = serializableObject->getObjectInfo();
				ManagedSerializableObjectDataPtr objectData = serializableObject->getObjectData();

				MemorySerializer ms;

				backupData.mTypeInfo.size = 0;
				backupData.mTypeInfo.data = ms.encode(objectInfo.get(), backupData.mTypeInfo.size);

				backupData.mObjectData.size = 0;
				backupData.mObjectData.data = ms.encode(objectData.get(), backupData.mObjectData.size);
			}
			else
			{
				backupData.mTypeInfo.size = 0;
				backupData.mTypeInfo.data = nullptr;

				backupData.mObjectData.size = 0;
				backupData.mObjectData.data = nullptr;
			}
		}
		else
		{
			MemorySerializer ms;

			backupData.mTypeInfo.size = 0;

			if (mMissingTypeObjectInfo != nullptr)
				backupData.mTypeInfo.data = ms.encode(mMissingTypeObjectInfo.get(), backupData.mTypeInfo.size);
			else
				backupData.mTypeInfo.data = nullptr;

			backupData.mObjectData.size = 0;

			if (mMissingTypeObjectData != nullptr)
				backupData.mObjectData.data = ms.encode(mMissingTypeObjectData.get(), backupData.mObjectData.size);
			else
				backupData.mObjectData.data = nullptr;
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
		}

		return backupData;
	}

	void ManagedComponent::restore(MonoObject* instance, const ComponentBackupData& data, bool missingType)
	{
		initialize(instance);

		if (instance != nullptr && data.mTypeInfo.data != nullptr && data.mObjectData.data != nullptr)
		{
			MemorySerializer ms;
			ManagedSerializableObjectInfoPtr objectInfo = std::static_pointer_cast<ManagedSerializableObjectInfo>(ms.decode(data.mTypeInfo.data, data.mTypeInfo.size));

			GameObjectManager::instance().startDeserialization();
			ManagedSerializableObjectDataPtr objectData = std::static_pointer_cast<ManagedSerializableObjectData>(ms.decode(data.mObjectData.data, data.mObjectData.size));
			GameObjectManager::instance().endDeserialization();

			if (!missingType)
			{
				ManagedSerializableObjectPtr serializableObject = ManagedSerializableObject::createFromExisting(instance);
				serializableObject->setObjectData(objectData, objectInfo);
			}
			else
			{
				mMissingTypeObjectInfo = objectInfo;
				mMissingTypeObjectData = objectData;
			}
		}

		if (!missingType)
		{
			mMissingTypeObjectInfo = nullptr;
			mMissingTypeObjectData = nullptr;
		}

		mMissingType = missingType;
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

	void ManagedComponent::triggerOnReset()
	{
		if (mOnResetThunk != nullptr && mManagedInstance != nullptr)
		{
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnResetThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}
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
			MonoException* exception = nullptr;

			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			mOnInitializedThunk(mManagedInstance, &exception);

			MonoUtil::throwIfException(exception);
		}

		triggerOnReset();
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