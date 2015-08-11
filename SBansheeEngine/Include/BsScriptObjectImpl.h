#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ScriptObject.
	 */
	class BS_SCR_BE_EXPORT ScriptObjectImpl : public ScriptObject<ScriptObjectImpl>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ScriptObject")

	private:
		ScriptObjectImpl(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_managedInstanceDeleted(ScriptObjectBase* instance);
	};
}