#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmDepthStencilBuffer.h"

namespace CamelotEngine
{
	class CM_D3D9_EXPORT D3D9DepthStencilBuffer : public DepthStencilBuffer
	{
	public:
		D3D9DepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);

		// TODO - Add D3D9 specific implementation
	};
}