//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsTimerQuery.h"

namespace BansheeEngine
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/** @copydoc TimerQuery */
	class BS_D3D9_EXPORT D3D9TimerQuery : public TimerQuery, public D3D9Resource
	{
	public:
		D3D9TimerQuery();
		~D3D9TimerQuery();

		/** @copydoc TimerQuery::begin */
		void begin() override;

		/** @copydoc TimerQuery::end */
		void end() override;

		/** @copydoc TimerQuery::isReady */
		bool isReady() const override;

		/** @copydoc TimerQuery::getTimeMs */
		float getTimeMs() override;

		/** @copydoc D3D9Resource::notifyOnDeviceCreate */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceDestroy */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceLost */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceReset */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) override;

	private:
		/**	Resolve timing information after the query has finished. */
		void finalize();

		/**	Creates the internal DX9 query. */
		void createQuery();

		/**	Releases the internal DX9 query. */
		void releaseQuery();

		/**	Checks if the internal query object is valid. */
		bool isQueryValid() const;

	private:
		bool mFinalized;
		bool mQueryIssued;
		float mTimeDelta;

		IDirect3DDevice9* mDevice;
		IDirect3DQuery9* mBeginQuery;
		IDirect3DQuery9* mEndQuery;
		IDirect3DQuery9* mDisjointQuery;
		IDirect3DQuery9* mFreqQuery;
	};

	/** @} */
}