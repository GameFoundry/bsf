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
		assert(mSurface.texture != nullptr);

		if(mSurface.texture->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((mSurface.face + mSurface.numFaces) >= mSurface.texture->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(mSurface.face + mSurface.numFaces) + ". Max num faces: " + toString(mSurface.texture->getNumFaces()));
		}

		if(mSurface.mipLevel >= mSurface.texture->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(mSurface.mipLevel) + ". Max num mipmaps: " + toString(mSurface.texture->getNumMipmaps()));
		}

		createInternalResourcesImpl();
	}

	void RenderTexture::setColorSurface(TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		mSurface.texture = texture;

		if(mSurface.texture == nullptr)
			return;

		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();

		mSurface.format = texture->getFormat();
		
		mSurface.face = face;
		mSurface.numFaces = numFaces;
		mSurface.mipLevel = mipLevel;
		
		throwIfBuffersDontMatch();

		if(mDepthStencilBuffer != nullptr && mSurface.texture != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::setDepthStencil(DepthStencilBufferPtr depthStencilBuffer)
	{
		mDepthStencilBuffer = depthStencilBuffer;

		if(mDepthStencilBuffer == nullptr)
			return;

		throwIfBuffersDontMatch();

		if(mDepthStencilBuffer != nullptr && mSurface.texture != nullptr)
			createInternalResourcesImpl();
	}

	void RenderTexture::throwIfBuffersDontMatch() const
	{
		if(mSurface.texture == nullptr || mDepthStencilBuffer == nullptr)
			return;

		if(mSurface.texture->getWidth() != mDepthStencilBuffer->getWidth() ||
			mSurface.texture->getHeight() != mDepthStencilBuffer->getHeight() ||
			mSurface.texture->getFSAA() != mDepthStencilBuffer->getFsaa() ||
			mSurface.texture->getFSAAHint() != mDepthStencilBuffer->getFsaaHint())
		{
			String errorInfo = "\nWidth: " + toString(mSurface.texture->getWidth()) + "/" + toString(mDepthStencilBuffer->getWidth());
			errorInfo += "\nHeight: " + toString(mSurface.texture->getHeight()) + "/" + toString(mDepthStencilBuffer->getHeight());
			errorInfo += "\nFSAA: " + toString(mSurface.texture->getFSAA()) + "/" + toString(mDepthStencilBuffer->getFsaa());
			errorInfo += "\nFSAAHint: " + mSurface.texture->getFSAAHint() + "/" + mDepthStencilBuffer->getFsaaHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void RenderTexture::copyContentsToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}
