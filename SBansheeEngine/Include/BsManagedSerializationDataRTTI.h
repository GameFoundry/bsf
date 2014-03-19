#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsManagedSerializationData.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializationDataRTTI : public CM::RTTIType<ManagedSerializationData, CM::IReflectable, ManagedSerializationDataRTTI>
	{
	private:
		CM::String& getNamespace(ManagedSerializationData* obj)
		{
			return obj->mNamespace;
		}

		void setNamespace(ManagedSerializationData* obj, CM::String& val)
		{
			obj->mNamespace = val;
		}

		CM::String& getTypename(ManagedSerializationData* obj)
		{
			return obj->mTypename;
		}

		void setTypename(ManagedSerializationData* obj, CM::String& val)
		{
			obj->mTypename = val;
		}

	public:
		ManagedSerializationDataRTTI()
		{
			addPlainField("mNamespace", 0, &ManagedSerializationDataRTTI::getNamespace, &ManagedSerializationDataRTTI::setNamespace);
			addPlainField("mTypename", 1, &ManagedSerializationDataRTTI::getTypename, &ManagedSerializationDataRTTI::setTypename);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ManagedSerializationData";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ManagedSerializationData;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			CM_EXCEPT(CM::InvalidStateException, "Cannot instantiate an abstract class.");
		}
	};
}