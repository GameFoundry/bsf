#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles initialization/shutdown of the script systems and 
	 *			loading/refresh of engine-specific assemblies.
	 */
	class BS_SCR_BE_EXPORT EngineScriptLibrary : public ScriptLibrary
	{
	public:
		/**
		 * @copydoc	ScriptLibrary::initialize
		 */
		void initialize() override;

		/**
		 * @copydoc	ScriptLibrary::reload
		 */
		void reload() override;

		/**
		 * @copydoc	ScriptLibrary::destroy
		 */
		void destroy() override;
	};
}