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

		bool& getMissingType(ManagedComponent* obj)
		{
			return obj->mMissingType;
		}

		void setMissingType(ManagedComponent* obj, bool& val)
		{
			obj->mMissingType = val;
		}

		ManagedSerializableObjectInfoPtr getMissingTypeObjectInfo(ManagedComponent* obj)
		{
			return obj->mMissingTypeObjectInfo;
		}

		void setMissingTypeObjectInfo(ManagedComponent* obj, ManagedSerializableObjectInfoPtr val)
		{
			obj->mMissingTypeObjectInfo = val;
		}

		ManagedSerializableObjectDataPtr getMissingTypeObjectData(ManagedComponent* obj)
		{
			return obj->mMissingTypeObjectData;
		}

		void setMissingTypeObjectData(ManagedComponent* obj, ManagedSerializableObjectDataPtr val)
		{
			obj->mMissingTypeObjectData = val;
		}

	public:
		ManagedComponentRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedComponentRTTI::getNamespace, &ManagedComponentRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedComponentRTTI::getTypename, &ManagedComponentRTTI::setTypename);
			addReflectablePtrField("mObjectData", 2, &ManagedComponentRTTI::getObjectData, &ManagedComponentRTTI::setObjectData);
			addPlainField("mMissingType", 3, &ManagedComponentRTTI::getMissingType, &ManagedComponentRTTI::setMissingType);
			addReflectablePtrField("mMissingTypeObjectInfo", 4, &ManagedComponentRTTI::getMissingTypeObjectInfo, &ManagedComponentRTTI::setMissingTypeObjectInfo);
			addReflectablePtrField("mMissingTypeObjectData", 5, &ManagedComponentRTTI::getMissingTypeObjectData, &ManagedComponentRTTI::setMissingTypeObjectData);
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
			MonoObject* managedInstance = serializableObject->getManagedInstance();

			mc->initialize(managedInstance);
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