//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a GPU query that measures execution time of GPU operations. The query will measure
	 * 			any GPU operations that take place between its "begin" and "end" calls.
	 * 			
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT TimerQuery
	{
	public:
		virtual ~TimerQuery() {}

		/**
		 * @brief	Starts the counter. 
		 * 			
		 * @note	Place any commands you want to measure after this call. Call "end" when done.
		 */
		virtual void begin() = 0;

		/**
		 * @brief	Stops the counter.
		 */
		virtual void end() = 0;

		/**
		 * @brief	Check if GPU has processed the query.
		 */
		virtual bool isReady() const = 0;

		/**
		 * @brief	Returns the time it took for the query to execute.
		 *
		 * @return	The time milliseconds.
		 * 			
		 * @note	Only valid after "isReady" returns true.
		 */
		virtual float getTimeMs() = 0;

		/**
		* @brief	Triggered when GPU processes the query. As a parameter
		*			it provides query duration in milliseconds.
		*/
		Event<void(float)> onTriggered;

		/**
		 * @brief	Creates a new query, but does not schedule it on GPU.
		 */
		static TimerQueryPtr create();

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