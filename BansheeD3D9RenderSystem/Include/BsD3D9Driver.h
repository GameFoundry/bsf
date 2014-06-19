#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsString.h"

namespace BansheeEngine
{
	class D3D9VideoModeList;
	class D3D9VideoMode;

	/**
	 * @brief	Holds data about a DX9 driver (adapter).
	 */
	class BS_D3D9_EXPORT D3D9Driver
	{
	public:
		D3D9Driver();
		D3D9Driver(const D3D9Driver &ob);
		D3D9Driver(UINT32 adapterNumber, const D3DCAPS9& deviceCaps, const D3DADAPTER_IDENTIFIER9& adapterIdentifer);
		~D3D9Driver();

		/**
		 * @brief	Returns hardware capabilities for this driver.
		 */
		const D3DCAPS9&	getD3D9DeviceCaps() const { return mD3D9DeviceCaps; }

		/**
		 * @brief	Returns name of the driver.
		 */
		String getDriverName() const;

		/**
		 * @brief	Returns description of the driver.
		 */
		String getDriverDescription() const;

		/**
		 * @brief	Returns adapter number which corresponds to adapter number returned by
		 *			DX9 API.
		 */
		UINT32 getAdapterNumber() const { return mAdapterNumber; }

		/**
		 * @brief	Returns DX9 adapter identifier.
		 */
		const D3DADAPTER_IDENTIFIER9& getAdapterIdentifier() const { return mAdapterIdentifier; }

	private:				
		UINT32 mAdapterNumber;
		D3DCAPS9 mD3D9DeviceCaps;		
		D3DADAPTER_IDENTIFIER9 mAdapterIdentifier;
	};
}