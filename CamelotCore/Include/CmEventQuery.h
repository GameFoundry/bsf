#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	/**
	 * @brief	Represents a GPU query that gets triggered when GPU starts processing the query.
	 * 			
	 * @note	Normally GPU will have many commands in its command buffer. When EventQuery::begin is called it is placed
	 * 			in that command buffer. Once the buffer empties and GPU reaches the EventQuery command, the query
	 * 			callback is triggered.
	 * 			
	 *			Core thread only.
	 */
	class CM_EXPORT EventQuery
	{
	public:
		virtual ~EventQuery() {}

		/**
		 * @brief	Starts the query. 
		 * 			
		 * @note	Once the query is started you may poll "isReady" method to check when query has finished,
		 * 			or you may hook up an "onTriggered" callback and be notified that way.
		 */
		virtual void begin() = 0;

		/**
		 * @brief	Check if GPU has processed the query.
		 */
		virtual bool isReady() const = 0;

		boost::signal<void()> onTriggered;

		static EventQueryPtr create();
	};
}