/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "CmRenderTexture.h"
#include "CmException.h"
#include "CmPixelBuffer.h"
#include "CmTexture.h"
#include "CmTextureManager.h"

namespace CamelotEngine
{
	RenderTexture::RenderTexture()
		:mColorSurface(nullptr), mDepthStencilSurface(nullptr)
	{

	}

	RenderTexture::~RenderTexture()
	{
		if(mColorSurface != nullptr)
			Texture::releaseView(mColorSurface);

		if(mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);
	}

	void RenderTexture::createInternalResources()
	{
		assert(mColorSurface != nullptr);
		assert(mColorSurface->getTexture() != nullptr);

		if(mColorSurface->getTexture()->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) >= mColorSurface->getTexture()->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) + 
				". Max num faces: " + toString(mColorSurface->getTexture()->getNumFaces()));
		}

		if(mColorSurface->getMostDetailedMip() >= mColorSurface->getTexture()->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(mColorSurface->getMostDetailedMip()) + ". Max num mipmaps: " + toString(mColorSurface->getTexture()->getNumMipmaps()));
		}

		createInternalResourcesImpl();
	}

	void RenderTexture::setColorSurface(TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(texture != nullptr && texture->getUsage() != TU_RENDERTARGET)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

		if(mColorSurface != nullptr)
		{
			mColorSurface->getTexture()->releaseView(mColorSurface);
			mColorSurface = nullptr;
		}

		if(texture == nullptr)
			return;

		mColorSurface = Texture::requestView(texture, mipLevel, 1, face, numFaces, GVU_RENDERTARGET);

		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();
		
		throwIfBuffersDontMatch();

		if(mDepthStencilSurface != nullptr && mColorSurface != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::setDepthStencilSurface(TexturePtr depthStencilSurface, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(depthStencilSurface != nullptr && depthStencilSurface->getUsage() != TU_DEPTHSTENCIL)
			CM_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

		if(mDepthStencilSurface != nullptr)
		{
			mDepthStencilSurface->getTexture()->releaseView(mDepthStencilSurface);
			mDepthStencilSurface = nullptr;
		}

		if(depthStencilSurface == nullptr)
			return;

		mDepthStencilSurface = Texture::requestView(depthStencilSurface, mipLevel, 1, face, numFaces, GVU_DEPTHSTENCIL);

		throwIfBuffersDontMatch();

		if(mDepthStencilSurface != nullptr && mColorSurface != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::throwIfBuffersDontMatch() const
	{
		if(mColorSurface == nullptr || mDepthStencilSurface == nullptr)
			return;

		if(mColorSurface->getTexture()->getWidth() != mDepthStencilSurface->getTexture()->getWidth() ||
			mColorSurface->getTexture()->getHeight() != mDepthStencilSurface->getTexture()->getHeight() ||
			mColorSurface->getTexture()->getFSAA() != mDepthStencilSurface->getTexture()->getFSAA() ||
			mColorSurface->getTexture()->getFSAAHint() != mDepthStencilSurface->getTexture()->getFSAAHint())
		{
			String errorInfo = "\nWidth: " + toString(mColorSurface->getTexture()->getWidth()) + "/" + toString(mDepthStencilSurface->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mColorSurface->getTexture()->getHeight()) + "/" + toString(mDepthStencilSurface->getTexture()->getHeight());
			errorInfo += "\nFSAA: " + toString(mColorSurface->getTexture()->getFSAA()) + "/" + toString(mDepthStencilSurface->getTexture()->getFSAA());
			errorInfo += "\nFSAAHint: " + mColorSurface->getTexture()->getFSAAHint() + "/" + mDepthStencilSurface->getTexture()->getFSAAHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void RenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}
