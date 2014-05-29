#include "BsD3D9TimerQuery.h"
#include "BsD3D9RenderSystem.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	D3D9TimerQuery::D3D9TimerQuery()
		:mFinalized(false), mBeginQuery(nullptr), mFreqQuery(nullptr), mQueryIssued(false),
		mEndQuery(nullptr), mDisjointQuery(nullptr), mTimeDelta(0.0f), mDevice(nullptr)
	{
		createQuery();
	}

	D3D9TimerQuery::~D3D9TimerQuery()
	{
		releaseQuery();
	}

	void D3D9TimerQuery::createQuery()
	{
		mDevice = D3D9RenderSystem::getActiveD3D9Device();

		HRESULT hr = mDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, &mDisjointQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = mDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &mFreqQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = mDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &mBeginQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = mDevice->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &mEndQuery);
		if (hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}
	}

	void D3D9TimerQuery::releaseQuery()
	{
		SAFE_RELEASE(mBeginQuery);
		SAFE_RELEASE(mEndQuery);
		SAFE_RELEASE(mDisjointQuery);
		SAFE_RELEASE(mFreqQuery);
	}

	bool D3D9TimerQuery::isQueryValid() const
	{
		return mDisjointQuery != nullptr && mBeginQuery != nullptr && 
			mEndQuery != nullptr && mFreqQuery != nullptr;
	}

	void D3D9TimerQuery::begin()
	{
		mQueryIssued = false;

		if (isQueryValid())
		{
			mDisjointQuery->Issue(D3DISSUE_BEGIN);
			mFreqQuery->Issue(D3DISSUE_END);
			mBeginQuery->Issue(D3DISSUE_END);
		}

		setActive(true);
	}

	void D3D9TimerQuery::end()
	{
		if (isQueryValid())
		{
			mEndQuery->Issue(D3DISSUE_END);
			mDisjointQuery->Issue(D3DISSUE_END);
		}

		mQueryIssued = true;
		mFinalized = false;
	}

	bool D3D9TimerQuery::isReady() const
	{
		if (!isQueryValid()) // Possibly device reset, in which case query is considered done if issued
			return mQueryIssued;

		BOOL queryData;
		return mDisjointQuery->GetData(&queryData, sizeof(BOOL), 0) == S_OK;
	}

	float D3D9TimerQuery::getTimeMs()
	{
		if(!mFinalized && isReady())
		{
			finalize();
		}

		return mTimeDelta;
	}

	void D3D9TimerQuery::finalize()
	{
		mFinalized = true;

		if (!isQueryValid())
		{
			mTimeDelta = 0.0f;
			return;
		}

		BOOL disjoint;
		mDisjointQuery->GetData(&disjoint, sizeof(BOOL), 0);

		if(!disjoint)
		{
			UINT64 frequency;
			mFreqQuery->GetData(&frequency, sizeof(UINT64), 0);

			UINT64 timeStart, timeEnd;
			mBeginQuery->GetData(&timeStart, sizeof(UINT64), 0);
			mEndQuery->GetData(&timeEnd, sizeof(UINT64), 0);

			UINT64 delta = timeEnd - timeStart;
			mTimeDelta = (delta/(float)frequency) * 1000.0f;
		}
		else
		{
			LOGWRN("Unrealiable GPU timer query detected.");
		}
	}

	void D3D9TimerQuery::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}

	void D3D9TimerQuery::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9TimerQuery::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			releaseQuery();
	}

	void D3D9TimerQuery::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{
		if (d3d9Device == mDevice)
			createQuery();
	}
}