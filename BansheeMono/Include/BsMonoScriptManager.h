#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptManager.h"
#include "BsModule.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles Mono and script system initialization and destruction.
	 */
	class BS_MONO_EXPORT MonoScriptSystem : public ScriptSystem
	{
	public:
		MonoScriptSystem();
		~MonoScriptSystem();

		/**
		 * @copydoc	ScriptSystem::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc	ScriptSystem::destroy
		 */
		virtual void destroy();

	private:
		bool mIsInitialized;
	};
}