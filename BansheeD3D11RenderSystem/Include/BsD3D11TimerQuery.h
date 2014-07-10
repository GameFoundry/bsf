//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		/**
		 * @brief	Resolve timing information after the query has finished.
		 */
		void finalize();

		bool mFinalized;
		bool mQueryEndCalled;
		float mTimeDelta;

		ID3D11Query* mBeginQuery;
		ID3D11Query* mEndQuery;
		ID3D11Query* mDisjointQuery;
		ID3D11DeviceContext* mContext;
	};
}