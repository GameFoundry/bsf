#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsEngineScriptLibrary.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles initialization/shutdown of the script systems and 
	 *			loading/refresh of engine and editor-specific assemblies.
	 */
	class BS_SCR_BED_EXPORT EditorScriptLibrary : public EngineScriptLibrary
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