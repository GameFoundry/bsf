#pragma once

#include "CmWin32Prerequisites.h"
#include "CmGLContext.h"

namespace BansheeEngine 
{
    class BS_RSGL_EXPORT Win32Context : public GLContext
    {
    public:
        Win32Context(HDC hdc, HGLRC glrc, bool ownsContext);
        virtual ~Win32Context();

        /** See GLContext */
        virtual void setCurrent();
		/** See GLContext */
		virtual void endCurrent();

		virtual void releaseContext();

	protected:
        HDC     mHDC;
        HGLRC   mGlrc;
		bool	mOwnsContext;
    };
}