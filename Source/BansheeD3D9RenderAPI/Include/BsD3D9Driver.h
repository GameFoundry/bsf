//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine
{
	/** @addtogroup D3D9
	 *  @{
	 */

	class D3D9VideoModeList;
	class D3D9VideoMode;

	/**	Holds data about a DX9 driver (adapter). */
	class BS_D3D9_EXPORT D3D9Driver
	{
	public:
		D3D9Driver();
		D3D9Driver(const D3D9Driver &ob);
		D3D9Driver(UINT32 adapterNumber, const D3DCAPS9& deviceCaps, const D3DADAPTER_IDENTIFIER9& adapterIdentifer);
		~D3D9Driver();

		/**	Returns hardware capabilities for this driver. */
		const D3DCAPS9&	getD3D9DeviceCaps() const { return mD3D9DeviceCaps; }

		/**	Returns name of the driver. */
		String getDriverName() const;

		/**	Returns description of the driver. */
		String getDriverDescription() const;

		/** Returns adapter number which corresponds to adapter number returned by DX9 API. */
		UINT32 getAdapterNumber() const { return mAdapterNumber; }

		/**	Returns DX9 adapter identifier. */
		const D3DADAPTER_IDENTIFIER9& getAdapterIdentifier() const { return mAdapterIdentifier; }

	private:				
		UINT32 mAdapterNumber;
		D3DCAPS9 mD3D9DeviceCaps;		
		D3DADAPTER_IDENTIFIER9 mAdapterIdentifier;
	};

	/** @} */
}