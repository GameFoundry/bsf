#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectRTTI.h"
#include "BsManagedComponent.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"
#include "BsGameObjectManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptComponent.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponentRTTI : public RTTIType<ManagedComponent, Component, ManagedComponentRTTI>
	{
	private:
		String& getNamespace(ManagedComponent* obj)
		{
			return obj->mNamespace;
		}

		void setNamespace(ManagedComponent* obj, String& val)
		{
			obj->mNamespace = val;
		}

		String& getTypename(ManagedComponent* obj)
		{
			return obj->mTypeName;
		}

		void setTypename(ManagedComponent* obj, String& val)
		{
			obj->mTypeName = val;
		}

		ManagedSerializableObjectPtr getObjectData(ManagedComponent* obj)
		{
			return any_cast<ManagedSerializableObjectPtr>(obj->mRTTIData);
		}

		void setObjectData(ManagedComponent* obj, ManagedSerializableObjectPtr val)
		{
			obj->mRTTIData = val;
		}

	public:
		ManagedComponentRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedComponentRTTI::getNamespace, &ManagedComponentRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedComponentRTTI::getTypename, &ManagedComponentRTTI::setTypename);
			addReflectablePtrField("mObjectData", 2, &ManagedComponentRTTI::getObjectData, &ManagedComponentRTTI::setObjectData);
		}

		void onSerializationStarted(IReflectable* obj)
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);

			mc->mRTTIData = ManagedSerializableObject::createFromExisting(mc->getManagedInstance());
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);

			GameObjectManager::instance().registerOnDeserializationEndCallback(std::bind(&ManagedComponentRTTI::finalizeDeserialization, mc));
		}

		static void finalizeDeserialization(ManagedComponent* mc)
		{
			ManagedSerializableObjectPtr serializableObject = any_cast<ManagedSerializableObjectPtr>(mc->mRTTIData);

			::MonoClass* monoClass = mono_object_get_class(serializableObject->getManagedInstance());
			MonoType* monoType = mono_class_get_type(monoClass);
			MonoReflectionType* runtimeType = mono_type_get_object(MonoManager::instance().getDomain(), monoType);

			MonoClass* managedClass = MonoManager::instance().findClass(monoClass);
			MonoObject* managedInstance = serializableObject->getManagedInstance();

			mc->initialize(managedInstance, runtimeType, managedClass);
			mc->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName()
		{
			static String name = "ManagedComponent";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ManagedComponent;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<ManagedComponent>();
		}
	};
}