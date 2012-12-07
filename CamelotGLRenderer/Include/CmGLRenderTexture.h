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

#include "CmGLTexture.h"
#include "CmModule.h"

namespace CamelotEngine {
    /** GL surface descriptor. Points to a 2D surface that can be rendered to. 
    */
    struct GLSurfaceDesc
    {
    public:
        GLHardwarePixelBuffer *buffer;
        UINT32 zoffset;
		UINT32 numSamples;

		GLSurfaceDesc() :buffer(0), zoffset(0), numSamples(0) {}
    };
    
    /** Base class for GL Render Textures
    */
    class CM_RSGL_EXPORT GLRenderTexture: public RenderTexture
    {
    public:
        GLRenderTexture(const String &name, const GLSurfaceDesc &target, bool writeGamma, UINT32 fsaa);
        virtual ~GLRenderTexture();
        
        bool requiresTextureFlipping() const { return true; }
    };
    
    /** Manager/factory for RenderTextures.
    */
    class CM_RSGL_EXPORT GLRTTManager: public Module<GLRTTManager>
    {
    public:
        virtual ~GLRTTManager();
        
        /** Create a texture rendertarget object
        */
        virtual RenderTexture *createRenderTexture(const String &name, const GLSurfaceDesc &target, bool writeGamma, UINT32 fsaa) = 0;
        
         /** Check if a certain format is usable as rendertexture format
        */
        virtual bool checkFormat(PixelFormat format) = 0;
        
        /** Bind a certain render target.
        */
        virtual void bind(RenderTarget *target) = 0;
        
        /** Unbind a certain render target. This is called before binding another RenderTarget, and
            before the context is switched. It can be used to do a copy, or just be a noop if direct
            binding is used.
        */
        virtual void unbind(RenderTarget *target) = 0;

		/** Create a multi render target 
		*/
		virtual MultiRenderTarget* createMultiRenderTarget(const String & name);
        
        /** Get the closest supported alternative format. If format is supported, returns format.
        */
        virtual PixelFormat getSupportedAlternative(PixelFormat format);
    };
    
    /** RenderTexture for simple copying from frame buffer
    */
    class GLCopyingRTTManager;
    class CM_RSGL_EXPORT GLCopyingRenderTexture: public GLRenderTexture
    {
    public:
        GLCopyingRenderTexture(GLCopyingRTTManager *manager, const String &name, const GLSurfaceDesc &target, 
			bool writeGamma, UINT32 fsaa);
        
        virtual void getCustomAttribute(const String& name, void* pData);
    };
    
    /** Simple, copying manager/factory for RenderTextures. This is only used as the last fallback if
        both PBuffers and FBOs aren't supported.
    */
    class CM_RSGL_EXPORT GLCopyingRTTManager: public GLRTTManager
    {
    public:
        GLCopyingRTTManager();
        virtual ~GLCopyingRTTManager();
        
        /** @copydoc GLRTTManager::createRenderTexture
        */
        virtual RenderTexture *createRenderTexture(const String &name, const GLSurfaceDesc &target, bool writeGamma, UINT32 fsaa);
        
        /** @copydoc GLRTTManager::checkFormat
        */
        virtual bool checkFormat(PixelFormat format);
        
        /** @copydoc GLRTTManager::bind
        */
        virtual void bind(RenderTarget *target);
        
        /** @copydoc GLRTTManager::unbind
        */
        virtual void unbind(RenderTarget *target);
    };
}

#endif // __GLTEXTURE_H__
