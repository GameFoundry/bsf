#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
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
		EventQuery()
			:mActive(false) {}
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

		/**
		 * @brief	Triggered when GPU starts processing the query.
		 */
		Event<void()> onTriggered;

		/**
		 * @brief	Creates a new query, but does not schedule it on GPU.
		 */
		static EventQueryPtr create();

	protected:
		friend class QueryManager;

		/**
		 * @brief	Returns true if the has still not been completed by the GPU.
		 */
		bool isActive() const { return mActive; }
		void setActive(bool active) { mActive = active; }

	protected:
		bool mActive;
	};
}