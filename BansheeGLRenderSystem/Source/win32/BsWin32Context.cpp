#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "BsWin32Context.h"
#include "BsException.h"

namespace BansheeEngine 
{
    Win32Context::Win32Context(HDC hdc, HGLRC glrc, bool ownsContext):
		mHDC(hdc), mGlrc(glrc), mOwnsContext(ownsContext)
    {
    }
    
    Win32Context::~Win32Context()
    {
		if (mOwnsContext)
			releaseContext();
    }
        
    void Win32Context::setCurrent()
    {
         wglMakeCurrent(mHDC, mGlrc);      
    }

	void Win32Context::endCurrent()
	{
		wglMakeCurrent(0, 0);
	}

	void Win32Context::releaseContext()
	{
		if (mGlrc != 0)
		{
			wglDeleteContext(mGlrc);
			mGlrc = 0;
			mHDC  = 0;
		}		
	}
}