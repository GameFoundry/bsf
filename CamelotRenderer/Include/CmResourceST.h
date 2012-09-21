#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmSerializableType.h"
#include "CmResource.h"

namespace CamelotEngine
{
	CM_SERIALIZABLE_SIMPLE_TYPE(UUID, 11);
	CM_SERIALIZABLE_SIMPLE_TYPE(UINT32&, 12);
	class CM_EXPORT ResourceST : public SerializableType
	{
	private:
		UINT32 getSize(Resource* obj) { return obj->mSize; }
		void setSize(Resource* obj, UINT32 size) { obj->mSize = size; } 
		UUID getUUID(Resource* obj) { return obj->mSourceUUID; }
		void setUUID(Resource* obj, UUID uuid) { obj->mSourceUUID = uuid; }

	public:
		ResourceST()
		{
			addPlainField("Size", 0, &ResourceST::getSize, &ResourceST::setSize);
			addPlainField("UUID", 1, &ResourceST::getUUID, &ResourceST::setUUID);
		}
	};
}