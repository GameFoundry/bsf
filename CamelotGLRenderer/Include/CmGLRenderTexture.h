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

#ifndef __GLRENDERTEXTURE_H__
#define __GLRENDERTEXTURE_H__

#include "CmGLPrerequisites.h"
#include "CmGLTexture.h"
#include "CmGLFrameBufferObject.h"
#include "CmModule.h"

/// Extra GL constants
#define GL_DEPTH24_STENCIL8_EXT                           0x88F0

namespace CamelotEngine 
{  
    /** Base class for GL Render Textures
    */
    class CM_RSGL_EXPORT GLRenderTexture: public RenderTexture
    {
	public:
		virtual ~GLRenderTexture();

		bool requiresTextureFlipping() const { return true; }
		virtual void getCustomAttribute(const String& name, void* pData);

	protected:
		friend class GLTextureManager;

		GLRenderTexture();
		void destroy_internal();

		GLFrameBufferObject* mFB;

		void createInternalResourcesImpl();
    };

    /** Manager/factory for RenderTextures.
    */
    class CM_RSGL_EXPORT GLRTTManager : public Module<GLRTTManager>
    {
    public:
        GLRTTManager();
		~GLRTTManager();
        
        /** Check if a certain format is usable as FBO rendertarget format
        */
        bool checkFormat(PixelFormat format) { return mProps[format].valid; }
        
        /** Get a FBO without depth/stencil for temporary use, like blitting between textures.
        */
        GLuint getTemporaryFBO() { return mTempFBO; }

		/** Get the closest supported alternative format. If format is supported, returns format.
        */
        virtual PixelFormat getSupportedAlternative(PixelFormat format);
    private:
        /** Frame Buffer Object properties for a certain texture format.
        */
        struct FormatProperties
        {
            bool valid; // This format can be used as RTT (FBO)
            
            /** Allowed modes/properties for this pixel format
            */
            struct Mode
            {
                UINT32 depth;     // Depth format (0=no depth)
                UINT32 stencil;   // Stencil format (0=no stencil)
            };
            
            vector<Mode>::type modes;
        };
        /** Properties for all internal formats defined by the engine
        */
        FormatProperties mProps[PF_COUNT];

        /** Temporary FBO identifier
         */
        GLuint mTempFBO;
        
        /** Detect allowed FBO formats */
        void detectFBOFormats();
        GLuint _tryFormat(GLenum depthFormat, GLenum stencilFormat);
        bool _tryPackedFormat(GLenum packedFormat);
    };
}

#endif // __GLTEXTURE_H__
