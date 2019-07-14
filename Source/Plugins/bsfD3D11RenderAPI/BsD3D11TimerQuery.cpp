//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsD3D11TimerQuery.h"
#include "BsD3D11RenderAPI.h"
#include "BsD3D11Device.h"
#include "BsD3D11CommandBuffer.h"
#include "Profiling/BsRenderStats.h"
#include "Debug/BsDebug.h"

namespace bs { namespace ct
{
	D3D11TimerQuery::D3D11TimerQuery(UINT32 deviceIdx)
	{
		assert(deviceIdx == 0 && "Multiple GPUs not supported natively on DirectX 11.");

		D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();

		D3D11_QUERY_DESC queryDesc;
		queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		queryDesc.MiscFlags = 0;

		HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &mDisjointQuery);
		if(hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		queryDesc.Query = D3D11_QUERY_TIMESTAMP;

		hr = device.getD3D11Device()->CreateQuery(&queryDesc, &mBeginQuery);
		if(hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		hr = device.getD3D11Device()->CreateQuery(&queryDesc, &mEndQuery);
		if(hr != S_OK)
		{
			BS_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
		}

		mContext = device.getImmediateContext();
		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Query);
	}

	D3D11TimerQuery::~D3D11TimerQuery()
	{
		if(mBeginQuery != nullptr)
			mBeginQuery->Release();

		if(mEndQuery != nullptr)
			mEndQuery->Release();

		if(mDisjointQuery != nullptr)
			mDisjointQuery->Release();

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Query);
	}

	void D3D11TimerQuery::begin(const SPtr<CommandBuffer>& cb)
	{
		auto execute = [&]()
		{
			mContext->Begin(mDisjointQuery);
			mContext->End(mBeginQuery);

			mQueryEndCalled = false;

			setActive(true);
		};

		if (cb == nullptr)
			execute();
		else
		{
			SPtr<D3D11CommandBuffer> d3d11cb = std::static_pointer_cast<D3D11CommandBuffer>(cb);
			d3d11cb->queueCommand(execute);
		}
	}

	void D3D11TimerQuery::end(const SPtr<CommandBuffer>& cb)
	{
		auto execute = [&]()
		{
			mContext->End(mEndQuery);
			mContext->End(mDisjointQuery);

			mQueryEndCalled = true;
			mFinalized = false;
		};

		if (cb == nullptr)
			execute();
		else
		{
			SPtr<D3D11CommandBuffer> d3d11cb = std::static_pointer_cast<D3D11CommandBuffer>(cb);
			d3d11cb->queueCommand(execute);
		}
	}

	bool D3D11TimerQuery::isReady() const
	{
		if (!mQueryEndCalled)
			return false;

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
		return mContext->GetData(mDisjointQuery, &disjointData, sizeof(disjointData), 0) == S_OK;
	}

	float D3D11TimerQuery::getTimeMs()
	{
		if(!mFinalized && isReady())
		{
			finalize();
		}

		return mTimeDelta;
	}

	void D3D11TimerQuery::finalize()
	{
		UINT64 timeStart, timeEnd;

		mContext->GetData(mBeginQuery, &timeStart, sizeof(timeStart), 0);
		mContext->GetData(mEndQuery, &timeEnd, sizeof(timeEnd), 0);

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
		mContext->GetData(mDisjointQuery, &disjointData, sizeof(disjointData), 0);

		float time = 0.0f;
		if(disjointData.Disjoint == FALSE)
		{
			float frequency = static_cast<float>(disjointData.Frequency);

			UINT64 delta = timeEnd - timeStart;
			mTimeDelta = (delta / (float)frequency) * 1000.0f;
		}
		else
			BS_LOG(Verbose, RenderBackend, "Unrealiable GPU timer query detected.");
	}
}}
