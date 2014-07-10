//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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