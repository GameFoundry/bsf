#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_EXPORT RenderableHandler
	{
	public:
		virtual void initializeRenderElem(RenderableElement* element) = 0;

		virtual void bindPerObjectBuffers(const RenderableElement* element) = 0;
		virtual void bindGlobalBuffers(const RenderableElement* element);
	};
}