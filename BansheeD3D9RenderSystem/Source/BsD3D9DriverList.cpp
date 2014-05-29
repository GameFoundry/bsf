#include "BsD3D9DriverList.h"
#include "BsD3D9VideoModeInfo.h"
#include "BsException.h"
#include "BsD3D9RenderSystem.h"

namespace BansheeEngine 
{
	D3D9DriverList::D3D9DriverList()
	{
		IDirect3D9* lpD3D9 = D3D9RenderSystem::getDirect3D9();

		for (UINT32 i = 0; i < lpD3D9->GetAdapterCount(); i++)
		{
			D3DADAPTER_IDENTIFIER9 adapterIdentifier;
			D3DCAPS9 d3dcaps9;

			lpD3D9->GetAdapterIdentifier(i, 0, &adapterIdentifier);
			lpD3D9->GetDeviceCaps(i, D3DDEVTYPE_HAL, &d3dcaps9);

			mDriverList.push_back(D3D9Driver(i, d3dcaps9, adapterIdentifier));
		}

		mVideoModeInfo = bs_shared_ptr<D3D9VideoModeInfo>(lpD3D9);
	}

	D3D9DriverList::~D3D9DriverList()
	{
		mDriverList.clear();
	}

	UINT32 D3D9DriverList::count() const
	{
		return (UINT32)mDriverList.size();
	}

	D3D9Driver* D3D9DriverList::item(UINT32 index)
	{
		return &mDriverList.at(index);
	}

	D3D9Driver* D3D9DriverList::item(const String &name)
	{
		Vector<D3D9Driver>::iterator it = mDriverList.begin();
		if (it == mDriverList.end())
			return nullptr;

		for (;it != mDriverList.end(); ++it)
		{
			if (it->getDriverDescription() == name)
				return &(*it);
		}

		return nullptr;
	}
}
