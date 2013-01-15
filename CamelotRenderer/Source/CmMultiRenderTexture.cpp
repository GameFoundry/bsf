#include "CmMultiRenderTexture.h"
#include "CmTexture.h"
#include "CmException.h"

namespace CamelotEngine
{
	MultiRenderTexture::MultiRenderTexture()
	{
		mSurfaces.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}

	void MultiRenderTexture::setColorSurface(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(surfaceIdx < 0 || surfaceIdx >= CM_MAX_MULTIPLE_RENDER_TARGETS)
			CM_EXCEPT(InvalidParametersException, "Invalid surface index. 0 <= " + toString(surfaceIdx) + " < CM_MAX_MULTIPLE_RENDER_TARGETS");

		if(texture != nullptr && texture->getUsage() != TU_RENDERTARGET)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

		mSurfaces[surfaceIdx].texture = texture;

		if(texture == nullptr)
		{
			setColorSurfaceImpl(surfaceIdx, texture, face, numFaces, mipLevel);
			return;
		}
			
		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();

		mSurfaces[surfaceIdx].format = texture->getFormat();

		mSurfaces[surfaceIdx].face = face;
		mSurfaces[surfaceIdx].numFaces = numFaces;
		mSurfaces[surfaceIdx].mipLevel = mipLevel;

		throwIfBuffersDontMatch();

		setColorSurfaceImpl(surfaceIdx, texture, face, numFaces, mipLevel);
	}

	void MultiRenderTexture::setDepthStencilSurface(TexturePtr depthStencilSurface)
	{
		if(depthStencilSurface != nullptr && depthStencilSurface->getUsage() != TU_DEPTHSTENCIL)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

		mDepthStencilSurface = depthStencilSurface;

		throwIfBuffersDontMatch();

		setDepthStencilImpl(depthStencilSurface);
	}

	void MultiRenderTexture::throwIfBuffersDontMatch() const
	{
		const SurfaceDesc* firstSurfaceDesc = nullptr;
		for(size_t i = 0; i < mSurfaces.size(); i++)
		{
			if(mSurfaces[i].texture == nullptr)
				continue;

			if(firstSurfaceDesc == nullptr)
			{
				firstSurfaceDesc = &mSurfaces[i];
				continue;
			}

			if(mSurfaces[i].texture->getWidth() != firstSurfaceDesc->texture->getWidth() ||
				mSurfaces[i].texture->getHeight() != firstSurfaceDesc->texture->getHeight() ||
				mSurfaces[i].texture->getFSAA() != firstSurfaceDesc->texture->getFSAA() ||
				mSurfaces[i].texture->getFSAAHint() != firstSurfaceDesc->texture->getFSAAHint())
			{
				String errorInfo = "\nWidth: " + toString(mSurfaces[i].texture->getWidth()) + "/" + toString(firstSurfaceDesc->texture->getWidth());
				errorInfo += "\nHeight: " + toString(mSurfaces[i].texture->getHeight()) + "/" + toString(firstSurfaceDesc->texture->getHeight());
				errorInfo += "\nFSAA: " + toString(mSurfaces[i].texture->getFSAA()) + "/" + toString(firstSurfaceDesc->texture->getFSAA());
				errorInfo += "\nFSAAHint: " + mSurfaces[i].texture->getFSAAHint() + "/" + firstSurfaceDesc->texture->getFSAAHint();

				CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
			}
		}

		if(firstSurfaceDesc == nullptr)
			return;

		if(firstSurfaceDesc->texture->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((firstSurfaceDesc->face + firstSurfaceDesc->numFaces) >= firstSurfaceDesc->texture->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(firstSurfaceDesc->face + firstSurfaceDesc->numFaces) + ". Max num faces: " + toString(firstSurfaceDesc->texture->getNumFaces()));
		}

		if(firstSurfaceDesc->mipLevel >= firstSurfaceDesc->texture->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(firstSurfaceDesc->mipLevel) + ". Max num mipmaps: " + toString(firstSurfaceDesc->texture->getNumMipmaps()));
		}

		if(mDepthStencilSurface == nullptr)
			return;

		if(mDepthStencilSurface->getWidth() != firstSurfaceDesc->texture->getWidth() ||
			mDepthStencilSurface->getHeight() != firstSurfaceDesc->texture->getHeight() ||
			mDepthStencilSurface->getFSAA() != firstSurfaceDesc->texture->getFSAA() ||
			mDepthStencilSurface->getFSAAHint() != firstSurfaceDesc->texture->getFSAAHint())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilSurface->getWidth()) + "/" + toString(firstSurfaceDesc->texture->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilSurface->getHeight()) + "/" + toString(firstSurfaceDesc->texture->getHeight());
			errorInfo += "\nFSAA: " + toString(mDepthStencilSurface->getFSAA()) + "/" + toString(firstSurfaceDesc->texture->getFSAA());
			errorInfo += "\nFSAAHint: " + mDepthStencilSurface->getFSAAHint() + "/" + firstSurfaceDesc->texture->getFSAAHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}