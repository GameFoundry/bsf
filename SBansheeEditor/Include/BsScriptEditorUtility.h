#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for EditorUtility.
	 */
	class BS_SCR_BED_EXPORT ScriptEditorUtility : public ScriptObject <ScriptEditorUtility>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorUtility")

	private:
		ScriptEditorUtility(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CalculateBounds(MonoObject* so, AABox* bounds);
		static void internal_CalculateBoundsArray(MonoArray* objects, AABox* bounds);
	};
}