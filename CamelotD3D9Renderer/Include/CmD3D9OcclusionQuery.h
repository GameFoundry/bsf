#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmD3D9Resource.h"
#include "CmOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	* @copydoc OcclusionQuery
	*/
	class CM_D3D9_EXPORT D3D9OcclusionQuery : public OcclusionQuery, public D3D9Resource
	{
	public:
		D3D9OcclusionQuery(bool binary);
		~D3D9OcclusionQuery();

		/**
		* @copydoc OcclusionQuery::begin
		*/
		virtual void begin();

		/**
		* @copydoc OcclusionQuery::end
		*/
		virtual void end();

		/**
		* @copydoc OcclusionQuery::isReady
		*/
		virtual bool isReady() const;

		/**
		* @copydoc OcclusionQuery::getNumFragments
		*/
		virtual UINT32 getNumSamples();

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
		friend class QueryManager;

		/**
		 * @brief	Creates the internal DX9 query.
		 */
		void createQuery();

		/**
		 * @brief	Releases the internal DX9 query.
		 */
		void releaseQuery();

		/**
		 * @brief	Resolves query results after it is ready.
		 */
		void finalize();
	private:
		IDirect3DDevice9* mDevice;
		IDirect3DQuery9* mQuery;
		bool mQueryIssued;
		bool mFinalized;

		UINT32 mNumSamples;
	};
}