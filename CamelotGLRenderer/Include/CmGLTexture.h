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
#include "CmHardwarePixelBuffer.h"

namespace CamelotEngine {

    class CM_RSGL_EXPORT GLTexture : public Texture
    {
    public:
        virtual ~GLTexture();      

		/// @copydoc Texture::getBuffer
		HardwarePixelBufferPtr getBuffer_internal(UINT32 face, UINT32 mipmap);

        // Takes the OGRE texture type (1d/2d/3d/cube) and returns the appropriate GL one
        GLenum getGLTextureTarget_internal(void) const;

        GLuint getGLID_internal() const;
		
		void getCustomAttribute_internal(const String& name, void* pData);

    protected:
		friend class GLTextureManager;

		// Constructor
		GLTexture(GLSupport& support);

		void initialize_internal();

		/// @copydoc Texture::createInternalResourcesImpl
		void createInternalResourcesImpl(void);
        /// @copydoc Resource::freeInternalResourcesImpl
        void freeInternalResourcesImpl(void);

		/** internal method, create GLHardwarePixelBuffers for every face and
			 mipmap level. This method must be called after the GL texture object was created,
			the number of mipmaps was set (GL_TEXTURE_MAX_LEVEL) and glTexImageXD was called to
			actually allocate the buffer
		*/
		void createSurfaceList();

		void createRenderTexture();

    private:
        GLuint mTextureID;
        GLSupport& mGLSupport;
		
		/// Vector of pointers to subsurfaces
		typedef vector<HardwarePixelBufferPtr>::type SurfaceList;
		SurfaceList	mSurfaceList;
    };

	typedef std::shared_ptr<GLTexture> GLTexturePtr;
}

#endif // __GLTEXTURE_H__
