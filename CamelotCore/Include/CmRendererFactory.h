#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	class CM_EXPORT RendererFactory
	{
	public:
		virtual RendererPtr create() = 0;
		virtual const std::string& name() const = 0;
	};
}