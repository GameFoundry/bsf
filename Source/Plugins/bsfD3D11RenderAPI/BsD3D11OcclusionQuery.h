//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "RenderAPI/BsOcclusionQuery.h"

namespace bs { namespace ct
{
	/** @addtogroup D3D11
	 *  @{
	 */

	/** @copydoc OcclusionQuery */
	class D3D11OcclusionQuery : public OcclusionQuery
	{
	public:
		D3D11OcclusionQuery(bool binary, UINT32 deviceIdx);
		~D3D11OcclusionQuery();

		/** @copydoc OcclusionQuery::begin */
		void begin(const SPtr<CommandBuffer>& cb = nullptr) override;

		/** @copydoc OcclusionQuery::end */
		void end(const SPtr<CommandBuffer>& cb = nullptr) override;

		/** @copydoc OcclusionQuery::isReady */
		bool isReady() const override;

		/** @copydoc OcclusionQuery::getNumSamples */
		UINT32 getNumSamples() override;

	private:
		friend class QueryManager;

		/** Resolves query results after it is ready. */
		void finalize();

		ID3D11Query* mQuery = nullptr;
		ID3D11DeviceContext* mContext = nullp;
		bool mFinalized = false;
		bool mQueryEndCalled = false;

		UINT32 mNumSamples = 0;
	};

	/** @} */
}}