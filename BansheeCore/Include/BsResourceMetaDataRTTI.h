#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceMetaData.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ResourceMetaDataRTTI : public RTTIType<ResourceMetaData, IReflectable, ResourceMetaDataRTTI>
	{
	private:
		WString& getDisplayName(ResourceMetaData* obj) { return obj->displayName; }
		void setDisplayName(ResourceMetaData* obj, WString& name) { obj->displayName = name; }

	public:
		ResourceMetaDataRTTI()
		{
			addPlainField("displayName", 0, &ResourceMetaDataRTTI::getDisplayName, &ResourceMetaDataRTTI::setDisplayName);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "ResourceMetaData";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ResourceMetaData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ResourceMetaData>();
		}
	};
}