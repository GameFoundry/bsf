#include "CmD3D9DepthStencilBuffer.h"

namespace CamelotEngine
{
	D3D9DepthStencilBuffer::D3D9DepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(bitDepth, width, height, fsaa, fsaaHint)
	{ }
}