#pragma once

#include "CmForwardRendererPrerequisites.h"
#include "CmRendererFactory.h"

namespace CamelotEngine
{
	const std::string SystemName = "CamelotForwardRenderer";

	class CM_FWDRND_EXPORT ForwardRendererFactory : public RendererFactory
	{
	public:
		virtual RendererPtr create();
		virtual const std::string& name() const;
	};
}