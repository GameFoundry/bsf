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

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmColor.h"
#include "CmRect.h"

namespace CamelotFramework {
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
		Viewport();
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
            RenderTargetPtr target,
            float left = 0.0f, float top = 0.0f,
            float width = 1.0f, float height = 1.0f,
            int ZOrder = 0);

        /** Default destructor.
        */
        virtual ~Viewport();

        /** Retrieves a pointer to the render target for this viewport.
        */
        RenderTargetPtr getTarget(void) const;

        /** Gets the Z-Order of this viewport. */
		int getZOrder(void) const;
		/** Gets one of the relative dimensions of the viewport,
            a value between 0.0 and 1.0.
        */
        float getNormalizedLeft(void) const;

        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */
        float getNormalizedTop(void) const;

        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */

        float getNormalizedWidth(void) const;
        /** Gets one of the relative dimensions of the viewport, a value
            between 0.0 and 1.0.
        */

        float getNormalizedHeight(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getLeft(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getTop(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */
        int getWidth(void) const;
        /** Gets one of the actual dimensions of the viewport, a value in
            pixels.
        */

        int getHeight(void) const;
               
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
        void setDimensions(float left, float top, float width, float height);

		/** Access to actual dimensions (based on target size).
        */
		const Rect& getDimensions() const { return mDimensions; }

    protected:
        RenderTargetPtr mTarget;
        // Relative dimensions, irrespective of target dimensions (0..1)
        float mRelLeft, mRelTop, mRelWidth, mRelHeight;
        // Actual dimensions, based on target dimensions
		Rect mDimensions;

        /// ZOrder
        int mZOrder;

		/** Notifies the viewport of a possible change in dimensions.
            @remarks
                Used by the target to update the viewport's dimensions
                (usually the result of a change in target size).
            @note
                Internal use by engine only.
        */
        void updateDimensions(void);
    };
	/** @} */
	/** @} */

}

#endif
