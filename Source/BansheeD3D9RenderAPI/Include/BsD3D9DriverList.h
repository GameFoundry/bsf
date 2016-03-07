//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Driver.h"

namespace BansheeEngine 
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/**	Holds a list of all drivers (adapters) and video modes. */
	class BS_D3D9_EXPORT D3D9DriverList
	{
	public:
		D3D9DriverList();
		~D3D9DriverList();

		/**	Returns the number of drivers (adapters) available. */
		UINT32 count() const;

		/**	Returns driver with the specified index. */
		D3D9Driver* item(UINT32 index);

		/**	Returns drivers with the specified name or null if it cannot be found. */
		D3D9Driver* item(const String &name);

		/**	Returns available video modes for all drivers and output devices. */
		VideoModeInfoPtr getVideoModeInfo() const { return mVideoModeInfo; }

	private:
		Vector<D3D9Driver> mDriverList;
		VideoModeInfoPtr mVideoModeInfo;
	};

	/** @} */
}