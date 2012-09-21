#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIType.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT SerializableType : public RTTIType
	{
	public:
		virtual void beginSerialization() {}
		virtual void endSerialization() {}
	};
}