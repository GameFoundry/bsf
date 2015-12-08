#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Resources & GameResourceManager.
	 */
	class BS_SCR_BE_EXPORT ScriptResources : public ScriptObject<ScriptResources>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Resources")

	private:
		ScriptResources(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_Load(MonoString* path);
		static MonoObject* internal_LoadRef(MonoObject* reference);
		static void internal_UnloadUnused();
	};
}