#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for SceneManager.
	 */
	class BS_SCR_BE_EXPORT ScriptScene : public ScriptObject<ScriptScene>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Scene")

	private:
		ScriptScene(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_LoadScene(MonoString* path);
		static MonoObject* internal_GetRoot();
		static void internal_ClearScene();
		static MonoObject* internal_GetMainCameraSO();
	};
}