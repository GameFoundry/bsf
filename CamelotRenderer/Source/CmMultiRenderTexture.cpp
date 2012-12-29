#include "CmMultiRenderTexture.h"
#include "CmTexture.h"
#include "CmDepthStencilBuffer.h"
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

	void MultiRenderTexture::setDepthStencil(DepthStencilBufferPtr depthStencilBuffer)
	{
		mDepthStencilBuffer = depthStencilBuffer;

		throwIfBuffersDontMatch();

		setDepthStencilImpl(depthStencilBuffer);
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

		if(mDepthStencilBuffer == nullptr)
			return;

		if(mDepthStencilBuffer->getWidth() != firstSurfaceDesc->texture->getWidth() ||
			mDepthStencilBuffer->getHeight() != firstSurfaceDesc->texture->getHeight() ||
			mDepthStencilBuffer->getFsaa() != firstSurfaceDesc->texture->getFSAA() ||
			mDepthStencilBuffer->getFsaaHint() != firstSurfaceDesc->texture->getFSAAHint())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilBuffer->getWidth()) + "/" + toString(firstSurfaceDesc->texture->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilBuffer->getHeight()) + "/" + toString(firstSurfaceDesc->texture->getHeight());
			errorInfo += "\nFSAA: " + toString(mDepthStencilBuffer->getFsaa()) + "/" + toString(firstSurfaceDesc->texture->getFSAA());
			errorInfo += "\nFSAAHint: " + mDepthStencilBuffer->getFsaaHint() + "/" + firstSurfaceDesc->texture->getFSAAHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}