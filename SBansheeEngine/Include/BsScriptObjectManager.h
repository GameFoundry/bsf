#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptObjectManager : public Module <ScriptObjectManager>
	{
	public:
		ScriptObjectManager();

		void registerScriptObject(ScriptObjectBase* instance);
		void unregisterScriptObject(ScriptObjectBase* instance);

		void refreshAssemblies();

		/**
		 * @brief	Triggered right after a domain was reloaded. This signals the outside world that they should
		 *			update any kept Mono references as the old ones will no longer be valid.
		 */
		Event<void()> onRefreshDomainLoaded;

		Event<void()> onRefreshComplete;
	private:
		Set<ScriptObjectBase*> mScriptObjects;
	};
}