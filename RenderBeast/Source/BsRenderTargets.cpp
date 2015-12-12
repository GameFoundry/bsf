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

		if (mGBufferRT == nullptr || rebuildTargets)
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
		}
	}

	void RenderTargets::release()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(nullptr);

		RenderTexturePool& texPool = RenderTexturePool::instance();

		texPool.release(mAlbedoTex);
		texPool.release(mNormalTex);
		texPool.release(mDepthTex);
	}

	void RenderTargets::bind()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(mGBufferRT);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);
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