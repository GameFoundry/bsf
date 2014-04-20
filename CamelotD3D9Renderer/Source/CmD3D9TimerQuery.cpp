#include "CmD3D9TimerQuery.h"
#include "CmD3D9RenderSystem.h"
#include "CmDebug.h"

namespace BansheeEngine
{
	D3D9TimerQuery::D3D9TimerQuery()
		:mFinalized(false), mBeginQuery(nullptr), mFreqQuery(nullptr),
		mEndQuery(nullptr), mDisjointQuery(nullptr), mTimeDelta(0.0f)
	{
		IDirect3DDevice9* device = D3D9RenderSystem::getActiveD3D9Device();

		HRESULT hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, &mDisjointQuery);
		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &mFreqQuery);
		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &mBeginQuery);
		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &mEndQuery);
		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}
	}

	D3D9TimerQuery::~D3D9TimerQuery()
	{
		if(mBeginQuery != nullptr)
			mBeginQuery->Release();

		if(mEndQuery != nullptr)
			mEndQuery->Release();

		if(mDisjointQuery != nullptr)
			mDisjointQuery->Release();

		if(mFreqQuery != nullptr)
			mFreqQuery->Release();
	}

	void D3D9TimerQuery::begin()
	{
		mDisjointQuery->Issue(D3DISSUE_BEGIN);
		mFreqQuery->Issue(D3DISSUE_END);
		mBeginQuery->Issue(D3DISSUE_END);
		setActive(true);
	}

	void D3D9TimerQuery::end()
	{
		mEndQuery->Issue(D3DISSUE_END);
		mDisjointQuery->Issue(D3DISSUE_END);
	}

	bool D3D9TimerQuery::isReady() const
	{
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
}