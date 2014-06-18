#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderTexture.h"
#include "BsTexture.h"
#include "BsGLSupport.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
	/**
	 * @brief	OpenGL implementation of a texture.
	 */
    class BS_RSGL_EXPORT GLTexture : public Texture
    {
    public:
        virtual ~GLTexture();      

		/**
		 * @brief	Returns OpenGL texture target type
		 */
        GLenum getGLTextureTarget() const;

		/**
		 * @brief	Returns internal OpenGL texture handle.
		 */
        GLuint getGLID() const;
		
		/**
		 * @brief	Returns a hardware pixel buffer for a certain face and level of the texture.
		 * 
		 * @param	face	Index of the texture face, if texture has more than one. Array index for
		 *					texture arrays and a cube face for cube textures.
		 * @param	mipmap	Index of the mip level. 0 being the largest mip level.
		 *
		 * @note	Cube face indices: +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
		 */
		std::shared_ptr<GLPixelBuffer> getBuffer(UINT32 face, UINT32 mipmap);

    protected:
		friend class GLTextureManager;

		GLTexture(GLSupport& support);

		/**
		 * @copydoc Texture::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc Texture::destroy_internal
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

		/**
		 * @brief	Creates pixel buffers for each face and mip level. Texture must 
		 *			have been created previously.
		 */
		void createSurfaceList();

    private:
        GLuint mTextureID;
        GLSupport& mGLSupport;
		std::shared_ptr<GLPixelBuffer> mLockedBuffer;
		
		Vector<std::shared_ptr<GLPixelBuffer>>mSurfaceList;
    };

	typedef std::shared_ptr<GLTexture> GLTexturePtr;
}