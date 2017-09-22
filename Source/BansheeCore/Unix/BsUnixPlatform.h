//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include "Platform/BsPlatform.h"

namespace bs
{
	class LinuxWindow;

	/** @addtogroup Platform-Internal
	 *  @{
	 */

	/**
	 * Contains various Linux specific platform functionality;
	 */
	class BS_CORE_EXPORT LinuxPlatform : public Platform
	{
	public:
		/** Returns the active X11 display. */
		static ::Display* getXDisplay();

		/** Returns the main X11 window. Caller must ensure the main window has been created. */
		static ::Window getMainXWindow();

		/** Notifies the system that a new window was created. */
		static void _registerWindow(::Window xWindow, LinuxWindow* window);

		/** Notifies the system that a window is about to be destroyed. */
		static void _unregisterWindow(::Window xWindow);

		/** Generates a X11 Pixmap from the provided pixel data. */
		static Pixmap createPixmap(const SPtr<PixelData>& data);
	};

	/** @} */
}

// Undefine conflicting defines from X.h
#undef None
#undef Success
#undef Convex
#undef Bool