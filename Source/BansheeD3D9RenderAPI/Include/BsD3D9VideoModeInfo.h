//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	* @copydoc	VideoMode
	*/
	class BS_D3D9_EXPORT D3D9VideoMode : public VideoMode
	{
	public:
		D3D9VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx);

	private:
		friend class D3D9VideoOutputInfo;
	};

	/**
	* @copydoc	VideoOutputInfo
	*/
	class BS_D3D9_EXPORT D3D9VideoOutputInfo : public VideoOutputInfo
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
	class BS_D3D9_EXPORT D3D9VideoModeInfo : public VideoModeInfo
	{
	public:
		D3D9VideoModeInfo(IDirect3D9* d3d9device);
	};
}