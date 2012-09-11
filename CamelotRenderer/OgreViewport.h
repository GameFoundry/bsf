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
#ifndef __Viewport_H__
#define __Viewport_H__

#include "OgrePrerequisites.h"
#include "OgreCommon.h"
#include "OgreColourValue.h"
#include "OgreFrustum.h"

namespace Ogre {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** An abstraction of a viewport, i.e. a rendering region on a render
        target.
        @remarks
            A viewport is the meeting of a camera and a rendering surface -
            the camera renders the scene from a viewpoint, and places its
            results into some subset of a rendering target, which may be the
            whole surface or just a part of the surface. Each viewport has a
            single camera as source and a single target as destination. A
            camera only has 1 viewport, but a render target may have several.
            A viewport also has a Z-order, i.e. if there is more than one
            viewport on a single render target and they overlap, one must
            obscure the other in some predetermined way.
    */
	class CM_EXPORT Viewport
    {
    public:       
        /** The usual constructor.
            @param
                cam Pointer to a camera to be the source for the image.
            @param
                target Pointer to the render target to be the destination
                for the rendering.
            @param
                left
            @param
                top
            @param
                width
            @param
                height
                Dimensions of the viewport, expressed as a value between
                0 and 1. This allows the dimensions to apply irrespective of
                changes in the target's size: e.g. to fill the whole area,
                values of 0,0,1,1 are appropriate.
            @param
                ZOrder Relative Z-order on the target. Lower = further to
                the front.
        */
        Viewport(
            RenderTarget* target,
            Real left, Real top,
            Real width, Real height,
            int ZOrder);

        /** Default destructor.
        */
        virtual ~Viewport();

        /** Notifies the viewport of a possible change in dimensions.
            @remarks
                Used by the target to update the viewport's dimensions
                (usually the result of a change in target size).
            @note
                Internal use by Ogre only.
        */
        void _updateDimensions(void);

        /** Instructs the viewport to updates its contents.
        */
        virtual void update(void);
		
		/** Instructs the viewport to clear itself, without performing an update.
		 @remarks
			You would not normally call this method when updating the viewport, 
			since the viewport usually clears itself when updating anyway (@see 
		    Viewport::setClearEveryFrame). However, if you wish you have the
			option of manually clearing the frame buffer (or elements of it)
		    using this method.
		 @param buffers Bitmask identifying which buffer elements to clear
		 @param colour The colour value to clear to, if FBT_COLOUR is included
		 @param depth The depth value to clear to, if FBT_DEPTH is included
		 @param stencil The stencil value to clear to, if FBT_STENCIL is included
		*/
		void clear(unsigned int buffers = FBT_COLOUR | FBT_DEPTH,
				   const ColourValue& colour = ColourValue::Black, 
				   Real depth = 1.0f, unsigned short stencil = 0);

        /** Retrieves a pointer to the render target for this viewport.
        */
        RenderTarget* getTarget(void) const;

        /** Gets the Z-Order of this viewport. */
		int getZOrder(void) const;
		/** Gets one of the relative dimensions of the viewport,
            a value between 0.0 and 1.0.
        */
        Real getLeft(void) const;

        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */
        Real getTop(void) const;

        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */

        Real getWidth(void) const;
        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */

        Real getHeight(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getActualLeft(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getActualTop(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */
        int getActualWidth(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getActualHeight(void) const;
               
        /** Sets the dimensions (after creation).
            @param
                left
            @param
                top
            @param
                width
            @param
                height Dimensions relative to the size of the target,
                represented as real values between 0 and 1. i.e. the full
                target area is 0, 0, 1, 1.
        */
        void setDimensions(Real left, Real top, Real width, Real height);

        /** Sets the initial background colour of the viewport (before
            rendering).
        */
        void setBackgroundColour(const ColourValue& colour);

        /** Gets the background colour.
        */
        const ColourValue& getBackgroundColour(void) const;

        /** Determines whether to clear the viewport before rendering.
		@remarks
			You can use this method to set which buffers are cleared
			(if any) before rendering every frame.
        @param clear Whether or not to clear any buffers
		@param buffers One or more values from FrameBufferType denoting
			which buffers to clear, if clear is set to true. Note you should
			not clear the stencil buffer here unless you know what you're doing.
         */
        void setClearEveryFrame(bool clear, unsigned int buffers = FBT_COLOUR | FBT_DEPTH);

        /** Determines if the viewport is cleared before every frame.
        */
        bool getClearEveryFrame(void) const;

		/** Gets which buffers are to be cleared each frame. */
        unsigned int getClearBuffers(void) const;

		/** Access to actual dimensions (based on target size).
        */
        void getActualDimensions(
            int &left, int &top, int &width, int &height ) const;

    protected:
        RenderTarget* mTarget;
        // Relative dimensions, irrespective of target dimensions (0..1)
        float mRelLeft, mRelTop, mRelWidth, mRelHeight;
        // Actual dimensions, based on target dimensions
        int mActLeft, mActTop, mActWidth, mActHeight;
        /// ZOrder
        int mZOrder;
        /// Background options
        ColourValue mBackColour;
        bool mClearEveryFrame;
		unsigned int mClearBuffers;
    };
	/** @} */
	/** @} */

}

#endif
