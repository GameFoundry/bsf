#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	* @copydoc OcclusionQuery
	*/
	class BS_D3D11_EXPORT D3D11OcclusionQuery : public OcclusionQuery
	{
	public:
		D3D11OcclusionQuery(bool binary);
		~D3D11OcclusionQuery();

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

	private:
		friend class QueryManager;

		ID3D11Query* mQuery;
		ID3D11DeviceContext* mContext;
		bool mFinalized;

		UINT32 mNumSamples;

		void finalize();
	};
}