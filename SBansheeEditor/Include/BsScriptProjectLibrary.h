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
		static void internal_Save(MonoObject* resource);
		static MonoObject* internal_GetRoot();
		static void internal_Reimport(MonoString* path, MonoObject* options, bool force);
		static MonoObject* internal_GetEntry(MonoString* path);
		static MonoString* internal_GetPath(MonoObject* resource);
		static void internal_Delete(MonoString* path);
		static void internal_CreateFolder(MonoString* path);
		static void internal_Rename(MonoString* path, MonoString* name);
		static void internal_Move(MonoString* oldPath, MonoString* newPath, bool overwrite);
		static void internal_Copy(MonoString* source, MonoString* destination, bool overwrite);

		ScriptProjectLibrary(MonoObject* instance);
	};
}