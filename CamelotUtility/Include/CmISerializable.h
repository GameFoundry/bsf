#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmSerializableType.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT ISerializable : public IReflectable
	{
	public:
		virtual RTTITypeBase* getRTTI() const { return getSerializable(); }
		virtual SerializableType* getSerializable() const = 0;
	};
}