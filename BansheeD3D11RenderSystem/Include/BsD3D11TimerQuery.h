#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTimerQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc TimerQuery
	 */
	class BS_D3D11_EXPORT D3D11TimerQuery : public TimerQuery
	{
	public:
		D3D11TimerQuery();
		~D3D11TimerQuery();

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

	private:
		bool mFinalized;
		bool mQueryEndCalled;
		float mTimeDelta;

		ID3D11Query* mBeginQuery;
		ID3D11Query* mEndQuery;
		ID3D11Query* mDisjointQuery;
		ID3D11DeviceContext* mContext;

		void finalize();
	};
}