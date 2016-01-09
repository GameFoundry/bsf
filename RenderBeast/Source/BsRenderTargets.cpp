#include "BsRenderTargets.h"
#include "BsRenderTexturePool.h"
#include "BsViewport.h"
#include "BsRenderAPI.h"
#include "BsTextureManager.h"

namespace BansheeEngine
{
	RenderTargets::RenderTargets(const SPtr<ViewportCore>& viewport, bool hdr, UINT32 numSamples)
		:mNumSamples(numSamples), mHDR(hdr), mViewport(viewport)
	{
		if (hdr)
			mDiffuseFormat = PF_FLOAT_R11G11B10;
		else
			mDiffuseFormat = PF_B8G8R8X8;

		mNormalFormat = PF_UNORM_R10G10B10A2;
	}

	SPtr<RenderTargets> RenderTargets::create(const SPtr<ViewportCore>& viewport, bool hdr, UINT32 numSamples)
	{
		return bs_shared_ptr<RenderTargets>(new (bs_alloc<RenderTargets>()) RenderTargets(viewport, hdr, numSamples));
	}

	void RenderTargets::allocate()
	{
		RenderTexturePool& texPool = RenderTexturePool::instance();

		UINT32 width = getWidth();
		UINT32 height = getHeight();

		SPtr<PooledRenderTexture> newAlbedoRT = texPool.get(mDiffuseFormat, width, height, false, mNumSamples);
		SPtr<PooledRenderTexture> newNormalRT = texPool.get(mNormalFormat, width, height, false, mNumSamples);
		SPtr<PooledRenderTexture> newDepthRT = texPool.get(PF_D24S8, width, height, false, mNumSamples);

		SPtr<PooledRenderTexture> newColorRT = nullptr;

		// See if I can use the final output color target for gbuffer rendering, this saves a little memory
		SPtr<RenderTargetCore> resolvedRT = mViewport->getTarget();
		const RenderTargetProperties& resolvedRTProps = resolvedRT->getProperties();

		bool useResolvedColor = !resolvedRTProps.isWindow() &&
			mViewport->getWidth() == getWidth() &&
			mViewport->getHeight() == getHeight() &&
			((resolvedRTProps.getMultisampleCount() <= 1) == (mNumSamples <= 1) ||
			resolvedRTProps.getMultisampleCount() == mNumSamples);

		if (!useResolvedColor)
			newColorRT = texPool.get(PF_B8G8R8X8, width, height, false, mNumSamples);

		bool rebuildTargets = newColorRT != mSceneColorTex || newAlbedoRT != mAlbedoTex || newNormalRT != mNormalTex || newDepthRT != mDepthTex;

		mSceneColorTex = newColorRT;
		mAlbedoTex = newAlbedoRT;
		mNormalTex = newNormalRT;
		mDepthTex = newDepthRT;

		if (mGBufferRT == nullptr || mSceneColorRT == nullptr || rebuildTargets)
		{
			MULTI_RENDER_TEXTURE_CORE_DESC gbufferDesc;
			gbufferDesc.colorSurfaces.resize(3);

			SPtr<TextureCore> sceneColorTex = nullptr;

			if (newColorRT != nullptr)
				sceneColorTex = newColorRT->texture;
			else // Re-using output scene color texture
			{
				SPtr<RenderTextureCore> resolvedRTex = std::static_pointer_cast<RenderTextureCore>(resolvedRT);
				sceneColorTex = resolvedRTex->getBindableColorTexture();
			}

			gbufferDesc.colorSurfaces[0].texture = sceneColorTex;
			gbufferDesc.colorSurfaces[0].face = 0;
			gbufferDesc.colorSurfaces[0].mipLevel = 0;

			gbufferDesc.colorSurfaces[1].texture = mAlbedoTex->texture;
			gbufferDesc.colorSurfaces[1].face = 0;
			gbufferDesc.colorSurfaces[1].mipLevel = 0;

			gbufferDesc.colorSurfaces[2].texture = mNormalTex->texture;
			gbufferDesc.colorSurfaces[2].face = 0;
			gbufferDesc.colorSurfaces[2].mipLevel = 0;

			gbufferDesc.depthStencilSurface.texture = mDepthTex->texture;
			gbufferDesc.depthStencilSurface.face = 0;
			gbufferDesc.depthStencilSurface.mipLevel = 0;

			mGBufferRT = TextureCoreManager::instance().createMultiRenderTexture(gbufferDesc);

			RENDER_TEXTURE_CORE_DESC sceneColorDesc;
			sceneColorDesc.colorSurface.texture = sceneColorTex;
			sceneColorDesc.colorSurface.face = 0;
			sceneColorDesc.colorSurface.mipLevel = 0;

			sceneColorDesc.depthStencilSurface.texture = mDepthTex->texture;
			sceneColorDesc.depthStencilSurface.face = 0;
			sceneColorDesc.depthStencilSurface.mipLevel = 0;

			mSceneColorRT = TextureCoreManager::instance().createRenderTexture(sceneColorDesc);
		}
	}

	void RenderTargets::release()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(nullptr);

		RenderTexturePool& texPool = RenderTexturePool::instance();

		if (mSceneColorTex != nullptr)
			texPool.release(mSceneColorTex);

		texPool.release(mAlbedoTex);
		texPool.release(mNormalTex);
		texPool.release(mDepthTex);
	}

	void RenderTargets::bindGBuffer()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(mGBufferRT);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);

		UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
		RenderAPICore::instance().clearViewport(clearBuffers, Color::ZERO, 1.0f, 0);
	}

	void RenderTargets::bindSceneColor()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(mSceneColorRT, true);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);
	}

	void RenderTargets::resolve()
	{
		// Prepare final render target
		SPtr<RenderTargetCore> target = mViewport->getTarget();

		RenderAPICore::instance().setRenderTarget(target);
		RenderAPICore::instance().setViewport(mViewport->getNormArea());

		// If using a separate scene color texture clear the final render target
		if (mSceneColorTex == nullptr)
		{
			// Do nothing as final render target is already our scene color target
		}
		else
		{
			UINT32 clearBuffers = 0;
			if (mViewport->getRequiresColorClear())
				clearBuffers |= FBT_COLOR;

			if (mViewport->getRequiresDepthClear())
				clearBuffers |= FBT_DEPTH;

			if (mViewport->getRequiresStencilClear())
				clearBuffers |= FBT_STENCIL;

			if (clearBuffers != 0)
			{
				RenderAPICore::instance().clearViewport(clearBuffers, mViewport->getClearColor(),
					mViewport->getClearDepthValue(), mViewport->getClearStencilValue());
			}

			// TODO - Copy from internal scene color to final scene color
		}
	}

	SPtr<TextureCore> RenderTargets::getTextureA() const
	{
		return mAlbedoTex->texture;
	}

	SPtr<TextureCore> RenderTargets::getTextureB() const
	{
		return mNormalTex->texture;
	}

	SPtr<TextureCore> RenderTargets::getTextureDepth() const
	{
		return mDepthTex->texture;
	}

	UINT32 RenderTargets::getWidth() const
	{
		return (UINT32)mViewport->getWidth();
	}

	UINT32 RenderTargets::getHeight() const
	{
		return (UINT32)mViewport->getHeight();
	}
}