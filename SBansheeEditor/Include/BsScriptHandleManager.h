#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsHandleManager.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleManager : public HandleManager
	{
	protected:
		void refreshHandles();
		void triggerHandles();
		void queueDrawCommands();
	};
}