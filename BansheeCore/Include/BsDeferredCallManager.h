//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to queue calls that can get executed later.
	 * 			
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT DeferredCallManager : public Module<DeferredCallManager>
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
		 * @brief	Executes all the scheduled calls. To be called once per frame.
		 *
		 * @note	Internal method.
		 */
		void _update();

	private:
		friend class DeferredCall;

		Vector<std::function<void()>> mCallbacks;
	};
}