//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Provides information about a driver (e.g. hardware GPU driver or software emulated).
	 */
	class D3D11Driver
	{
	public:
		/**
		 * @brief	Constructs a new object from the adapter number provided by DX11 runtime, and
		 *			DXGI adapter object.
		 */
		D3D11Driver(UINT32 adapterNumber, IDXGIAdapter* dxgiAdapter);
		D3D11Driver(const D3D11Driver &ob);
		~D3D11Driver();

		D3D11Driver& operator=(const D3D11Driver& r);

		/**
		 * @brief	Returns the name of the driver.
		 */
		String getDriverName() const;

		/**
		 * @brief	Returns the description of the driver.
		 */
		String getDriverDescription() const;

		/**
		 * @brief	Returns adapter index of the adapter the driver is managing.
		 */
		UINT32 getAdapterNumber() const { return mAdapterNumber; }

		/**
		 * @brief	Returns number of outputs connected to the adapter the driver is managing.
		 */
		UINT32 getNumAdapterOutputs() const { return mNumOutputs; }

		/**
		 * @brief	Returns a description of the adapter the driver is managing.
		 */
		const DXGI_ADAPTER_DESC& getAdapterIdentifier() const { return mAdapterIdentifier; }

		/**
		 * @brief	Returns internal DXGI adapter object for the driver.
		 */
		IDXGIAdapter* getDeviceAdapter() { return mDXGIAdapter; }

		/**
		 * @brief	Returns description of an output device at the specified index.
		 */
		DXGI_OUTPUT_DESC getOutputDesc(UINT32 adapterOutputIdx) const;

		/**
		 * @brief	Returns a list of all available video modes for all output devices.
		 */
		VideoModeInfoPtr getVideoModeInfo() const { return mVideoModeInfo; }

	private:
		/**
		 * @brief	Initializes the internal data.
		 */
		void construct();

	private:
		UINT32 mAdapterNumber;
		UINT32 mNumOutputs;
		DXGI_ADAPTER_DESC mAdapterIdentifier;
		IDXGIAdapter* mDXGIAdapter;
		VideoModeInfoPtr mVideoModeInfo;
	};
}