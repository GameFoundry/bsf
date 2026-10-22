//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DIVulkanRenderWindowSurface.h"

using namespace b3d;
using namespace b3d::render;

TOptional<RenderWindowImageLayout> IVulkanRenderWindowSurface::GetImageLayout() const
{
	if(!SupportsReadback() || GetCurrentColorImage() == nullptr)
		return {};

	RenderWindowImageLayout layout;
	layout.Width = GetWidth();
	layout.Height = GetHeight();
	layout.Format = GetColorPixelFormat();
	layout.Pitch = ImageSubresourcePitch(layout.Width, layout.Height);

	return layout;
}
