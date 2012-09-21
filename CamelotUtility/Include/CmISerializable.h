#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT ISerializable : IReflectable
	{
	public:
		virtual SerializableType* getSerializable() = 0;
	};
}