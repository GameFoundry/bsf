//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsOcclusionQuery.h"

namespace BansheeEngine
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/** @copydoc OcclusionQuery */
	class BS_D3D9_EXPORT D3D9OcclusionQuery : public OcclusionQuery, public D3D9Resource
	{
	public:
		D3D9OcclusionQuery(bool binary);
		~D3D9OcclusionQuery();

		/** @copydoc OcclusionQuery::begin */
		void begin() override;

		/** @copydoc OcclusionQuery::end */
		void end() override;

		/** @copydoc OcclusionQuery::isReady */
		bool isReady() const override;

		/** @copydoc OcclusionQuery::getNumSamples */
		UINT32 getNumSamples() override;

		/** @copydoc D3D9Resource::notifyOnDeviceCreate */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceDestroy */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceLost */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceReset */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) override;

	private:
		friend class QueryManager;

		/**	Creates the internal DX9 query. */
		void createQuery();

		/**	Releases the internal DX9 query. */
		void releaseQuery();

		/**	Resolves query results after it is ready. */
		void finalize();
	private:
		IDirect3DDevice9* mDevice;
		IDirect3DQuery9* mQuery;
		bool mQueryIssued;
		bool mFinalized;

		UINT32 mNumSamples;
	};

	/** @} */
}