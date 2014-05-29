#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsRectI.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptObjectImpl : public ScriptObject<ScriptObjectImpl>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "ScriptObject")

	private:
		ScriptObjectImpl(MonoObject* instance);

		static void internal_managedInstanceDeleted(ScriptObjectBase* instance);
	};
}