#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotFramework
{
	/**
	 * @brief	Allows you to queue calls that can get executed later.
	 * 			
	 * @note	Sim thread only.
	 */
	class CM_EXPORT DeferredCallManager : public Module<DeferredCallManager>
	{
	public:
		DeferredCallManager();

		/**
		 * @brief	Register a deferred call that will be executed once at the start of next frame.
		 *
		 * @param	func		The function to execute.
		 */
		void queueDeferredCall(std::function<void()> func);

		/**
		 * @brief	Executes all the scheduled calls.
		 */
		void update();

	private:
		friend class DeferredCall;

		Vector<std::function<void()>>::type mCallbacks;
	};
}