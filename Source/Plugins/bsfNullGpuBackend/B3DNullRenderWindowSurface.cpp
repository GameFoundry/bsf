//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DNullRenderWindowSurface.h"

namespace b3d::render
{
	NullRenderWindowSurface::NullRenderWindowSurface(const RenderWindowSurfaceCreateInformation& createInformation)
		: mWidth(createInformation.Width), mHeight(createInformation.Height)
	{
	}

	NullRenderWindowSurface::~NullRenderWindowSurface()
	{
		// No-op destructor
	}

	void NullRenderWindowSurface::RebuildSwapChain(u32 width, u32 height, bool vsync, u32 vsyncInterval)
	{
		// No-op implementation
	}

	void NullRenderWindowSurface::MarkSwapChainAsInvalid()
	{
		// No-op implementation
	}

	TOptional<RenderWindowImageLayout> NullRenderWindowSurface::GetImageLayout() const
	{
		RenderWindowImageLayout layout;
		layout.Width = mWidth;
		layout.Height = mHeight;
		layout.Format = PF_RGBA8;
		layout.Pitch = ImageSubresourcePitch(mWidth, mHeight);

		return layout;
	}

	void NullRenderWindowSurface::Destroy()
	{
		// No-op implementation
	}
} // namespace b3d::render
