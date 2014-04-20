#pragma once

#include "CmGLPrerequisites.h"
#include "CmTimerQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc TimerQuery
	 */
	class CM_RSGL_EXPORT GLTimerQuery : public TimerQuery
	{
	public:
		GLTimerQuery();
		~GLTimerQuery();

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
		friend class QueryManager;

		GLuint mQueryStartObj;
		GLuint mQueryEndObj;
		bool mFinalized;

		float mTimeDelta;

		virtual void finalize();
	};
}