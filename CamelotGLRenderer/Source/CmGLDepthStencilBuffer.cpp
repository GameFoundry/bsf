#include "CmGLDepthStencilBuffer.h"

namespace CamelotEngine
{
	GLDepthStencilBuffer::GLDepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(bitDepth, width, height, fsaa, fsaaHint)
	{ }
}