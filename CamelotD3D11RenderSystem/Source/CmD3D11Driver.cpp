#include "CmD3D11Driver.h"
#include "CmD3D11VideoModeList.h"

namespace CamelotEngine
{
	D3D11Driver::D3D11Driver( const D3D11Driver &ob ) 
	{
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;
		mDesktopDisplayMode = ob.mDesktopDisplayMode;
		mVideoModeList = nullptr;
		mDXGIAdapter = ob.mDXGIAdapter;

		if(mDXGIAdapter)
			mDXGIAdapter->AddRef();

		init();
	}

	D3D11Driver::D3D11Driver(unsigned int adapterNumber, IDXGIAdapter* pDXGIAdapter)
	{
		mAdapterNumber = adapterNumber;
		mVideoModeList = nullptr;
		mDXGIAdapter = pDXGIAdapter;

		if(mDXGIAdapter)
			mDXGIAdapter->AddRef();

		// get the description of the adapter
		pDXGIAdapter->GetDesc(&mAdapterIdentifier);

		init();
	}

	D3D11Driver::~D3D11Driver()
	{
		for(UINT32 i = 0; i < mNumOutputs; i++)
		{
			if(mVideoModeList[i] != nullptr)
				CM_DELETE(mVideoModeList[i], D3D11VideoModeList, GenAlloc);
		}

		if(mVideoModeList != nullptr)
			CM_DELETE_ARRAY(mVideoModeList, D3D11VideoModeList*, mNumOutputs, GenAlloc);

		SAFE_RELEASE(mDXGIAdapter);
	}

	void D3D11Driver::init()
	{
		assert(mDXGIAdapter != nullptr);

		UINT32 outputIdx = 0;
		IDXGIOutput* output;
		while(mDXGIAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND)
		{
			outputIdx++;
		}

		mNumOutputs = outputIdx;

		mVideoModeList = CM_NEW_ARRAY(D3D11VideoModeList*, mNumOutputs, GenAlloc);
		for(UINT32 i = 0; i < mNumOutputs; i++)
			mVideoModeList[i] = CM_NEW(D3D11VideoModeList, GenAlloc) D3D11VideoModeList(this, i);
	}

	D3D11Driver& D3D11Driver::operator=(const D3D11Driver& ob)
	{
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;
		mDesktopDisplayMode = ob.mDesktopDisplayMode;
		mVideoModeList = nullptr;

		if(ob.mDXGIAdapter)
			ob.mDXGIAdapter->AddRef();

		SAFE_RELEASE(mDXGIAdapter);
		mDXGIAdapter=ob.mDXGIAdapter;

		return *this;
	}
	
	String D3D11Driver::getDriverName() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char* str = (char*)CM_NEW_BYTES((UINT32)(size + 1), ScratchAlloc);

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		String Description = str;
		
		CM_DELETE_BYTES(str, ScratchAlloc);
		return String(Description );
	}

	String D3D11Driver::getDriverDescription() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char* str = (char*)CM_NEW_BYTES((UINT32)(size + 1), ScratchAlloc);

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		String driverDescription = str;

		CM_DELETE_BYTES(str, ScratchAlloc);
		StringUtil::trim(driverDescription);

		return driverDescription;
	}

	const D3D11VideoModeList* D3D11Driver::getVideoModeList(UINT32 adapterOutputIdx) const
	{
		assert(adapterOutputIdx >= 0 && adapterOutputIdx < mNumOutputs);

		return mVideoModeList[adapterOutputIdx];
	}
}