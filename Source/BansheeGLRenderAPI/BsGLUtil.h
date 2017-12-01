//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#if BS_PLATFORM == BS_PLATFORM_WIN32

#include "Win32/BsWin32GLSupport.h"
#include "Win32/BsWin32VideoModeInfo.h"

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	Helper method that returns a platform specific GL support object. */
	GLSupport* getGLSupport()
	{
		return bs_new<Win32GLSupport>();
	}

	/** @} */
}}

#elif BS_PLATFORM == BS_PLATFORM_LINUX

#include "Linux/BsLinuxGLSupport.h"

namespace bs { namespace ct
	{
		/** @addtogroup GL
		 *  @{
		 */

		/**	Helper method that returns a platform specific GL support object. */
		GLSupport* getGLSupport()
		{
			return bs_new<LinuxGLSupport>();
		}

		/** @} */
	}}

#elif BS_PLATFORM == BS_PLATFORM_OSX

#include "MacOS/BsMacOSGLSupport.h"

namespace bs::ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	Helper method that returns a platform specific GL support object. */
	GLSupport* getGLSupport()
	{
		return bs_new<MacOSGLSupport>();
	}

	/** @} */
}
#endif