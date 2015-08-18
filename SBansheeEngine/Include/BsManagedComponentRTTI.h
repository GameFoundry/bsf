#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectRTTI.h"
#include "BsManagedComponent.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"
#include "BsGameObjectManager.h"
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
			obj->mSerializedObjectData = val;
		}

		bool& getMissingType(ManagedComponent* obj)
		{
			return obj->mMissingType;
		}

		void setMissingType(ManagedComponent* obj, bool& val)
		{
			obj->mMissingType = val;
		}

	public:
		ManagedComponentRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedComponentRTTI::getNamespace, &ManagedComponentRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedComponentRTTI::getTypename, &ManagedComponentRTTI::setTypename);
			addReflectablePtrField("mObjectData", 2, &ManagedComponentRTTI::getObjectData, &ManagedComponentRTTI::setObjectData);
			addPlainField("mMissingType", 3, &ManagedComponentRTTI::getMissingType, &ManagedComponentRTTI::setMissingType);
		}

		void onSerializationStarted(IReflectable* obj) override
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);

			mc->mRTTIData = ManagedSerializableObject::createFromExisting(mc->getManagedInstance());
		}

		virtual const String& getRTTIName() override
		{
			static String name = "ManagedComponent";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ManagedComponent;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<ManagedComponent>();
		}
	};
}