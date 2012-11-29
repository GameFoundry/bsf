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

/* Define the number of priority groups for the render system's render targets. */
#ifndef CM_NUM_RENDERTARGET_GROUPS
	#define CM_NUM_RENDERTARGET_GROUPS 10
	#define OGRE_DEFAULT_RT_GROUP 4
	#define OGRE_REND_TO_TEX_RT_GROUP 2
#endif

namespace CamelotEngine {

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
    class CM_EXPORT RenderTarget
    {
    public:
		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};

        RenderTarget();
        virtual ~RenderTarget();

        /// Retrieve target's name.
        virtual const String& getName(void) const;

        /// Retrieve information about the render target.
        virtual void getMetrics(unsigned int& width, unsigned int& height, unsigned int& colourDepth);

        virtual unsigned int getWidth(void) const;
        virtual unsigned int getHeight(void) const;
        virtual unsigned int getColourDepth(void) const;

        /** Tells the target to update it's contents.
            @remarks
                If OGRE is not running in an automatic rendering loop
                (started using Root::startRendering),
                the user of the library is responsible for asking each render
                target to refresh. This is the method used to do this. It automatically
                re-renders the contents of the target using whatever cameras have been
                pointed at it (using Camera::setRenderTarget).
            @par
                This allows OGRE to be used in multi-windowed utilities
                and for contents to be refreshed only when required, rather than
                constantly as with the automatic rendering loop.
			@param swapBuffers For targets that support double-buffering, if set 
				to true, the target will immediately
				swap it's buffers after update. Otherwise, the buffers are
				not swapped, and you have to call swapBuffers yourself sometime
				later. You might want to do this on some rendersystems which 
				pause for queued rendering commands to complete before accepting
				swap buffers calls - so you could do other CPU tasks whilst the 
				queued commands complete. Or, you might do this if you want custom
				control over your windows, such as for externally created windows.
        */
        virtual void update(bool swapBuffers = true);
        /** Swaps the frame buffers to display the next frame.
            @remarks
                For targets that are double-buffered so that no
                'in-progress' versions of the scene are displayed
                during rendering. Once rendering has completed (to
                an off-screen version of the window) the buffers
                are swapped to display the new frame.

            @param
                waitForVSync If true, the system waits for the
                next vertical blank period (when the CRT beam turns off
                as it travels from bottom-right to top-left at the
                end of the pass) before flipping. If false, flipping
                occurs no matter what the beam position. Waiting for
                a vertical blank can be slower (and limits the
                framerate to the monitor refresh rate) but results
                in a steadier image with no 'tearing' (a flicker
                resulting from flipping buffers when the beam is
                in the progress of drawing the last frame).
        */
        virtual void swapBuffers(bool waitForVSync = true)
        { (void)waitForVSync; }

        /** Gets a custom (maybe platform-specific) attribute.
            @remarks
                This is a nasty way of satisfying any API's need to see platform-specific details.
                It horrid, but D3D needs this kind of info. At least it's abstracted.
            @param
                name The name of the attribute.
            @param
                pData Pointer to memory of the right kind of structure to receive the info.
        */
        virtual void getCustomAttribute(const String& name, void* pData);

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

        /** Indicates whether this target is the primary window. The
            primary window is special in that it is destroyed when
            ogre is shut down, and cannot be destroyed directly.
            This is the case because it holds the context for vertex,
            index buffers and textures.
        */
        virtual bool isPrimary(void) const;

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

        /** RenderSystem specific interface for a RenderTarget;
            this should be subclassed by RenderSystems.
        */
        class Impl
        {
        protected:
            ~Impl() { }
        };
        /** Get rendersystem specific interface for this RenderTarget.
            This is used by the RenderSystem to (un)bind this target, 
            and to get specific information like surfaces
            and framebuffer objects.
        */
        virtual Impl *_getImpl();

		/** Method for manual management of rendering : fires 'preRenderTargetUpdate'
			and initialises statistics etc.
		@remarks 
		<ul>
		<li>_beginUpdate resets statistics and fires 'preRenderTargetUpdate'.</li>
		<li>_updateViewport renders the given viewport (even if it is not autoupdated),
		fires preViewportUpdate and postViewportUpdate and manages statistics.</li>
		<li>_updateAutoUpdatedViewports renders only viewports that are auto updated,
		fires preViewportUpdate and postViewportUpdate and manages statistics.</li>
		<li>_endUpdate() ends statistics calculation and fires postRenderTargetUpdate.</li>
		</ul>
		you can use it like this for example :
		<pre>
			renderTarget->_beginUpdate();
			renderTarget->_updateViewport(1); // which is not auto updated
			renderTarget->_updateViewport(2); // which is not auto updated
			renderTarget->_updateAutoUpdatedViewports();
			renderTarget->_endUpdate();
			renderTarget->swapBuffers(true);
		</pre>
			Please note that in that case, the zorder may not work as you expect,
			since you are responsible for calling _updateViewport in the correct order.
        */
		virtual void _beginUpdate();
		
		/** Method for manual management of rendering - finishes statistics calculation 
			and fires 'postRenderTargetUpdate'.
		@remarks
		You should call it after a _beginUpdate
		@see _beginUpdate for more details.
		*/
		virtual void _endUpdate();

    protected:
        /// The name of this target.
        String mName;
		/// The priority of the render target.
		UINT8 mPriority;

        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mColourDepth;
        bool mIsDepthBuffered;

        bool mActive;
        bool mAutoUpdate;
		// Hardware sRGB gamma conversion done on write?
		bool mHwGamma;
		// FSAA performed?
		UINT32 mFSAA;
		String mFSAAHint;
			
		/// Internal implementation of update()
		virtual void updateImpl();
    };
	/** @} */
	/** @} */

} // Namespace

#endif
