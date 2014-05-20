#include "CmD3D9Driver.h"
#include "CmD3D9VideoModeList.h"
#include "CmD3D9VideoMode.h"

namespace BansheeEngine
{   
	D3D9Driver::D3D9Driver()
	{						
		mAdapterNumber	= 0;
		ZeroMemory(&mD3D9DeviceCaps, sizeof(mD3D9DeviceCaps));
		ZeroMemory(&mAdapterIdentifier, sizeof(mAdapterIdentifier));
		ZeroMemory(&mDesktopDisplayMode, sizeof(mDesktopDisplayMode));		
		mpVideoModeList = NULL;				
	}

	D3D9Driver::D3D9Driver( const D3D9Driver &ob )
	{			
		mAdapterNumber		= ob.mAdapterNumber;
		mD3D9DeviceCaps		= ob.mD3D9DeviceCaps;
		mAdapterIdentifier	= ob.mAdapterIdentifier;
		mDesktopDisplayMode = ob.mDesktopDisplayMode;
		mpVideoModeList		= NULL;				
	}

	D3D9Driver::D3D9Driver( unsigned int adapterNumber,
		const D3DCAPS9& deviceCaps,
		const D3DADAPTER_IDENTIFIER9& adapterIdentifier, 
		const D3DDISPLAYMODE& desktopDisplayMode )
	{				
		mAdapterNumber		= adapterNumber;
		mD3D9DeviceCaps		= deviceCaps;
		mAdapterIdentifier	= adapterIdentifier;
		mDesktopDisplayMode = desktopDisplayMode;
		mpVideoModeList		= NULL;			
	}

	D3D9Driver::~D3D9Driver()
	{
		if(mpVideoModeList != nullptr)
			cm_delete(mpVideoModeList);
	}

	String D3D9Driver::DriverName() const
	{
		return String(mAdapterIdentifier.Driver);
	}

	String D3D9Driver::DriverDescription() const
	{       
		StringStream str;
		str << "Monitor-" << (mAdapterNumber+1) << "-" << mAdapterIdentifier.Description;
		String driverDescription(str.str());
		StringUtil::trim(driverDescription);

        return  driverDescription;
	}

	D3D9VideoModeList* D3D9Driver::getVideoModeList()
	{
		if( !mpVideoModeList )
			mpVideoModeList = cm_new<D3D9VideoModeList>(this);

		return mpVideoModeList;
	}	
}
