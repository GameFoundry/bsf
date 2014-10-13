#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptProjectLibrary : public ScriptObject<ScriptProjectLibrary>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "ProjectLibrary")

	private:
		static void internal_Create(MonoObject* resource, MonoString* path);
		static MonoObject* internal_Load(MonoString* path);

		ScriptProjectLibrary(MonoObject* instance);
	};
}