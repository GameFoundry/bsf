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
        GLRenderTexture(GLRTTManager* manager, const String &name, const GLSurfaceDesc &target, bool writeGamma, UINT32 fsaa);
        virtual ~GLRenderTexture();
        
        bool requiresTextureFlipping() const { return true; }

		virtual void getCustomAttribute_internal(const String& name, void* pData);

		virtual void swapBuffers(bool waitForVSync = true);
	protected:
		GLFrameBufferObject mFB;
    };
    
    /** Manager/factory for RenderTextures.
    */
    class CM_RSGL_EXPORT GLRTTManager : public Module<GLRTTManager>
    {
    public:
        GLRTTManager(bool atimode);
		~GLRTTManager();
        
        /** Bind a certain render target if it is a FBO. If it is not a FBO, bind the
            main frame buffer.
        */
        void bind(RenderTarget *target);
        
        /** Unbind a certain render target. No-op for FBOs.
        */
        void unbind(RenderTarget *target) {};
        
        /** Get best depth and stencil supported for given internalFormat
        */
        void getBestDepthStencil(GLenum internalFormat, GLenum *depthFormat, GLenum *stencilFormat);
        
        /** Create a texture rendertarget object
        */
        virtual GLRenderTexture *createRenderTexture(const String &name, 
			const GLSurfaceDesc &target, bool writeGamma, UINT32 fsaa);

		/** Create a multi render target 
		*/
		virtual MultiRenderTarget* createMultiRenderTarget(const String & name);
        
        /** Request a render buffer. If format is GL_NONE, return a zero buffer.
        */
        GLSurfaceDesc requestRenderBuffer(GLenum format, UINT32 width, UINT32 height, UINT32 fsaa);
        /** Request the specify render buffer in case shared somewhere. Ignore
            silently if surface.buffer is 0.
        */
        void requestRenderBuffer(const GLSurfaceDesc &surface);
        /** Release a render buffer. Ignore silently if surface.buffer is 0.
        */
        void releaseRenderBuffer(const GLSurfaceDesc &surface);
        
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
        
        /** Stencil and depth renderbuffers of the same format are re-used between surfaces of the 
            same size and format. This can save a lot of memory when a large amount of rendertargets
            are used.
        */
        struct RBFormat
        {
            RBFormat(GLenum inFormat, UINT32 inWidth, UINT32 inHeight, UINT32 fsaa):
                format(inFormat), width(inWidth), height(inHeight), samples(fsaa)
            {}
            GLenum format;
            UINT32 width;
            UINT32 height;
			UINT32 samples;
            // Overloaded comparison operator for usage in map
            bool operator < (const RBFormat &other) const
            {
                if(format < other.format)
                {
                    return true;
                }
                else if(format == other.format)
                {
                    if(width < other.width)
                    {
                        return true;
                    }
                    else if(width == other.width)
                    {
                        if(height < other.height)
                            return true;
						else if (height == other.height)
						{
							if (samples < other.samples)
								return true;
						}
                    }
                }
                return false;
            }
        };
        struct RBRef
        {
            RBRef(){}
            RBRef(GLRenderBuffer *inBuffer):
                buffer(inBuffer), refcount(1)
            { }
            GLRenderBuffer *buffer;
            size_t refcount;
        };
        typedef map<RBFormat, RBRef>::type RenderBufferMap;
        RenderBufferMap mRenderBufferMap;
        // map(format, sizex, sizey) -> [GLSurface*,refcount]
        
        /** Temporary FBO identifier
         */
        GLuint mTempFBO;
        
		/// Buggy ATI driver?
		bool mATIMode;
        
        /** Detect allowed FBO formats */
        void detectFBOFormats();
        GLuint _tryFormat(GLenum depthFormat, GLenum stencilFormat);
        bool _tryPackedFormat(GLenum packedFormat);
       
    };
}

#endif // __GLTEXTURE_H__
