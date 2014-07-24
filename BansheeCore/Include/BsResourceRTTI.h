#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResource.h"
#include "BsResourceMetaData.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ResourceRTTI : public RTTIType<Resource, IReflectable, ResourceRTTI>
	{
	private:
		UINT32& getSize(Resource* obj) { return obj->mSize; }
		void setSize(Resource* obj, UINT32& size) { obj->mSize = size; } 

		ResourceMetaDataPtr getMetaData(Resource* obj) { return obj->mMetaData; }
		void setMetaData(Resource* obj, ResourceMetaDataPtr value) { obj->mMetaData = value; }

	public:
		ResourceRTTI()
		{
			addPlainField("mSize", 0, &ResourceRTTI::getSize, &ResourceRTTI::setSize);
			addReflectablePtrField("mMetaData", 1, &ResourceRTTI::getMetaData, &ResourceRTTI::setMetaData);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Resource";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return 100;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}