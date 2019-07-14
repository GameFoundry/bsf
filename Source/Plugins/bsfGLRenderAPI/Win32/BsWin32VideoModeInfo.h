//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsGLPrerequisites.h"
#include "Renderapi/BsVideoModeInfo.h"

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/** @copydoc VideoMode */
	class Win32VideoMode : public VideoMode
	{
	public:
		Win32VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx);

	private:
		friend class Win32VideoOutputInfo;
	};

	/** @copydoc VideoOutputInfo */
	class Win32VideoOutputInfo : public VideoOutputInfo
	{
	public:
		Win32VideoOutputInfo(HMONITOR monitorHandle, UINT32 outputIdx);

		/**	Gets a Win32 handle to the monitor referenced by this object. */
		HMONITOR getMonitorHandle() const { return mMonitorHandle; }

	private:
		HMONITOR mMonitorHandle;
	};

	/** @copydoc VideoModeInfo */
	class Win32VideoModeInfo : public VideoModeInfo
	{
	public:
		Win32VideoModeInfo();
	};

	/** @} */
}}
