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
#include "CmResources.h"

namespace BansheeEngine
{
	RenderTexture::RenderTexture()
		:mColorSurface(nullptr), mDepthStencilSurface(nullptr)
	{

	}

	RenderTexture::~RenderTexture()
	{

	}

	RenderTexturePtr RenderTexture::create(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint, 
		bool createDepth, PixelFormat depthStencilFormat)
	{
		return TextureManager::instance().createRenderTexture(textureType, width, height, format, hwGamma, fsaa, fsaaHint, createDepth, depthStencilFormat);
	}

	void RenderTexture::destroy_internal()
	{
		if(mColorSurface != nullptr)
			Texture::releaseView(mColorSurface);

		if(mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTarget::destroy_internal();
	}

	void RenderTexture::initialize(const RENDER_TEXTURE_DESC& desc)
	{
		if(desc.colorSurface.texture != nullptr)
		{
			TexturePtr texture = desc.colorSurface.texture;

			if(texture->getUsage() != TU_RENDERTARGET)
				CM_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

			mColorSurface = Texture::requestView(texture, desc.colorSurface.mipLevel, 1, 
				desc.colorSurface.face, desc.colorSurface.numFaces, GVU_RENDERTARGET);

			mWidth = texture->getWidth();
			mHeight = texture->getHeight();
			mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
			mActive = true;
			mHwGamma = texture->isHardwareGammaEnabled();
			mFSAA = texture->getFSAA();
			mFSAAHint = texture->getFSAAHint();
		}

		if(desc.depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = desc.depthStencilSurface.texture;

			if(texture->getUsage() != TU_DEPTHSTENCIL)
				CM_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, desc.depthStencilSurface.mipLevel, 1, 
				desc.depthStencilSurface.face, desc.depthStencilSurface.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();

		assert(mColorSurface != nullptr);
		assert(mColorSurface->getTexture() != nullptr);

		if(mColorSurface->getTexture()->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) > mColorSurface->getTexture()->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) + 
				". Max num faces: " + toString(mColorSurface->getTexture()->getNumFaces()));
		}

		if(mColorSurface->getMostDetailedMip() > mColorSurface->getTexture()->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(mColorSurface->getMostDetailedMip()) + ". Max num mipmaps: " + toString(mColorSurface->getTexture()->getNumMipmaps()));
		}

		RenderTarget::initialize();

		// Create non-persistent resource handles for the used textures (we only need them because a lot of the code accepts only handles,
		// since they're non persistent they don't really have any benefit over shared pointers)
		if(mColorSurface != nullptr)
			mBindableColorTex = gResources().createResourceHandle(mColorSurface->getTexture());

		if(mDepthStencilSurface != nullptr)
			mBindableDepthStencilTex = gResources().createResourceHandle(mDepthStencilSurface->getTexture());
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

	void RenderTexture::copyToMemory( const PixelData &dst, FrameBuffer buffer /*= FB_AUTO */ )
	{
		throw std::exception("The method or operation is not implemented.");
	}
}
