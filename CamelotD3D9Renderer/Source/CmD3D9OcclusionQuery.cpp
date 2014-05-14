#include "CmD3D9OcclusionQuery.h"
#include "CmD3D9RenderSystem.h"
#include "CmMath.h"

namespace BansheeEngine
{
	D3D9OcclusionQuery::D3D9OcclusionQuery(bool binary)
		:OcclusionQuery(binary), mQuery(nullptr), mNumFragments(0), mFinalized(false)
	{
		IDirect3DDevice9* device = D3D9RenderSystem::getActiveD3D9Device();

		HRESULT hr = device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &mQuery);
		if (hr != S_OK)
		{
			CM_EXCEPT(RenderingAPIException, "Failed to create an occlusion query.");
		}
	}

	D3D9OcclusionQuery::~D3D9OcclusionQuery()
	{
		if (mQuery != nullptr)
			mQuery->Release();
	}

	void D3D9OcclusionQuery::begin()
	{
		mQuery->Issue(D3DISSUE_BEGIN);

		mNumFragments = 0;
		setActive(true);
	}

	void D3D9OcclusionQuery::end()
	{
		mQuery->Issue(D3DISSUE_END);
	}

	bool D3D9OcclusionQuery::isReady() const
	{
		BOOL queryData;
		return mQuery->GetData(&queryData, sizeof(BOOL), 0) == S_OK;
	}

	UINT32 D3D9OcclusionQuery::getNumFragments()
	{
		if (!mFinalized && isReady())
		{
			finalize();
		}

		return mNumFragments;
	}

	void D3D9OcclusionQuery::finalize()
	{
		mFinalized = true;

		DWORD numFragments;
		mQuery->GetData(&numFragments, sizeof(DWORD), 0);

		mNumFragments = (UINT32)numFragments;
	}
}