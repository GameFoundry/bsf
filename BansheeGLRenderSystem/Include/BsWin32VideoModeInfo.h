//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	 * @copydoc	VideoMode
	 */
	class BS_RSGL_EXPORT Win32VideoMode : public VideoMode
	{
	public:
		Win32VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx);

	private:
		friend class Win32VideoOutputInfo;
	};

	/**
	 * @copydoc	VideoOutputInfo
	 */
	class BS_RSGL_EXPORT Win32VideoOutputInfo : public VideoOutputInfo
	{
	public:
		Win32VideoOutputInfo(HMONITOR monitorHandle, UINT32 outputIdx);
		~Win32VideoOutputInfo();

		/**
		 * @brief	Gets a Win32 handle to the monitor referenced by this object.
		 */
		HMONITOR getMonitorHandle() const { return mMonitorHandle; }

	private:
		HMONITOR mMonitorHandle;
	};

	/**
	 * @copydoc	VideoModeInfo
	 */
	class BS_RSGL_EXPORT Win32VideoModeInfo : public VideoModeInfo
	{
	public:
		Win32VideoModeInfo();
	};
}