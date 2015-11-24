#include "BsManagedComponent.h"
#include "BsManagedComponentRTTI.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsMonoMethod.h"
#include "BsMemorySerializer.h"
#include "BsManagedSerializableObject.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedComponent::ManagedComponent()
		:mManagedInstance(nullptr), mUpdateThunk(nullptr), mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), 
		mOnResetThunk(nullptr), mMissingType(false), mRequiresReset(true), mOnEnabledThunk(nullptr), mOnDisabledThunk(nullptr),
		mCalculateBoundsMethod(nullptr)
	{ }

	ManagedComponent::ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType)
		: Component(parent), mManagedInstance(nullptr), mRuntimeType(runtimeType), mUpdateThunk(nullptr), 
		mOnDestroyThunk(nullptr), mOnInitializedThunk(nullptr), mOnResetThunk(nullptr), mMissingType(false),
		mRequiresReset(true), mOnEnabledThunk(nullptr), mOnDisabledThunk(nullptr), mCalculateBoundsMethod(nullptr)
	{
		MonoType* monoType = mono_reflection_type_get_type(mRuntimeType);
		::MonoClass* monoClass = mono_type_get_class(monoType);

		MonoUtil::getClassName(monoClass, mNamespace, mTypeName);
		setName(mTypeName);
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

			if (mSerializedObjectData != nullptr)
				backupData.data = ms.encode(mSerializedObjectData.get(), backupData.size);
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
			mCalculateBoundsMethod = nullptr;
		}

		return backupData;
	}

	void ManagedComponent::restore(MonoObject* instance, const ComponentBackupData& data, bool missingType)
	{
		initialize(instance);
		mObjInfo = nullptr;

		if (instance != nullptr && data.data != nullptr)
		{
			MemorySerializer ms;

			GameObjectManager::instance().startDeserialization();
			ManagedSerializableObjectPtr serializableObject = std::static_pointer_cast<ManagedSerializableObject>(ms.decode(data.data, data.size));
			GameObjectManager::instance().endDeserialization();

			if (!missingType)
			{
				ScriptAssemblyManager::instance().getSerializableObjectInfo(mNamespace, mTypeName, mObjInfo);

				serializableObject->deserialize(instance, mObjInfo);
			}
			else
				mSerializedObjectData = serializableObject;
		}

		if (!missingType)
			mSerializedObjectData = nullptr;

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

			mCalculateBoundsMethod = managedClass->getMethod("CalculateBounds", 2);
		}
	}

	bool ManagedComponent::typeEquals(const Component& other)
	{
		if(Component::typeEquals(other))
		{
			const ManagedComponent& otherMC = static_cast<const ManagedComponent&>(other);

			// Not comparing MonoReflectionType directly because this needs to be able to work before instantiation
			return mNamespace == otherMC.getManagedNamespace() && mTypeName == otherMC.getManagedTypeName();
		}

		return false;
	}

	bool ManagedComponent::calculateBounds(Bounds& bounds)
	{
		if (mManagedInstance != nullptr && mCalculateBoundsMethod != nullptr)
		{
			AABox box;
			Sphere sphere;

			void* params[2];
			params[0] = &box;
			params[1] = &sphere;

			MonoObject* areBoundsValidObj = mCalculateBoundsMethod->invokeVirtual(mManagedInstance, params);

			bool areBoundsValid;
			areBoundsValid = *(bool*)mono_object_unbox(areBoundsValidObj);

			bounds = Bounds(box, sphere);
			return areBoundsValid;
		}

		return Component::calculateBounds(bounds);
	}

	void ManagedComponent::update()
	{
		assert(mManagedInstance != nullptr);

		if (mUpdateThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mUpdateThunk, mManagedInstance);
		}
	}

	void ManagedComponent::triggerOnReset()
	{
		assert(mManagedInstance != nullptr);

		if (mRequiresReset && mOnResetThunk != nullptr)
		{
			// Note: Not calling virtual methods. Can be easily done if needed but for now doing this
			// for some extra speed.
			MonoUtil::invokeThunk(mOnResetThunk, mManagedInstance);
		}

		mRequiresReset = false;
	}

	void ManagedComponent::instantiate()
	{
		mObjInfo = nullptr;
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(mNamespace, mTypeName, mObjInfo))
		{
			MonoObject* instance = ScriptAssemblyManager::instance().getMissingComponentClass()->createInstance(true);

			initialize(instance);
			mMissingType = true;
		}
		else
		{
			initialize(mObjInfo->mMonoClass->createInstance());
			mMissingType = false;
		}

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
	}

	void ManagedComponent::onInitialized()
	{
		assert(mManagedInstance != nullptr);

		if (mSerializedObjectData != nullptr && !mMissingType)
		{
			mSerializedObjectData->deserialize(mManagedInstance, mObjInfo);
			mSerializedObjectData = nullptr;
		}

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

		mManagedInstance = nullptr;
		mono_gchandle_free(mManagedHandle);
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