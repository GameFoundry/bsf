#pragma once

#include "CmGLPrerequisites.h"
#include "CmEventQuery.h"

namespace CamelotFramework
{
	/**
	 * @copydoc EventQuery
	 */
	class CM_RSGL_EXPORT GLEventQuery : public EventQuery
	{
	public:
		GLEventQuery();
		~GLEventQuery();

		/**
		 * @copydoc EventQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc EventQuery::isReady
		 */
		virtual bool isReady() const;

	private:
		GLuint mQueryObj;
	};
}