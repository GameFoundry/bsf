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

#ifndef __GLTEXTURE_H__
#define __GLTEXTURE_H__

#include "CmGLPrerequisites.h"
#include "CmRenderTexture.h"
#include "CmTexture.h"
#include "CmGLSupport.h"
#include "CmPixelBuffer.h"

namespace CamelotFramework {

    class CM_RSGL_EXPORT GLTexture : public Texture
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
		PixelBufferPtr getBuffer(UINT32 face, UINT32 mipmap);

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

		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face);
		void unlockImpl();

		void copyImpl(TexturePtr& target);

		/** internal method, create GLHardwarePixelBuffers for every face and
			 mipmap level. This method must be called after the GL texture object was created,
			the number of mipmaps was set (GL_TEXTURE_MAX_LEVEL) and glTexImageXD was called to
			actually allocate the buffer
		*/
		void createSurfaceList();

    private:
        GLuint mTextureID;
        GLSupport& mGLSupport;
		PixelBufferPtr mLockedBuffer;
		
		/// Vector of pointers to subsurfaces
		typedef vector<PixelBufferPtr>::type SurfaceList;
		SurfaceList	mSurfaceList;
    };

	typedef std::shared_ptr<GLTexture> GLTexturePtr;
}

#endif // __GLTEXTURE_H__
