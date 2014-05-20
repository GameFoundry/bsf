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
		// Constructors
		D3D9Driver();						// Default
		D3D9Driver( const D3D9Driver &ob );	// Copy
		D3D9Driver( unsigned int adapterNumber, 
			const D3DCAPS9& deviceCaps,
			const D3DADAPTER_IDENTIFIER9& adapterIdentifer, 
			const D3DDISPLAYMODE& desktopDisplayMode);
		~D3D9Driver();

				
		const D3DCAPS9&		getD3D9DeviceCaps	() const { return mD3D9DeviceCaps; }
		String				DriverName			() const;
		String				DriverDescription	() const;
				
		unsigned int					getAdapterNumber	() const { return mAdapterNumber; }
		const D3DADAPTER_IDENTIFIER9&	getAdapterIdentifier() const { return mAdapterIdentifier; }
		const D3DDISPLAYMODE&			getDesktopMode		() const { return mDesktopDisplayMode; }
		D3D9VideoModeList*				getVideoModeList	();
			
	private:				
		unsigned int mAdapterNumber;
		D3DCAPS9 mD3D9DeviceCaps;		
		D3DADAPTER_IDENTIFIER9 mAdapterIdentifier;
		D3DDISPLAYMODE mDesktopDisplayMode;
		D3D9VideoModeList* mpVideoModeList;	
	};
}