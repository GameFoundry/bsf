#include "BsD3D9EventQuery.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9ResourceManager.h"
#include "BsD3D9Device.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D9EventQuery::D3D9EventQuery()
		:mQuery(nullptr), mDevice(nullptr), mQueryIssued(false)
	{
		createQuery();
	}

	D3D9EventQuery::~D3D9EventQuery()
	{
		releaseQuery();
	}

	void D3D9EventQuery::createQuery()
	{
		mDevice = D3D9RenderSystem::getActiveD3D9Device();

		HRESULT hr = mDevice->CreateQuery(D3DQUERYTYPE_EVENT, &mQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create an Event query.");
		}
	}

	void D3D9EventQuery::releaseQuery()
	{
		SAFE_RELEASE(mQuery);
	}

	void D3D9EventQuery::begin()
	{
		if (mQuery != nullptr)
			mQuery->Issue(D3DISSUE_END);

		setActive(true);
		mQueryIssued = true;
	}

	bool D3D9EventQuery::isReady() const
	{
		if (mQuery == nullptr) // Possibly device reset, in which case query is done
			return mQueryIssued;

		BOOL queryData;
		return mQuery->GetData(&queryData, sizeof(BOOL), 0) == S_OK;
	}

	void D3D9EventQuery::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}

	void D3D9EventQuery::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9EventQuery::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9EventQuery::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}
}