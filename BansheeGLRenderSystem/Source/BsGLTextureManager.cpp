#include "BsGLTextureManager.h"
#include "BsRenderSystem.h"
#include "BsGLRenderTexture.h"
#include "BsGLMultiRenderTexture.h"
#include "BsGLPixelFormat.h"

namespace BansheeEngine
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
        GLTexture* tex = new (bs_alloc<GLTexture, PoolAlloc>()) GLTexture(mGLSupport);

		return bs_core_ptr<GLTexture, PoolAlloc>(tex);
    }

	RenderTexturePtr GLTextureManager::createRenderTextureImpl()
	{
		GLRenderTexture* tex = new (bs_alloc<GLRenderTexture, PoolAlloc>()) GLRenderTexture();

		return bs_core_ptr<GLRenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr GLTextureManager::createMultiRenderTextureImpl()
	{
		GLMultiRenderTexture* tex = new (bs_alloc<GLMultiRenderTexture, PoolAlloc>()) GLMultiRenderTexture();

		return bs_core_ptr<GLMultiRenderTexture, PoolAlloc>(tex);
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
