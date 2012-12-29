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
#include "CmHardwarePixelBuffer.h"
#include "CmTexture.h"
#include "CmTextureManager.h"
#include "CmDepthStencilBuffer.h"

namespace CamelotEngine
{
	RenderTexture::RenderTexture()
	{

	}

	void RenderTexture::createInternalResources()
	{
		if(mTexture->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((mFace + mNumFaces) >= mTexture->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(mFace + mNumFaces) + ". Max num faces: " + toString(mTexture->getNumFaces()));
		}

		if(mMipLevel >= mTexture->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(mMipLevel) + ". Max num mipmaps: " + toString(mTexture->getNumMipmaps()));
		}

		createInternalResourcesImpl();
	}

	void RenderTexture::setColorSurface(TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		mTexture = texture;

		if(mTexture == nullptr)
			return;

		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();
		mType = texture->getTextureType();
		mFormat = texture->getFormat();
		
		mFace = face;
		mNumFaces = face;
		mMipLevel = mipLevel;

		throwIfBuffersDontMatch();

		if(mDepthStencilBuffer != nullptr && mTexture != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::setDepthStencil(DepthStencilBufferPtr depthStencilBuffer)
	{
		mDepthStencilBuffer = depthStencilBuffer;

		if(mDepthStencilBuffer == nullptr)
			return;

		mDepthStencilFormat = depthStencilBuffer->getFormat();

		throwIfBuffersDontMatch();

		if(mDepthStencilBuffer != nullptr && mTexture != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::throwIfBuffersDontMatch() const
	{
		if(mTexture == nullptr || mDepthStencilBuffer == nullptr)
			return;

		if(mTexture->getWidth() != mDepthStencilBuffer->getWidth() ||
			mTexture->getHeight() != mDepthStencilBuffer->getWidth() ||
			mTexture->getFSAA() != mDepthStencilBuffer->getFsaa() ||
			mTexture->getFSAAHint() != mDepthStencilBuffer->getFsaaHint())
		{
			String errorInfo = "\nWidth: " + toString(mTexture->getWidth()) + "/" + toString(mDepthStencilBuffer->getWidth());
			errorInfo += "\nHeight: " + toString(mTexture->getHeight()) + "/" + toString(mDepthStencilBuffer->getHeight());
			errorInfo += "\nFSAA: " + toString(mTexture->getFSAA()) + "/" + toString(mDepthStencilBuffer->getFsaa());
			errorInfo += "\nFSAAHint: " + mTexture->getFSAAHint() + "/" + mDepthStencilBuffer->getFsaaHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void RenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}
