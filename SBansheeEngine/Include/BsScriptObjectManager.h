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

		Event<void()> onRefreshComplete;
	private:
		Set<ScriptObjectBase*> mScriptObjects;
	};
}