//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DMetalHeadlessRenderWindowSurface.h"
#include "B3DMetalGpuDevice.h"
#include "B3DMetalTexture.h"
#include "Debug/B3DLog.h"

namespace b3d::render
{
	MetalHeadlessRenderWindowSurface::MetalHeadlessRenderWindowSurface(MetalGpuDevice& device, const RenderWindowSurfaceCreateInformation& createInformation)
		: mGpuDevice(device), mWidth(createInformation.Width), mHeight(createInformation.Height), mVSync(createInformation.VSync), mCreateDepthBuffer(createInformation.CreateDepthBuffer), mUseHardwareSRGB(createInformation.UseHardwareSRGB)
	{
		CreateSwapChainImages();
	}

	MetalHeadlessRenderWindowSurface::~MetalHeadlessRenderWindowSurface()
	{
		Destroy();
	}

	void MetalHeadlessRenderWindowSurface::CreateSwapChainImages()
	{
		if (mWidth == 0 || mHeight == 0)
		{
			B3D_LOG(Error, LogRenderBackend, "Headless render window surface created with zero size ({0}x{1}).", mWidth, mHeight);
			mIsValid = false;

			return;
		}

		// BGRA8 matches the format the windowed surface's CAMetalLayer would use, so headless and windowed
		// rendering produce byte-identical readbacks (GetColorPixelFormat() returns PF_BGRA8 for both).
		const MTLPixelFormat colorFormat = mUseHardwareSRGB ? MTLPixelFormatBGRA8Unorm_sRGB : MTLPixelFormatBGRA8Unorm;
		for (u32 imageIndex = 0; imageIndex < kImageCount; imageIndex++)
		{
			const String name = "HeadlessSwapChainColor" + ToString(imageIndex);
			mColorImages[imageIndex] = CreateSurfaceImage(mGpuDevice, colorFormat, PF_BGRA8, TextureUsageFlag::RenderTarget, mWidth, mHeight, name);
			if (mColorImages[imageIndex] == nullptr)
			{
				DestroySwapChainImages();
				mIsValid = false;

				return;
			}
		}

		if (mCreateDepthBuffer)
		{
			mDepthStencilImage = CreateSurfaceImage(mGpuDevice, MTLPixelFormatDepth32Float_Stencil8, PF_D32_S8X24, TextureUsageFlag::DepthStencil, mWidth, mHeight, "HeadlessSwapChainDepthStencil");
			if (mDepthStencilImage == nullptr)
			{
				DestroySwapChainImages();
				mIsValid = false;

				return;
			}
		}

		mCurrentImageIndex = 0;
		mIsValid = true;
	}

	void MetalHeadlessRenderWindowSurface::DestroySwapChainImages()
	{
		for (MetalImage*& image : mColorImages)
		{
			if (image != nullptr)
			{
				image->Destroy();
				image = nullptr;
			}
		}

		if (mDepthStencilImage != nullptr)
		{
			mDepthStencilImage->Destroy();
			mDepthStencilImage = nullptr;
		}
	}

	id<MTLTexture> MetalHeadlessRenderWindowSurface::GetCurrentColorTexture() const
	{
		MetalImage* image = mColorImages[mCurrentImageIndex];
		return image != nullptr ? image->GetMetalHandle() : nil;
	}

	id<MTLTexture> MetalHeadlessRenderWindowSurface::GetDepthStencilTexture() const
	{
		return mDepthStencilImage != nullptr ? mDepthStencilImage->GetMetalHandle() : nil;
	}

	id<MTLTexture> MetalHeadlessRenderWindowSurface::AcquireColorTexture()
	{
		// Even when flagged invalid (pending rebuild) the current image remains usable — rendering at a stale size
		// is preferable to dropping the frame; the rebuild lands before the next acquire.
		if (mIsDestroyed)
			return nil;

		mHasAcquiredImage = true;
		return GetCurrentColorTexture();
	}

	MTLPixelFormat MetalHeadlessRenderWindowSurface::GetColorFormat() const
	{
		return mUseHardwareSRGB ? MTLPixelFormatBGRA8Unorm_sRGB : MTLPixelFormatBGRA8Unorm;
	}

	void MetalHeadlessRenderWindowSurface::SwapBuffers(GpuQueue& queue, GpuQueueMask syncMask)
	{
		// Like a swap chain, only an image that was acquired for rendering is presented
		if (mIsDestroyed || !mIsValid || !mHasAcquiredImage)
			return;

		mCurrentImageIndex = (mCurrentImageIndex + 1) % kImageCount;
		mHasAcquiredImage = false;
	}

	void MetalHeadlessRenderWindowSurface::RebuildSwapChain(u32 width, u32 height, bool vsync, u32 vsyncInterval)
	{
		if (mIsDestroyed)
			return;

		if (mIsValid && width == mWidth && height == mHeight && vsync == mVSync)
			return;

		mWidth = width;
		mHeight = height;
		mVSync = vsync;
		mHasAcquiredImage = false;

		DestroySwapChainImages();
		CreateSwapChainImages();
	}

	void MetalHeadlessRenderWindowSurface::MarkSwapChainAsInvalid()
	{
		mIsValid = false;
	}

	void MetalHeadlessRenderWindowSurface::Destroy()
	{
		if (mIsDestroyed)
			return;

		mIsDestroyed = true;
		mIsValid = false;
		DestroySwapChainImages();
	}
} // namespace b3d::render
