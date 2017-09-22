//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "RenderAPI/BsVideoModeInfo.h"
#include <X11/extensions/Xrandr.h>

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/** @copydoc VideoMode */
	class LinuxVideoMode : public VideoMode
	{
	public:
		LinuxVideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx);

	private:
		friend class LinuxVideoOutputInfo;
	};

	/** @copydoc VideoOutputInfo */
	class LinuxVideoOutputInfo : public VideoOutputInfo
	{
	public:
		LinuxVideoOutputInfo(::Display* x11Display, XRROutputInfo* outputInfo, XRRCrtcInfo* crtcInfo,
			 XRRScreenResources* screenRes, UINT32 resIdx, UINT32 outputIdx);
	};

	/** @copydoc VideoModeInfo */
	class LinuxVideoModeInfo : public VideoModeInfo
	{
	public:
		LinuxVideoModeInfo();
	};

	/** @} */
}}

