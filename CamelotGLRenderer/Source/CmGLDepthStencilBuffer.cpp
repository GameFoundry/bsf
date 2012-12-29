#include "CmGLDepthStencilBuffer.h"
#include "CmGLHardwarePixelBuffer.h"

namespace CamelotEngine
{
	GLDepthStencilBuffer::GLDepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(bitDepth, width, height, fsaa, fsaaHint)
	{
		mDepthStencilRenderBuffer = GLRenderBufferPtr(new GLRenderBuffer(GL_DEPTH24_STENCIL8_EXT, width, height, fsaa));
	}

	GLDepthStencilBuffer::~GLDepthStencilBuffer()
	{

	}
}