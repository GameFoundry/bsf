#pragma once

#include "CmGLPrerequisites.h"
#include "CmDepthStencilBuffer.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLDepthStencilBuffer : public DepthStencilBuffer
	{
	public:
		GLDepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);
		~GLDepthStencilBuffer();

		GLRenderBufferPtr getGLRenderBuffer() const { return mDepthStencilRenderBuffer; }

	protected:
		GLRenderBufferPtr mDepthStencilRenderBuffer;
	};
}