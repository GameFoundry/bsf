//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsTimerQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc TimerQuery
	 */
	class BS_D3D9_EXPORT D3D9TimerQuery : public TimerQuery, public D3D9Resource
	{
	public:
		D3D9TimerQuery();
		~D3D9TimerQuery();

		/**
		 * @copydoc TimerQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc TimerQuery::end
		 */
		virtual void end();

		/**
		 * @copydoc TimerQuery::isReady
		 */
		virtual bool isReady() const;

		/**
		 * @copydoc TimerQuery::getTimeMs
		 */
		virtual float getTimeMs();

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceLost
		 */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceReset
		 */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

	private:
		/**
		 * @brief	Resolve timing information after the query has finished.
		 */
		void finalize();

		/**
		 * @brief	Creates the internal DX9 query.
		 */
		void createQuery();

		/**
		 * @brief	Releases the internal DX9 query.
		 */
		void releaseQuery();

		/**
		 * @brief	Checks if the internal query object is valid.
		 */
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
}