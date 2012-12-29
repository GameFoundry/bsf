#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmDepthStencilBuffer.h"

namespace CamelotEngine
{
	class CM_D3D9_EXPORT D3D9DepthStencilBuffer : public DepthStencilBuffer
	{
	public:
		D3D9DepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);
		~D3D9DepthStencilBuffer();

		IDirect3DSurface9* getSurface() const { return mSurface; }
	protected:
		IDirect3DSurface9* mSurface;
	};
}