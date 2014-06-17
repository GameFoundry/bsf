#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderTexture.h"
#include "BsTexture.h"
#include "BsGLSupport.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
    class BS_RSGL_EXPORT GLTexture : public Texture
    {
    public:
        virtual ~GLTexture();      

        // Takes the engine texture type (1d/2d/3d/cube) and returns the appropriate GL one
        GLenum getGLTextureTarget(void) const;

        GLuint getGLID() const;
		
		/** Return hardware pixel buffer for a surface. This buffer can then
			be used to copy data from and to a particular level of the texture.
			@param face 	Face number, in case of a cubemap texture. Must be 0
							for other types of textures.
                            For cubemaps, this is one of 
                            +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
			@param mipmap	Mipmap level. This goes from 0 for the first, largest
							mipmap level to getNumMipmaps()-1 for the smallest.
			@returns	A shared pointer to a hardware pixel buffer
			@remarks	The buffer is invalidated when the resource is unloaded or destroyed.
						Do not use it after the lifetime of the containing texture.
		*/
		std::shared_ptr<GLPixelBuffer> getBuffer(UINT32 face, UINT32 mipmap);

    protected:
		friend class GLTextureManager;

		// Constructor
		GLTexture(GLSupport& support);

		/**
		 * @copydoc Texture::initialize_internal()
		 */
		void initialize_internal();

		/**
		 * @copydoc Texture::destroy_internal()
		 */
		void destroy_internal();

		/**
		 * @copydoc Texture::lock
		 */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::unlock
		 */
		void unlockImpl();

		/**
		 * @copydoc Texture::copy
		 */
		void copyImpl(TexturePtr& target);

		/**
		 * @copydoc Texture::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false);

		/** internal method, create GLHardwarePixelBuffers for every face and
			 mipmap level. This method must be called after the GL texture object was created,
			the number of mipmaps was set (GL_TEXTURE_MAX_LEVEL) and glTexImageXD was called to
			actually allocate the buffer
		*/
		void createSurfaceList();

    private:
        GLuint mTextureID;
        GLSupport& mGLSupport;
		std::shared_ptr<GLPixelBuffer> mLockedBuffer;
		
		/// Vector of pointers to subsurfaces
		typedef Vector<std::shared_ptr<GLPixelBuffer>> SurfaceList;
		SurfaceList	mSurfaceList;
    };

	typedef std::shared_ptr<GLTexture> GLTexturePtr;
}