#include "BsD3D9Driver.h"

namespace BansheeEngine
{   
	D3D9Driver::D3D9Driver()
	{						
		mAdapterNumber = 0;
		ZeroMemory(&mD3D9DeviceCaps, sizeof(mD3D9DeviceCaps));
		ZeroMemory(&mAdapterIdentifier, sizeof(mAdapterIdentifier));			
	}

	D3D9Driver::D3D9Driver(const D3D9Driver &ob)
	{			
		mAdapterNumber = ob.mAdapterNumber;
		mD3D9DeviceCaps	= ob.mD3D9DeviceCaps;
		mAdapterIdentifier = ob.mAdapterIdentifier;		
	}

	D3D9Driver::D3D9Driver(UINT32 adapterNumber, const D3DCAPS9& deviceCaps,
		const D3DADAPTER_IDENTIFIER9& adapterIdentifier)
	{				
		mAdapterNumber = adapterNumber;
		mD3D9DeviceCaps	= deviceCaps;
		mAdapterIdentifier = adapterIdentifier;		
	}

	D3D9Driver::~D3D9Driver()
	{ }

	String D3D9Driver::getDriverName() const
	{
		return String(mAdapterIdentifier.Driver);
	}

	String D3D9Driver::getDriverDescription() const
	{       
		StringStream str;
		str << "Monitor-" << (mAdapterNumber+1) << "-" << mAdapterIdentifier.Description;
		String driverDescription(str.str());
		StringUtil::trim(driverDescription);

        return driverDescription;
	}
}
