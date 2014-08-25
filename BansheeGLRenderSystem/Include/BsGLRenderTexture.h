#pragma once

#include "BsGLPrerequisites.h"
#include "BsGLTexture.h"
#include "BsGLFrameBufferObject.h"
#include "BsModule.h"

#define GL_DEPTH24_STENCIL8_EXT 0x88F0

namespace BansheeEngine 
{  
	class GLRenderTexture;

	/**
	 * @brief	OpenGL implementation of a render texture.
	 *
	 * @note	Core thread only.
	 */
    class BS_RSGL_EXPORT GLRenderTextureCore : public RenderTextureCore
    {
	public:
		GLRenderTextureCore(GLRenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc);

		virtual ~GLRenderTextureCore();

		/**
		 * @copydoc	RenderTextureCore::getCustomAttribute
		 */
		virtual void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class GLRenderTexture;

		GLFrameBufferObject* mFB;
    };

	/**
	 * @brief	OpenGL implementation of a render texture.
	 *
	 * @note	Sim thread only.
	 */
	class BS_RSGL_EXPORT GLRenderTexture : public RenderTexture
	{
	public:
		virtual ~GLRenderTexture() { }

	protected:
		friend class GLTextureManager;

		GLRenderTexture() { }

		/**
		 * @copydoc	RenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return true; }

		/**
		 * @copydoc	RenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual RenderTextureCore* createCore(RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc);
	};

    /**
     * @brief	Manager that handles valid render texture formats.
     * 			
	 * @note	Must be initialized when RenderSystem is first started.
     */
    class BS_RSGL_EXPORT GLRTTManager : public Module<GLRTTManager>
    {
    public:
        GLRTTManager();
		~GLRTTManager();
        
        /**
         * @brief	Check if a certain format is usable as a render target format.
         *
         * @note	Thread safe.
         */
        bool checkFormat(PixelFormat format) { return mProps[format].valid; }
        
        /**
         * @brief	Get the closest supported alternative format. If format is supported, returns format.
         *
         * @note	Thread safe.
         */
        virtual PixelFormat getSupportedAlternative(PixelFormat format);
    private:
        /** 
		 * Frame buffer object properties for a certain texture format.
         */
        struct FormatProperties
        {
            /** 
			 * Allowed modes/properties for this pixel format
             */
            struct Mode
            {
                UINT32 depth;     /**< Depth format (0 = no depth). */
                UINT32 stencil;   /**< Stencil format (0 = no stencil). */
            };
            
            Vector<Mode> modes;
			bool valid;
        };

        FormatProperties mProps[PF_COUNT];
        GLuint mTempFBO;
        
		/**
		 * @brief	Detect which internal formats are allowed to be used on render target 
		 *			color or depth/stencil surfaces.
		 */
        void detectFBOFormats();

		/**
		 * @brief	Checks are the specified depth & stencil formats compatible.
		 */
		bool _tryFormat(GLenum depthFormat, GLenum stencilFormat);

		/**
		 * @brief	Checks is the specified packed format valid for using
		 *			in the render target.
		 */
        bool _tryPackedFormat(GLenum packedFormat);
    };
}