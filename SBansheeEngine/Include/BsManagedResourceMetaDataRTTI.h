#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsManagedResourceMetaData.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedResourceMetaDataRTTI : public RTTIType<ManagedResourceMetaData, ResourceMetaData, ManagedResourceMetaDataRTTI>
	{
	private:
		String& getNamespace(ManagedResourceMetaData* obj)
		{
			return obj->typeNamespace;
		}

		void setNamespace(ManagedResourceMetaData* obj, String& val)
		{
			obj->typeNamespace = val;
		}

		String& getTypename(ManagedResourceMetaData* obj)
		{
			return obj->typeName;
		}

		void setTypename(ManagedResourceMetaData* obj, String& val)
		{
			obj->typeName = val;
		}

	public:
		ManagedResourceMetaDataRTTI()
		{
			addPlainField("mTypeNamespace", 0, &ManagedResourceMetaDataRTTI::getNamespace, &ManagedResourceMetaDataRTTI::setNamespace);
			addPlainField("mTypeName", 1, &ManagedResourceMetaDataRTTI::getTypename, &ManagedResourceMetaDataRTTI::setTypename);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ManagedResourceMetaData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ManagedResourceMetaData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr_new<ManagedResourceMetaData>();
		}
	};
}