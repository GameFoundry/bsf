#pragma once

#include "CmGLPrerequisites.h"
#include "CmVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	* @copydoc	VideoMode
	*/
	class CM_RSGL_EXPORT Win32VideoMode : public VideoMode
	{
	public:
		Win32VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo);

	private:
		friend class Win32VideoOutputInfo;
	};

	/**
	* @copydoc	VideoOutputInfo
	*/
	class CM_RSGL_EXPORT Win32VideoOutputInfo : public VideoOutputInfo
	{
	public:
		Win32VideoOutputInfo(char* deviceName);
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
	class CM_RSGL_EXPORT Win32VideoModeInfo : public VideoModeInfo
	{
	public:
		Win32VideoModeInfo();
	};
}