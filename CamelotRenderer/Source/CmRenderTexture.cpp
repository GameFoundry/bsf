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

	void RenderTexture::initialize(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint,
		bool createDepth, DepthStencilFormat depthStencilFormat)
	{
		mPriority = CM_REND_TO_TEX_RT_GROUP;
		mWidth = width;
		mHeight = height;
		mType = textureType;
		mFormat = format;
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(format);
		mActive = true;
		mHwGamma = hwGamma;
		mFSAA = fsaa;
		mFSAAHint = fsaaHint;
		mDepthStencilFormat = depthStencilFormat;
		mFace = 0;
		mMipLevel = 0;

		createTextureBuffer();

		if(createDepth)
			createDepthStencilBuffer();

		createInternalResourcesImpl();
	}

	void RenderTexture::initialize(TexturePtr texture, DepthStencilBufferPtr depthStencilbuffer, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		setBuffers(texture, depthStencilbuffer, face, numFaces, mipLevel);
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

	void RenderTexture::createTextureBuffer()
	{
		mTexture = TextureManager::instance().createTexture(mType, mWidth, mHeight, 0, mFormat, TU_RENDERTARGET, mHwGamma, mFSAA, mFSAAHint);
	}

	void RenderTexture::createDepthStencilBuffer()
	{
		mDepthStencilBuffer = TextureManager::instance().createDepthStencilBuffer(mDepthStencilFormat, mWidth, mHeight, mFSAA, mFSAAHint);
	}

	void RenderTexture::setBuffers(TexturePtr texture, DepthStencilBufferPtr depthStencilBuffer, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		assert(texture != nullptr);
		assert(depthStencilBuffer != nullptr);

		mPriority = CM_REND_TO_TEX_RT_GROUP;

		if(texture->getWidth() != depthStencilBuffer->getWidth() ||
			texture->getHeight() != depthStencilBuffer->getWidth() ||
			texture->getFSAA() != depthStencilBuffer->getFsaa() ||
			texture->getFSAAHint() != depthStencilBuffer->getFsaaHint())
		{
			String errorInfo = "\nWidth: " + toString(texture->getWidth()) + "/" + toString(depthStencilBuffer->getWidth());
			errorInfo += "\nHeight: " + toString(texture->getHeight()) + "/" + toString(depthStencilBuffer->getHeight());
			errorInfo += "\nFSAA: " + toString(texture->getFSAA()) + "/" + toString(depthStencilBuffer->getFsaa());
			errorInfo += "\nFSAAHint: " + texture->getFSAAHint() + "/" + depthStencilBuffer->getFsaaHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}

		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();
		mType = texture->getTextureType();
		mFormat = texture->getFormat();
		mDepthStencilFormat = depthStencilBuffer->getFormat();
		mFace = face;
		mNumFaces = face;
		mMipLevel = mipLevel;

		mTexture = texture;
		mDepthStencilBuffer = depthStencilBuffer;

		createInternalResourcesImpl();
	}
	//-----------------------------------------------------------------------------
	MultiRenderTarget::MultiRenderTarget(const String &name)
    {
        mPriority = CM_REND_TO_TEX_RT_GROUP;
		mName = name;
		/// Width and height is unknown with no targets attached
		mWidth = mHeight = 0;
    }
	//-----------------------------------------------------------------------------
	void MultiRenderTarget::copyContentsToMemory(const PixelData &dst, FrameBuffer buffer)
	{
		CM_EXCEPT(InvalidParametersException, 
					"Cannot get MultiRenderTargets pixels");
	}
}
