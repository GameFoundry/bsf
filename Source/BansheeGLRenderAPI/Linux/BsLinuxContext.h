//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLContext.h"
#include <GL/glxew.h>

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	Linux specific implementation of an OpenGL context. */
	class LinuxContext : public GLContext
	{
	public:
		/**
		 * Constructs a new OpenGL context.
		 *
		 * @param[in] 	x11display 		X11 display all windows using this context will be created with.
		 * @param[in]	visualInfo		X11 visual info describing properties of all windows that will use this context.
		 **/
		LinuxContext(::Display* x11display, XVisualInfo& visualInfo);
		virtual ~LinuxContext();

		/** @copydoc GLContext::setCurrent */
		void setCurrent(const RenderWindow& window) override;

		/** @copydoc GLContext::endCurrent */
		void endCurrent() override;

		/** @copydoc GLContext::releaseContext  */
		void releaseContext() override;

	protected:
		::Display* mDisplay;
		::Window mCurrentWindow;
		GLXContext mContext;
	};

	/** @} */
}}