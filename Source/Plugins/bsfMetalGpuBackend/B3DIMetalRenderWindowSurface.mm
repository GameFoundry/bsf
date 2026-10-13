//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DIMetalRenderWindowSurface.h"
#include "B3DMetalUtility.h"
#include "Image/B3DPixelUtility.h"

namespace b3d::render
{
	TOptional<RenderWindowImageLayout> IMetalRenderWindowSurface::GetImageLayout() const
	{
		id<MTLTexture> colorTexture = GetCurrentColorTexture();
		if (colorTexture == nil)
			return {};

		// Only development builds configure the CAMetalLayer with framebufferOnly=NO so the drawable texture can be used as a blit
		// source. Blitting from a framebuffer-only texture silently produces undefined bytes on Apple Silicon and fails Metal API
		// validation.
		if (colorTexture.framebufferOnly == YES)
			return {};

		RenderWindowImageLayout layout;
		layout.Width = (u32)colorTexture.width;
		layout.Height = (u32)colorTexture.height;
		layout.Format = GetColorPixelFormat();

		const u32 rowPitchBytes = MetalUtility::GetTextureRowPitch(layout.Format, layout.Width);
		layout.Pitch = ImageSubresourcePitch(rowPitchBytes / PixelUtility::GetBlockSize(layout.Format), layout.Height);

		return layout;
	}
} // namespace b3d::render
