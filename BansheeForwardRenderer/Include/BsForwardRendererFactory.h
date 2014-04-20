#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRendererFactory.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeForwardRenderer";

	class BS_FWDRND_EXPORT ForwardRendererFactory : public RendererFactory
	{
	public:
		virtual RendererPtr create();
		virtual const String& name() const;
	};
}