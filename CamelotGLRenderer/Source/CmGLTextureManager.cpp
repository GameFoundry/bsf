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

#include "CmGLTextureManager.h"
#include "CmRenderSystem.h"
#include "CmGLRenderTexture.h"
#include "CmGLMultiRenderTexture.h"
#include "CmGLPixelFormat.h"

namespace CamelotFramework
{
    GLTextureManager::GLTextureManager( GLSupport& support)
        :TextureManager(), mGLSupport(support)
    {

    }

    GLTextureManager::~GLTextureManager()
    {

    }

    TexturePtr GLTextureManager::createTextureImpl()
    {
        GLTexture* tex = new (cm_alloc<GLTexture, PoolAlloc>()) GLTexture(mGLSupport);

		return cm_core_ptr<GLTexture, PoolAlloc>(tex);
    }

	RenderTexturePtr GLTextureManager::createRenderTextureImpl()
	{
		GLRenderTexture* tex = new (cm_alloc<GLRenderTexture, PoolAlloc>()) GLRenderTexture();

		return cm_core_ptr<GLRenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr GLTextureManager::createMultiRenderTextureImpl()
	{
		GLMultiRenderTexture* tex = new (cm_alloc<GLMultiRenderTexture, PoolAlloc>()) GLMultiRenderTexture();

		return cm_core_ptr<GLMultiRenderTexture, PoolAlloc>(tex);
	}

	PixelFormat GLTextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Adjust requested parameters to capabilities
        const RenderSystemCapabilities *caps = RenderSystem::instancePtr()->getCapabilities();

		// Check compressed texture support
		// If a compressed format not supported, revert to PF_A8R8G8B8
		if(PixelUtil::isCompressed(format) && !caps->hasCapability(RSC_TEXTURE_COMPRESSION_DXT))
		{
			return PF_A8R8G8B8;
		}

		// If floating point textures not supported, revert to PF_A8R8G8B8
		if(PixelUtil::isFloatingPoint(format) && !caps->hasCapability(RSC_TEXTURE_FLOAT))
		{
			return PF_A8R8G8B8;
		}
        
        // Check if this is a valid rendertarget format
		if( usage & TU_RENDERTARGET )
        {
            /// Get closest supported alternative
            /// If mFormat is supported it's returned
            return GLRTTManager::instance().getSupportedAlternative(format);
        }

		return GLPixelUtil::getClosestValidFormat(format);
	}
}
