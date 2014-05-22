#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmString.h"

namespace BansheeEngine
{
	class D3D9VideoModeList;
	class D3D9VideoMode;

	class CM_D3D9_EXPORT D3D9Driver
	{
	public:
		D3D9Driver();
		D3D9Driver(const D3D9Driver &ob);
		D3D9Driver(UINT32 adapterNumber, const D3DCAPS9& deviceCaps, const D3DADAPTER_IDENTIFIER9& adapterIdentifer);
		~D3D9Driver();

		const D3DCAPS9&	getD3D9DeviceCaps() const { return mD3D9DeviceCaps; }
		String getDriverName() const;
		String getDriverDescription() const;
				
		UINT32 getAdapterNumber() const { return mAdapterNumber; }
		const D3DADAPTER_IDENTIFIER9& getAdapterIdentifier() const { return mAdapterIdentifier; }

	private:				
		UINT32 mAdapterNumber;
		D3DCAPS9 mD3D9DeviceCaps;		
		D3DADAPTER_IDENTIFIER9 mAdapterIdentifier;
	};
}