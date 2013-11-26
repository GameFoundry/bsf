#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmTimerQuery.h"

namespace CamelotFramework
{
	/**
	 * @copydoc TimerQuery
	 */
	class CM_D3D9_EXPORT D3D9TimerQuery : public TimerQuery
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
		virtual UINT64 getTimeMs() const;

	private:
		bool mInitialized;
	};
}