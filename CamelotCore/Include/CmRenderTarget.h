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
#ifndef __RenderTarget_H__
#define __RenderTarget_H__

#include "CmPrerequisites.h"

#include "CmString.h"
#include "CmPixelUtil.h"
#include "CmViewport.h"
#include "CmCoreObject.h"

/* Define the number of priority groups for the render system's render targets. */
#ifndef CM_NUM_RENDERTARGET_GROUPS
	#define CM_NUM_RENDERTARGET_GROUPS 10
	#define CM_DEFAULT_RT_GROUP 4
	#define CM_REND_TO_TEX_RT_GROUP 2
#endif

namespace CamelotFramework 
{
	struct CM_EXPORT RENDER_SURFACE_DESC
	{
		TexturePtr texture;
		UINT32 face;
		UINT32 numFaces;
		UINT32 mipLevel;
	};

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** A 'canvas' which can receive the results of a rendering
        operation.
        @remarks
            This abstract class defines a common root to all targets of rendering operations. A
            render target could be a window on a screen, or another
            offscreen surface like a texture or bump map etc.
        @author
            Steven Streeting
        @version
            1.0
     */
    class CM_EXPORT RenderTarget : public CoreObject
    {
    public:
		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};

        virtual ~RenderTarget();

        /// Retrieve target's name.
        virtual const String& getName(void) const;

        /// Retrieve information about the render target.
        virtual void getMetrics(unsigned int& width, unsigned int& height, unsigned int& colourDepth);

        virtual unsigned int getWidth(void) const;
        virtual unsigned int getHeight(void) const;
        virtual unsigned int getColourDepth(void) const;

        /**
         * @brief	Swaps the frame buffers to display the next frame.
         * 			
		 * @note	Render thread only.
         */
		virtual void swapBuffers() {};

		/**
		 * @brief	Returns true if the render target is a render window.
		 */
		virtual bool isWindow() const = 0;

        /** Gets a custom (maybe platform-specific) attribute.
            @remarks
                This is a nasty way of satisfying any API's need to see platform-specific details.
                It horrid, but D3D needs this kind of info. At least it's abstracted.
            @param
                name The name of the attribute.
            @param
                pData Pointer to memory of the right kind of structure to receive the info.
        */
        virtual void getCustomAttribute(const String& name, void* pData) const;

		/** Sets the priority of this render target in relation to the others. 
        @remarks
            This can be used in order to schedule render target updates. Lower
            priorities will be rendered first. Note that the priority must be set
            at the time the render target is attached to the render system, changes
            afterwards will not affect the ordering.
        */
        virtual void setPriority( UINT8 priority ) { mPriority = priority; }
        /** Gets the priority of a render target. */
		virtual UINT8 getPriority() const { return mPriority; }

        /** Used to retrieve or set the active state of the render target.
        */
        virtual bool isActive() const;

        /** Used to set the active state of the render target.
        */
        virtual void setActive( bool state );

		/** Copies the current contents of the render target to a pixelbox. 
		@remarks See suggestPixelFormat for a tip as to the best pixel format to
			extract into, although you can use whatever format you like and the 
			results will be converted.
		*/
		virtual void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO) = 0;

		/** Suggests a pixel format to use for extracting the data in this target, 
			when calling copyContentsToMemory.
		*/
		virtual PixelFormat suggestPixelFormat() const { return PF_BYTE_RGBA; }

		virtual bool requiresTextureFlipping() const = 0;

		/** Indicates whether on rendering, linear colour space is converted to 
			sRGB gamma colour space. This is the exact opposite conversion of
			what is indicated by Texture::isHardwareGammaEnabled, and can only
			be enabled on creation of the render target. For render windows, it's
			enabled through the 'gamma' creation misc parameter. For textures, 
			it is enabled through the hwGamma parameter to the create call.
		*/
		virtual bool isHardwareGammaEnabled() const { return mHwGamma; }

		/** Indicates whether multisampling is performed on rendering and at what level.
		*/
		virtual UINT32 getFSAA() const { return mFSAA; }

		/** Gets the FSAA hint (@see Root::createRenderWindow)
		*/
		virtual const String& getFSAAHint() const { return mFSAAHint; }

		/**
		 * @brief	Returns true if the render target will wait for vertical sync before swapping buffers.
		 */
		bool getVSync() const { return mVSync; }

		/**
		 * @brief	Set whether the render target will wait for vertical sync before swapping buffers.
		 */
		void setVSync(bool vsync)  { mVSync = vsync; }

    protected:
		RenderTarget();

        /// The name of this target.
        String mName;
		/// The priority of the render target.
		UINT8 mPriority;

        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mColorDepth;

        bool mActive;
		// Hardware sRGB gamma conversion done on write?
		bool mHwGamma;
		// Wait for vsync?
		bool mVSync;
		// FSAA performed?
		UINT32 mFSAA;
		String mFSAAHint;
    };
	/** @} */
	/** @} */

} // Namespace

#endif
