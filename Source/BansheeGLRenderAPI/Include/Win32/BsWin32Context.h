//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsWin32Prerequisites.h"
#include "BsGLContext.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Windows specific implementation of an OpenGL context.
	 */
    class BS_RSGL_EXPORT Win32Context : public GLContext
    {
    public:
		/**
		 * @brief	Constructs a new context from a Windows device context and OpenGL rendering context.
		 *			Optionally you may specify that the context isn't owned by us (might be created by some external
		 *			library), in which case it will not be automatically released.
		 */
        Win32Context(HDC hdc, HGLRC glrc, bool ownsContext);
        virtual ~Win32Context();

        /**
         * @copydoc	GLContext::setCurrent
         */
        virtual void setCurrent();

        /**
         * @copydoc	GLContext::endCurrent
         */
		virtual void endCurrent();

        /**
         * @copydoc	GLContext::releaseContext
         */
		virtual void releaseContext();

	protected:
        HDC     mHDC;
        HGLRC   mGlrc;
		bool	mOwnsContext;
    };
}