#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptEditorBuiltin : public ScriptObject <ScriptEditorBuiltin>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorBuiltin")

	private:
		static MonoObject* internal_getFolderIcon();
		static MonoObject* internal_getMeshIcon();
		static MonoObject* internal_getFontIcon();
		static MonoObject* internal_getTextureIcon();

		ScriptEditorBuiltin(MonoObject* instance);
	};
}