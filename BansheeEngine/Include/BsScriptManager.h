//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Abstraction of a specific scripting system.
	 */
	class BS_EXPORT ScriptSystem
	{
	public:
		virtual ~ScriptSystem() { }

		/**
		 * @brief	Called when the script system is being activated.
		 */
		virtual void initialize() = 0;

		/**
		 * @brief	Called when the script system is being destryoed.
		 */
		virtual void destroy() = 0;
	};

	/**
	 * @brief	Handles initialization of a scripting system.
	 */
	class BS_EXPORT ScriptManager : public Module<ScriptManager>
	{
	public:
		ScriptManager() { }
		~ScriptManager() { }

		/**
		 * @brief	Initializes the script managed with the specified script system,
		 *			making it active. Should be called right after construction.
		 */
		void initialize(const std::shared_ptr<ScriptSystem>& scriptSystem);

		/**
		 * @brief	Destroys the currently active script system. Must be called just
		 *			before shutdown.
		 */
		void destroy();

	private:
		/**
		 * @copydoc	ScriptManager::onShutDown
		 */
		void onShutDown();

		std::shared_ptr<ScriptSystem> mScriptSystem;
	};
}