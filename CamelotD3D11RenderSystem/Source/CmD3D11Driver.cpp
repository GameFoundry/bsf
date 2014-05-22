#include "CmD3D11Driver.h"
#include "CmD3D11VideoModeInfo.h"
#include "CmException.h"

namespace BansheeEngine
{
	D3D11Driver::D3D11Driver(const D3D11Driver &ob) 
	{
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;
		mDXGIAdapter = ob.mDXGIAdapter;

		if(mDXGIAdapter)
			mDXGIAdapter->AddRef();

		init();
	}

	D3D11Driver::D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* pDXGIAdapter)
	{
		mAdapterNumber = adapterNumber;
		mDXGIAdapter = pDXGIAdapter;

		if(mDXGIAdapter)
			mDXGIAdapter->AddRef();

		pDXGIAdapter->GetDesc(&mAdapterIdentifier);

		init();
	}

	D3D11Driver::~D3D11Driver()
	{
		SAFE_RELEASE(mDXGIAdapter);
	}

	void D3D11Driver::init()
	{
		assert(mDXGIAdapter != nullptr);

		UINT32 outputIdx = 0;
		IDXGIOutput* output = nullptr;
		while(mDXGIAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND)
		{
			outputIdx++;
			SAFE_RELEASE(output);
		}

		mNumOutputs = outputIdx;

		mVideoModeInfo = cm_shared_ptr<D3D11VideoModeInfo>(mDXGIAdapter);
	}

	D3D11Driver& D3D11Driver::operator=(const D3D11Driver& ob)
	{
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;

		if(ob.mDXGIAdapter)
			ob.mDXGIAdapter->AddRef();

		SAFE_RELEASE(mDXGIAdapter);
		mDXGIAdapter=ob.mDXGIAdapter;

		return *this;
	}
	
	String D3D11Driver::getDriverName() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char* str = (char*)cm_alloc<ScratchAlloc>((UINT32)(size + 1));

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		String Description = str;
		
		cm_free<ScratchAlloc>(str);
		return String(Description );
	}

	String D3D11Driver::getDriverDescription() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char* str = (char*)cm_alloc<ScratchAlloc>((UINT32)(size + 1));

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		String driverDescription = str;

		cm_free<ScratchAlloc>(str);
		StringUtil::trim(driverDescription);

		return driverDescription;
	}

	DXGI_OUTPUT_DESC D3D11Driver::getOutputDesc(UINT32 adapterOutputIdx) const
	{
		DXGI_OUTPUT_DESC desc;
		
		IDXGIOutput* output = nullptr;
		if(mDXGIAdapter->EnumOutputs(adapterOutputIdx, &output) == DXGI_ERROR_NOT_FOUND)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot find output with the specified index: " + toString(adapterOutputIdx));
		}

		output->GetDesc(&desc);

		SAFE_RELEASE(output);

		return desc;
	}
}