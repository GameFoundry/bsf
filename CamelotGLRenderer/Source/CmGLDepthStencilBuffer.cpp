#include "CmGLDepthStencilBuffer.h"
#include "CmGLHardwarePixelBuffer.h"
#include "CmGLPixelFormat.h"

namespace CamelotEngine
{
	GLDepthStencilBuffer::GLDepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(format, width, height, fsaa, fsaaHint)
	{
		GLenum glDepthFormat = GLPixelUtil::getDepthStencilFormat(format);
		mDepthStencilRenderBuffer = GLRenderBufferPtr(new GLRenderBuffer(glDepthFormat, width, height, fsaa));
	}

	GLDepthStencilBuffer::~GLDepthStencilBuffer()
	{

	}
}