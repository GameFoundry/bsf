#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	* @copydoc	VideoMode
	*/
	class CM_D3D9_EXPORT D3D9VideoMode : public VideoMode
	{
	public:
		D3D9VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo);

	private:
		friend class D3D9VideoOutputInfo;
	};

	/**
	* @copydoc	VideoOutputInfo
	*/
	class CM_D3D9_EXPORT D3D9VideoOutputInfo : public VideoOutputInfo
	{
	public:
		D3D9VideoOutputInfo(IDirect3D9* d3d9device, UINT32 adapterIdx);

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
	class CM_D3D9_EXPORT D3D9VideoModeInfo : public VideoModeInfo
	{
	public:
		D3D9VideoModeInfo(IDirect3D9* d3d9device);
	};
}