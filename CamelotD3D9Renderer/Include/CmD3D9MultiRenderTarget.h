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
#ifndef __D3D9MULTIRENDERTARGET_H__
#define __D3D9MULTIRENDERTARGET_H__

#include "CmD3D9Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"
#include "CmException.h"
#include "CmD3D9HardwarePixelBuffer.h"

namespace CamelotEngine {
	class CM_D3D9_EXPORT D3D9MultiRenderTarget : public MultiRenderTarget
	{
	public:
		D3D9MultiRenderTarget(const String &name);
		~D3D9MultiRenderTarget();

        virtual void update(bool swapBuffers);

		virtual void getCustomAttribute_internal( const String& name, void *pData );

		bool requiresTextureFlipping() const { return false; }
	private:
		D3D9HardwarePixelBuffer *mRenderTargets[CM_MAX_MULTIPLE_RENDER_TARGETS];
		virtual void bindSurfaceImpl(UINT32 attachment, RenderTexture *target);
		virtual void unbindSurfaceImpl(UINT32 attachment);

		/** Check surfaces and update RenderTarget extent */
		void checkAndUpdate();
	};
};

#endif
