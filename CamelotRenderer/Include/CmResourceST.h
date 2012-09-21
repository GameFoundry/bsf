#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmSerializableType.h"
#include "CmResource.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceST : public SerializableType
	{
	private:
		UINT32 getSize(Resource* obj) { return obj->mSize; }
		void setSize(Resource* obj, UINT32 size) { obj->mSize = size; } 
	public:
		ResourceST()
		{
			addPlainField("Size", 0, &ResourceST::getSize, &ResourceST::setSize);
		}
	};
}