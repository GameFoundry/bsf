#include "CmD3D11EventQuery.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmException.h"

namespace CamelotFramework
{
	D3D11EventQuery::D3D11EventQuery()
		:mQuery(nullptr), mInitialized(false)
	{

	}

	D3D11EventQuery::~D3D11EventQuery()
	{
		if(mInitialized)
		{
			mQuery->Release();
		}
	}

	void D3D11EventQuery::begin()
	{
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();

		D3D11_QUERY_DESC queryDesc;
		queryDesc.Query = D3D11_QUERY_EVENT;

		HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &mQuery);
		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create an Event query.");
		}

		mContext = device.getImmediateContext();
		mContext->End(mQuery);

		mInitialized = true;
	}

	bool D3D11EventQuery::isReady() const
	{
		BOOL queryData;
		return mContext->GetData(mQuery, &queryData, sizeof(BOOL), 0) == S_OK;
	}
}