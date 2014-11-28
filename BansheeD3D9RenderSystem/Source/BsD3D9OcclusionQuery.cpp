#include "BsD3D9OcclusionQuery.h"
#include "BsD3D9RenderAPI.h"
#include "BsRenderStats.h"
#include "BsMath.h"

namespace BansheeEngine
{
	D3D9OcclusionQuery::D3D9OcclusionQuery(bool binary)
		:OcclusionQuery(binary), mQuery(nullptr), mNumSamples(0), 
		mFinalized(false), mDevice(nullptr), mQueryIssued(false)
	{
		createQuery();
	}

	D3D9OcclusionQuery::~D3D9OcclusionQuery()
	{
		releaseQuery();
	}

	void D3D9OcclusionQuery::createQuery()
	{
		mDevice = D3D9RenderAPI::getActiveD3D9Device();

		HRESULT hr = mDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &mQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create an occlusion query.");
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Query);
	}

	void D3D9OcclusionQuery::releaseQuery()
	{
		SAFE_RELEASE(mQuery);
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Query);
	}

	void D3D9OcclusionQuery::begin()
	{
		if (mQuery != nullptr)
			mQuery->Issue(D3DISSUE_BEGIN);

		mNumSamples = 0;
		mQueryIssued = false;
		setActive(true);
	}

	void D3D9OcclusionQuery::end()
	{
		if (mQuery != nullptr)
			mQuery->Issue(D3DISSUE_END);

		mQueryIssued = true;
		mFinalized = false;
	}

	bool D3D9OcclusionQuery::isReady() const
	{
		if (mQuery == nullptr)
			return mQueryIssued; // If we lost the query, return as ready if it was ever issued

		if (!mQueryIssued)
			return false;

		BOOL queryData;
		return mQuery->GetData(&queryData, sizeof(BOOL), 0) == S_OK;
	}

	UINT32 D3D9OcclusionQuery::getNumSamples()
	{
		if (!mFinalized && isReady())
		{
			finalize();
		}

		return mNumSamples;
	}

	void D3D9OcclusionQuery::finalize()
	{
		mFinalized = true;

		if (mQuery == nullptr)
		{
			mNumSamples = 0;
			return;
		}

		DWORD numSamples;
		mQuery->GetData(&numSamples, sizeof(DWORD), 0);

		mNumSamples = (UINT32)numSamples;
	}

	void D3D9OcclusionQuery::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}

	void D3D9OcclusionQuery::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9OcclusionQuery::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9OcclusionQuery::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}
}