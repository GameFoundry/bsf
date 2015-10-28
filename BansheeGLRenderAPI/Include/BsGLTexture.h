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
    class BS_RSGL_EXPORT GLTextureCore : public TextureCore
    {
    public:
		virtual ~GLTextureCore();

		/**
		 * @brief	Returns OpenGL texture target type
		 */
        GLenum getGLTextureTarget() const;

		/**
		 * @brief	Returns internal OpenGL texture handle.
		 */
        GLuint getGLID() const;

		/**
		 * @brief	Returns the internal OpenGL format used by the texture.
		 */
		GLenum getGLFormat() const { return mGLFormat; }
		
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
		friend class GLTextureCoreManager;

		GLTextureCore(GLSupport& support, TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount, const PixelDataPtr& initialData);

		/**
		 * @copydoc TextureCore::initialize
		 */
		void initialize() override;

		/**
		 * @copydoc TextureCore::lock
		 */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/**
		 * @copydoc TextureCore::unlock
		 */
		void unlockImpl() override;

		/**
		 * @copydoc TextureCore::copy
		 */
		void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target) override;

		/**
		 * @copydoc TextureCore::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/**
		 * @copydoc TextureCore::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false) override;

		/**
		 * @brief	Creates pixel buffers for each face and mip level. Texture must 
		 *			have been created previously.
		 */
		void createSurfaceList();

    private:
        GLuint mTextureID;
		GLenum mGLFormat;
        GLSupport& mGLSupport;
		std::shared_ptr<GLPixelBuffer> mLockedBuffer;
		
		Vector<std::shared_ptr<GLPixelBuffer>>mSurfaceList;
    };
}