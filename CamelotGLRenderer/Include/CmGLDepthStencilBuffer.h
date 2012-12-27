#pragma once

#include "CmGLPrerequisites.h"
#include "CmDepthStencilBuffer.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLDepthStencilBuffer : public DepthStencilBuffer
	{
	public:
		GLDepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);
		// TODO - Add GL specific implementation
	};
}