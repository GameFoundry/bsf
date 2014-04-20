#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	class CM_EXPORT RendererFactory
	{
	public:
		virtual RendererPtr create() = 0;
		virtual const String& name() const = 0;
	};
}