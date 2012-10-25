#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResource.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceRTTI : public RTTIType<Resource, IReflectable, ResourceRTTI>
	{
	private:
		UINT32& getSize(Resource* obj) { return obj->mSize; }
		void setSize(Resource* obj, UINT32& size) { obj->mSize = size; } 
		String& getUUID(Resource* obj) { return obj->mUUID; }
		void setUUID(Resource* obj, String& uuid) { obj->mUUID = uuid; }

	public:
		ResourceRTTI()
		{
			addPlainField("Size", 0, &ResourceRTTI::getSize, &ResourceRTTI::setSize);
			addPlainField("UUID", 1, &ResourceRTTI::getUUID, &ResourceRTTI::setUUID);
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
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}