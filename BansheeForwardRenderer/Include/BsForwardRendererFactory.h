#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRendererFactory.h"

namespace BansheeEngine
{
	const std::string SystemName = "BansheeForwardRenderer";

	class BS_FWDRND_EXPORT ForwardRendererFactory : public CM::RendererFactory
	{
	public:
		virtual CM::RendererPtr create();
		virtual const std::string& name() const;
	};
}