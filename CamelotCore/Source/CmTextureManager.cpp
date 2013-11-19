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
#include "CmTextureManager.h"
#include "CmException.h"
#include "CmPixelUtil.h"
#include "CmMultiRenderTexture.h"
#include "CmRenderSystem.h"

namespace CamelotFramework 
{
    TextureManager::TextureManager()
    {
        // Subclasses should register (when this is fully constructed)
    }

    TextureManager::~TextureManager()
    {
        // subclasses should unregister with resource group manager
    }

	void TextureManager::onStartUp()
	{
		// Internally this will call our createTextureImpl virtual method. But since this is guaranteed
		// to be the last class in the hierarchy, we can call a virtual method from constructor.
		mDummyTexture = Texture::create(TEX_TYPE_2D, 2, 2, 0, PF_R8G8B8A8);

		UINT32 subresourceIdx = mDummyTexture->mapToSubresourceIdx(0, 0);
		PixelDataPtr data = mDummyTexture->allocateSubresourceBuffer(subresourceIdx);

		data->setColorAt(Color::Red, 0, 0);
		data->setColorAt(Color::Red, 0, 1);
		data->setColorAt(Color::Red, 1, 0);
		data->setColorAt(Color::Red, 1, 1);

		AsyncOp op;
		RenderSystem::instance().writeSubresource(mDummyTexture.getInternalPtr(), mDummyTexture->mapToSubresourceIdx(0, 0), data, true, op);
	}

    TexturePtr TextureManager::createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, int numMipmaps,
        PixelFormat format, int usage, bool hwGamma, 
		UINT32 fsaa, const String& fsaaHint)
    {
        TexturePtr ret = createTextureImpl();
		ret->setThisPtr(ret);
		ret->initialize(texType, width, height, depth, static_cast<size_t>(numMipmaps), format, usage, hwGamma, fsaa, fsaaHint);

		return ret;
    }

	TexturePtr TextureManager::createEmpty()
	{
		TexturePtr texture = createTextureImpl();
		texture->setThisPtr(texture);

		return texture;
	}

	RenderTexturePtr TextureManager::createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint, 
			bool createDepth, PixelFormat depthStencilFormat)
	{
		TexturePtr texture = createTexture(textureType, width, height, 0, format, TU_RENDERTARGET, hwGamma, fsaa, fsaaHint);

		TexturePtr depthStencil = nullptr;
		if(createDepth)
		{
			depthStencil = createTexture(TEX_TYPE_2D, width, height, 0, depthStencilFormat, TU_DEPTHSTENCIL, false, fsaa, fsaaHint);
		}

		RENDER_TEXTURE_DESC desc;
		desc.colorSurface.texture = texture;
		desc.colorSurface.face = 0;
		desc.colorSurface.mipLevel = 0;
		desc.colorSurface.numFaces = 1;

		desc.depthStencilSurface.texture = depthStencil;
		desc.depthStencilSurface.face = 0;
		desc.depthStencilSurface.mipLevel = 0;
		desc.depthStencilSurface.numFaces = 1;

		RenderTexturePtr newRT = createRenderTexture(desc);

		return newRT;
	}

	RenderTexturePtr TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC& desc)
	{
		RenderTexturePtr newRT = createRenderTextureImpl();
		newRT->setThisPtr(newRT);
		newRT->initialize(desc);

		return newRT;
	}

	MultiRenderTexturePtr TextureManager::createEmptyMultiRenderTexture()
	{
		MultiRenderTexturePtr newRT = createMultiRenderTextureImpl();
		newRT->setThisPtr(newRT);

		return newRT;
	}

	bool TextureManager::isFormatSupported(TextureType ttype, PixelFormat format, int usage)
	{
		return getNativeFormat(ttype, format, usage) == format;
	}

	bool TextureManager::isEquivalentFormatSupported(TextureType ttype, PixelFormat format, int usage)
	{
		PixelFormat supportedFormat = getNativeFormat(ttype, format, usage);

		// Assume that same or greater number of bits means quality not degraded
		return PixelUtil::getNumElemBits(supportedFormat) >= PixelUtil::getNumElemBits(format);
	}
}
