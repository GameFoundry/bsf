#include "CmD3D9EventQuery.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9Device.h"
#include "CmException.h"

namespace CamelotFramework
{
	D3D9EventQuery::D3D9EventQuery()
		:mQuery(nullptr), mInitialized(false)
	{

	}

	D3D9EventQuery::~D3D9EventQuery()
	{
		if(mInitialized)
		{
			mQuery->Release();
		}
	}

	void D3D9EventQuery::begin()
	{
		IDirect3DDevice9* device = D3D9RenderSystem::getActiveD3D9Device();
		HRESULT hr = device->CreateQuery(D3DQUERYTYPE_EVENT, &mQuery);

		if(hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create an Event query.");
		}

		mQuery->Issue(D3DISSUE_END);

		mInitialized = true;
	}

	bool D3D9EventQuery::isReady() const
	{
		BOOL queryData;
		return mQuery->GetData(&queryData, sizeof(BOOL), 0) == S_OK;
	}
}