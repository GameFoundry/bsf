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
#pragma once

#include "CmPrerequisites.h"

#include "CmString.h"
#include "CmPixelUtil.h"
#include "CmViewport.h"
#include "CmCoreObject.h"
#include "boost/signal.hpp"

namespace CamelotFramework 
{
	struct CM_EXPORT RENDER_SURFACE_DESC
	{
		TexturePtr texture;
		UINT32 face;
		UINT32 numFaces;
		UINT32 mipLevel;
	};

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

        const String& getName() const { return mName; }
        UINT32 getWidth() const { return mWidth; }
        UINT32 getHeight() const { return mHeight; }
		UINT32 getColorDepth() const { return mColorDepth; }
		UINT32 getFSAA() const { return mFSAA; }
		const String& getFSAAHint() const { return mFSAAHint; }

		/**
		 * @brief	Returns true if the render target will wait for vertical sync before swapping buffers.
		 */
		bool getVSync() const { return mVSync; }

		virtual void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @brief	Returns true if the render target is a render window.
		 */
		virtual bool isWindow() const = 0;
		bool isActive() const { return mActive; }
		bool isHwGammaEnabled() const { return mHwGamma; }

		virtual void setActive(bool state) { mActive = state; }

		INT32 getPriority() const { return mPriority; }
		void setPriority(INT32 priority) { mPriority = priority; }

        /**
         * @brief	Swaps the frame buffers to display the next frame.
         * 			
		 * @note	Core thread only.
         */
		virtual void swapBuffers() {};
		virtual void copyToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO) = 0;
		virtual bool requiresTextureFlipping() const = 0;

		mutable boost::signal<void()> onResized;
    protected:
		RenderTarget();

        String mName;

        UINT32 mWidth;
        UINT32 mHeight;
        UINT32 mColorDepth;
		
		INT32 mPriority;

        bool mActive;
		bool mHwGamma;
		bool mVSync;
		UINT32 mFSAA;
		String mFSAAHint;
    };
}
