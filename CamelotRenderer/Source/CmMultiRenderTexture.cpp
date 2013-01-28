#include "CmMultiRenderTexture.h"
#include "CmTexture.h"
#include "CmException.h"

namespace CamelotEngine
{
	MultiRenderTexture::MultiRenderTexture()
	{
		mColorSurfaces.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}

	MultiRenderTexture::~MultiRenderTexture()
	{

	}

	void MultiRenderTexture::destroy_internal()
	{
		for(auto iter = mColorSurfaces.begin(); iter != mColorSurfaces.end(); ++iter)
		{
			if(*iter != nullptr)
				Texture::releaseView(*iter);
		}

		if(mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTarget::destroy_internal();
	}

	void MultiRenderTexture::setColorSurface(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(surfaceIdx < 0 || surfaceIdx >= CM_MAX_MULTIPLE_RENDER_TARGETS)
			CM_EXCEPT(InvalidParametersException, "Invalid surface index. 0 <= " + toString(surfaceIdx) + " < CM_MAX_MULTIPLE_RENDER_TARGETS");

		if(texture != nullptr && texture->getUsage() != TU_RENDERTARGET)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

		if(mColorSurfaces[surfaceIdx] != nullptr)
		{
			mColorSurfaces[surfaceIdx]->getTexture()->releaseView(mColorSurfaces[surfaceIdx]);
			mColorSurfaces[surfaceIdx] = nullptr;
		}

		if(texture == nullptr)
		{
			setColorSurfaceImpl(surfaceIdx, texture, face, numFaces, mipLevel);
			return;
		}

		mColorSurfaces[surfaceIdx] = Texture::requestView(texture, mipLevel, 1, face, numFaces, GVU_RENDERTARGET);

		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();

		throwIfBuffersDontMatch();

		setColorSurfaceImpl(surfaceIdx, texture, face, numFaces, mipLevel);
	}

	void MultiRenderTexture::setDepthStencilSurface(TexturePtr depthStencil, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(depthStencil != nullptr && depthStencil->getUsage() != TU_DEPTHSTENCIL)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

		if(mDepthStencilSurface != nullptr)
		{
			Texture::releaseView(mDepthStencilSurface);
			mDepthStencilSurface = nullptr;
		}

		if(depthStencil == nullptr)
			return;

		mDepthStencilSurface = Texture::requestView(depthStencil, mipLevel, 1, face, numFaces, GVU_DEPTHSTENCIL);

		throwIfBuffersDontMatch();

		setDepthStencilImpl(depthStencil, face, numFaces, mipLevel);
	}

	void MultiRenderTexture::throwIfBuffersDontMatch() const
	{
		const TextureView* firstSurfaceDesc = nullptr;
		for(size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if(mColorSurfaces[i] == nullptr)
				continue;

			if(firstSurfaceDesc == nullptr)
			{
				firstSurfaceDesc = mColorSurfaces[i];
				continue;
			}

			if(mColorSurfaces[i]->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
				mColorSurfaces[i]->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
				mColorSurfaces[i]->getTexture()->getFSAA() != firstSurfaceDesc->getTexture()->getFSAA() ||
				mColorSurfaces[i]->getTexture()->getFSAAHint() != firstSurfaceDesc->getTexture()->getFSAAHint())
			{
				String errorInfo = "\nWidth: " + toString(mColorSurfaces[i]->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
				errorInfo += "\nHeight: " + toString(mColorSurfaces[i]->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
				errorInfo += "\nFSAA: " + toString(mColorSurfaces[i]->getTexture()->getFSAA()) + "/" + toString(firstSurfaceDesc->getTexture()->getFSAA());
				errorInfo += "\nFSAAHint: " + mColorSurfaces[i]->getTexture()->getFSAAHint() + "/" + firstSurfaceDesc->getTexture()->getFSAAHint();

				CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
			}
		}

		if(firstSurfaceDesc == nullptr)
			return;

		if(firstSurfaceDesc->getTexture()->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) >= firstSurfaceDesc->getTexture()->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) + ". Max num faces: " + toString(firstSurfaceDesc->getTexture()->getNumFaces()));
		}

		if(firstSurfaceDesc->getMostDetailedMip() >= firstSurfaceDesc->getTexture()->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(firstSurfaceDesc->getMostDetailedMip()) + ". Max num mipmaps: " + toString(firstSurfaceDesc->getTexture()->getNumMipmaps()));
		}

		if(mDepthStencilSurface == nullptr)
			return;

		if(mDepthStencilSurface->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
			mDepthStencilSurface->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
			mDepthStencilSurface->getTexture()->getFSAA() != firstSurfaceDesc->getTexture()->getFSAA() ||
			mDepthStencilSurface->getTexture()->getFSAAHint() != firstSurfaceDesc->getTexture()->getFSAAHint())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilSurface->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilSurface->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
			errorInfo += "\nFSAA: " + toString(mDepthStencilSurface->getTexture()->getFSAA()) + "/" + toString(firstSurfaceDesc->getTexture()->getFSAA());
			errorInfo += "\nFSAAHint: " + mDepthStencilSurface->getTexture()->getFSAAHint() + "/" + firstSurfaceDesc->getTexture()->getFSAAHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}