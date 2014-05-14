#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	* @copydoc OcclusionQuery
	*/
	class CM_D3D9_EXPORT D3D9OcclusionQuery : public OcclusionQuery
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
		virtual UINT32 getNumFragments();

	private:
		friend class QueryManager;

		IDirect3DQuery9* mQuery;
		bool mFinalized;

		UINT32 mNumFragments;

		void finalize();
	};
}