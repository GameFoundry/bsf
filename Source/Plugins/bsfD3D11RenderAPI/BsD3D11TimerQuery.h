//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "RenderAPI/BsTimerQuery.h"

namespace bs { namespace ct
{
	/** @addtogroup D3D11
	 *  @{
	 */

	/** @copydoc TimerQuery */
	class D3D11TimerQuery : public TimerQuery
	{
	public:
		D3D11TimerQuery(UINT32 deviceIdx);
		~D3D11TimerQuery();

		/** @copydoc TimerQuery::begin */
		void begin(const SPtr<CommandBuffer>& cb = nullptr) override;

		/** @copydoc TimerQuery::end */
		void end(const SPtr<CommandBuffer>& cb = nullptr) override;

		/** @copydoc TimerQuery::isReady */
		bool isReady() const override;

		/** @copydoc TimerQuery::getTimeMs */
		float getTimeMs() override;

	private:
		/**	Resolve timing information after the query has finished. */
		void finalize();

		bool mFinalized = false;
		bool mQueryEndCalled = false;
		float mTimeDelta = 0.0f;

		ID3D11Query* mBeginQuery = nullptr;
		ID3D11Query* mEndQuery = nullptr;
		ID3D11Query* mDisjointQuery = nullptr;
		ID3D11DeviceContext* mContext = nullptr;
	};

	/** @} */
}}