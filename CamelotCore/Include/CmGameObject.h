#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_EXPORT GameObject : public IReflectable
	{
	public:
		virtual ~GameObject() 
		{ }
	};
}