#include "CmD3D11OcclusionQuery.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmMath.h"

namespace BansheeEngine
{
	D3D11OcclusionQuery::D3D11OcclusionQuery(bool binary)
		:OcclusionQuery(binary), mContext(nullptr), mQuery(nullptr), mNumSamples(0), mFinalized(false)
	{
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();

		D3D11_QUERY_DESC queryDesc;
		queryDesc.Query = mBinary ? D3D11_QUERY_OCCLUSION_PREDICATE : D3D11_QUERY_OCCLUSION;
		queryDesc.MiscFlags = 0;

		HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &mQuery);
		if (hr != S_OK)
			BS_EXCEPT(RenderingAPIException, "Failed to create an occlusion query.");

		mContext = device.getImmediateContext();
	}

	D3D11OcclusionQuery::~D3D11OcclusionQuery()
	{
		if (mQuery != nullptr)
			mQuery->Release();
	}

	void D3D11OcclusionQuery::begin()
	{
		mContext->Begin(mQuery);

		mNumSamples = 0;
		setActive(true);
	}

	void D3D11OcclusionQuery::end()
	{
		mContext->End(mQuery);
	}

	bool D3D11OcclusionQuery::isReady() const
	{
		if (mBinary)
		{
			BOOL anySamples = FALSE;
			return mContext->GetData(mQuery, &anySamples, sizeof(anySamples), 0) == S_OK;
		}
		else
		{
			UINT64 numSamples = 0;
			return mContext->GetData(mQuery, &numSamples, sizeof(numSamples), 0) == S_OK;
		}
	}

	UINT32 D3D11OcclusionQuery::getNumSamples()
	{
		if (!mFinalized && isReady())
		{
			finalize();
		}

		return mNumSamples;
	}

	void D3D11OcclusionQuery::finalize()
	{
		mFinalized = true;

		if (mBinary)
		{
			BOOL anySamples = FALSE;
			mContext->GetData(mQuery, &anySamples, sizeof(anySamples), 0);

			mNumSamples = anySamples == TRUE ? 1 : 0;
		}
		else
		{
			UINT64 numSamples = 0;
			mContext->GetData(mQuery, &numSamples, sizeof(numSamples), 0);

			mNumSamples = (UINT32)numSamples;
		}
	}
}