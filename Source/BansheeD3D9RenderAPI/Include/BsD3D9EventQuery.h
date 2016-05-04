//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsEventQuery.h"

namespace BansheeEngine
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/** @copydoc EventQuery */
	class BS_D3D9_EXPORT D3D9EventQuery : public EventQuery, public D3D9Resource
	{
	public:
		D3D9EventQuery();
		~D3D9EventQuery();

		/** @copydoc EventQuery::begin */
		void begin() override;

		/** @copydoc EventQuery::isReady */
		bool isReady() const override;

		/** @copydoc D3D9Resource::notifyOnDeviceCreate */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceDestroy */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceLost */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceReset */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) override;

	private:
		/**	Creates the internal DX9 query. */
		void createQuery();

		/**	Releases the internal DX9 query. */
		void releaseQuery();

	private:
		bool mQueryIssued;
		IDirect3DQuery9* mQuery;
		IDirect3DDevice9* mDevice;
	};

	/** @} */
}