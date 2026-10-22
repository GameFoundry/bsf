//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DID3D12RenderWindowSurface.h"
#include "Image/B3DPixelUtility.h"

using namespace b3d;
using namespace b3d::render;

TOptional<RenderWindowImageLayout> ID3D12RenderWindowSurface::GetImageLayout() const
{
	if (GetCurrentColorImage() == nullptr)
		return {};

	RenderWindowImageLayout layout;
	layout.Width = GetWidth();
	layout.Height = GetHeight();
	layout.Format = GetColorPixelFormat();

	// Copies to buffers place rows at an aligned pitch
	const u32 blockSize = PixelUtility::GetBlockSize(layout.Format);
	const u32 rowPitchBytes = Math::CeilToMultiple(layout.Width * blockSize, (u32)D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	layout.Pitch = ImageSubresourcePitch(rowPitchBytes / blockSize, layout.Height);

	return layout;
}
