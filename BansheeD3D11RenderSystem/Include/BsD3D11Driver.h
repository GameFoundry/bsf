#pragma once

#include "BsD3D11Prerequisites.h"

namespace BansheeEngine
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
		IDXGIAdapter* getDeviceAdapter() { return mDXGIAdapter; }
		DXGI_OUTPUT_DESC getOutputDesc(UINT32 adapterOutputIdx) const;

		VideoModeInfoPtr getVideoModeInfo() const { return mVideoModeInfo; }

	private:
		void init();

	private:
		UINT32 mAdapterNumber;
		UINT32 mNumOutputs;
		DXGI_ADAPTER_DESC mAdapterIdentifier;
		IDXGIAdapter* mDXGIAdapter;
		VideoModeInfoPtr mVideoModeInfo;
	};
}