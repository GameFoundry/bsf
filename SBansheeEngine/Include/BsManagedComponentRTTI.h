#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectRTTI.h"
#include "BsManagedComponent.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponentRTTI : public CM::RTTIType<ManagedComponent, CM::Component, ManagedComponentRTTI>
	{
	private:
		CM::String& getNamespace(ManagedComponent* obj)
		{
			return obj->mNamespace;
		}

		void setNamespace(ManagedComponent* obj, CM::String& val)
		{
			obj->mNamespace = val;
		}

		CM::String& getTypename(ManagedComponent* obj)
		{
			return obj->mTypeName;
		}

		void setTypename(ManagedComponent* obj, CM::String& val)
		{
			obj->mTypeName = val;
		}

	public:
		ManagedComponentRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedComponentRTTI::getNamespace, &ManagedComponentRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedComponentRTTI::getTypename, &ManagedComponentRTTI::setTypename);
		}

		void onSerializationStarted(CM::IReflectable* obj)
		{
		}

		virtual void onDeserializationEnded(CM::IReflectable* obj)
		{
			ManagedComponent* mc = static_cast<ManagedComponent*>(obj);
			mc->construct();
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ManagedComponent";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ManagedComponent;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::GameObjectRTTI::createGameObject<ManagedComponent>();
		}
	};
}