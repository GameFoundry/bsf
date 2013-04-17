#pragma once

#include "CmD3D11Prerequisites.h"

namespace CamelotFramework
{
	class D3D11Driver
	{
	public:
		D3D11Driver(const D3D11Driver &ob);
		D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* dxgiAdapter);
		~D3D11Driver();

		D3D11Driver& operator=(const D3D11Driver& r);

		String getDriverName() const;
		String getDriverDescription() const;
		UINT32 getAdapterNumber() const { return mAdapterNumber; }
		UINT32 getNumAdapterOutputs() const { return mNumOutputs; }
		const DXGI_ADAPTER_DESC& getAdapterIdentifier() const { return mAdapterIdentifier; }
		const DXGI_MODE_DESC& getDesktopMode() const { return mDesktopDisplayMode; }
		IDXGIAdapter* getDeviceAdapter() { return mDXGIAdapter; }
		DXGI_OUTPUT_DESC getOutputDesc(UINT32 adapterOutputIdx) const;
		const D3D11VideoModeList* getVideoModeList(UINT32 adapterOutputIdx) const;

	private:
		// D3D only allows one device per adapter, so it can safely be stored here as well.
		UINT32					mAdapterNumber;
		UINT32					mNumOutputs;
		DXGI_ADAPTER_DESC		mAdapterIdentifier;
		DXGI_MODE_DESC			mDesktopDisplayMode;
		D3D11VideoModeList**	mVideoModeList;
		IDXGIAdapter*			mDXGIAdapter;

		void init();
	};
}