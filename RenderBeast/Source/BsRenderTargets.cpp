#include "BsRenderTargets.h"
#include "BsRenderTexturePool.h"
#include "BsViewport.h"
#include "BsRenderAPI.h"
#include "BsTextureManager.h"

namespace BansheeEngine
{
	RenderTargets::RenderTargets(const ViewportCore& viewport, bool hdr, UINT32 numSamples)
		:mNumSamples(numSamples), mHDR(hdr)
	{
		// TODO - Round up width/height so it's divisible by 8?
		mWidth = (UINT32)viewport.getWidth();
		mHeight = (UINT32)viewport.getHeight();

		if (hdr)
			mDiffuseFormat = PF_FLOAT_R11G11B10;
		else
			mDiffuseFormat = PF_R8G8B8X8;

		mNormalFormat = PF_FLOAT_R11G11B10;
	}

	SPtr<RenderTargets> RenderTargets::create(const ViewportCore& viewport, bool hdr, UINT32 numSamples)
	{
		return bs_shared_ptr<RenderTargets>(new (bs_alloc<RenderTargets>()) RenderTargets(viewport, hdr, numSamples));
	}

	void RenderTargets::bind()
	{
		RenderTexturePool& texPool = RenderTexturePool::instance();

		mDiffuseRT = texPool.get(mDiffuseFormat, mWidth, mHeight, false, mNumSamples);
		mNormalRT = texPool.get(mNormalFormat, mWidth, mHeight, false, mNumSamples);
		mDepthRT = texPool.get(PF_D24S8, mWidth, mHeight, false, mNumSamples);

		// Note: I'm making an assumption here that textures retrieved from render texture pool
		// won't change, which should be true as long as I don't request these same sizes & formats
		// somewhere else at the same time while binding the gbuffer (which shouldn't happen).
		if (mGBuffer == nullptr)
		{
			MULTI_RENDER_TEXTURE_CORE_DESC gbufferDesc;
			gbufferDesc.colorSurfaces.resize(2);

			gbufferDesc.colorSurfaces[0].texture = mDiffuseRT->texture;
			gbufferDesc.colorSurfaces[0].face = 0;
			gbufferDesc.colorSurfaces[0].mipLevel = 0;

			gbufferDesc.colorSurfaces[1].texture = mNormalRT->texture;
			gbufferDesc.colorSurfaces[1].face = 0;
			gbufferDesc.colorSurfaces[1].mipLevel = 0;

			gbufferDesc.depthStencilSurface.texture = mDepthRT->texture;
			gbufferDesc.depthStencilSurface.face = 0;
			gbufferDesc.depthStencilSurface.mipLevel = 0;

			mGBuffer = TextureCoreManager::instance().createMultiRenderTexture(gbufferDesc);
		}

		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(mGBuffer);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);
	}

	void RenderTargets::unbind()
	{
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(nullptr);

		RenderTexturePool& texPool = RenderTexturePool::instance();

		texPool.release(mDiffuseRT);
		texPool.release(mNormalRT);
		texPool.release(mDepthRT);
	}
}