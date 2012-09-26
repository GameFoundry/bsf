#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResource.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceST : public RTTIType<Resource, IReflectable, ResourceST>
	{
	private:
		UINT32& getSize(Resource* obj) { return obj->mSize; }
		void setSize(Resource* obj, UINT32& size) { obj->mSize = size; } 
		UUID& getUUID(Resource* obj) { return obj->mSourceUUID; }
		void setUUID(Resource* obj, UUID& uuid) { obj->mSourceUUID = uuid; }

	public:
		ResourceST()
		{
			addPlainField("Size", 0, &ResourceST::getSize, &ResourceST::setSize);
			addPlainField("UUID", 1, &ResourceST::getUUID, &ResourceST::setUUID);
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