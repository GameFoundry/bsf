#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsProjectLibrary.h"
#include "BsScriptResource.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptProjectLibrary : public ScriptObject<ScriptProjectLibrary>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "ProjectLibrary")

		void static startUp();
		void static shutDown();

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

		static void onEntryAdded(const Path& path);
		static void onEntryRemoved(const Path& path);

		ScriptProjectLibrary(MonoObject* instance);

		typedef void(__stdcall *OnEntryChangedThunkDef) (MonoString*, MonoException**);

		static OnEntryChangedThunkDef OnEntryAddedThunk;
		static OnEntryChangedThunkDef OnEntryRemovedThunk;

		static HEvent mOnEntryAddedConn;
		static HEvent mOnEntryRemovedConn;
	};

	class BS_SCR_BED_EXPORT ScriptLibraryEntryBase : public ScriptObjectBase
	{
	public:
		const Path& getAssetPath() const { return mAssetPath; }

	protected:
		ScriptLibraryEntryBase(MonoObject* instance);
		virtual ~ScriptLibraryEntryBase() {}

		Path mAssetPath;
	};

	class BS_SCR_BED_EXPORT ScriptLibraryEntry : public ScriptObject <ScriptLibraryEntry>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "LibraryEntry")

	private:
		static MonoString* internal_GetPath(ScriptLibraryEntryBase* thisPtr);
		static MonoString* internal_GetName(ScriptLibraryEntryBase* thisPtr);
		static ProjectLibrary::LibraryEntryType internal_GetType(ScriptLibraryEntryBase* thisPtr);
		static MonoObject* internal_GetParent(ScriptLibraryEntryBase* thisPtr);
	};

	class BS_SCR_BED_EXPORT ScriptDirectoryEntry : public ScriptObject <ScriptDirectoryEntry, ScriptLibraryEntryBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "DirectoryEntry")

		ScriptDirectoryEntry(MonoObject* instance, const Path& assetPath);

		static MonoObject* create(const ProjectLibrary::DirectoryEntry* entry);

	private:
		static MonoArray* internal_GetChildren(ScriptDirectoryEntry* thisPtr);
	};

	class BS_SCR_BED_EXPORT ScriptFileEntry : public ScriptObject <ScriptFileEntry, ScriptLibraryEntryBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "FileEntry")

		ScriptFileEntry(MonoObject* instance, const Path& assetPath);

		static MonoObject* create(const ProjectLibrary::ResourceEntry* entry);

	private:
		static MonoObject* internal_GetImportOptions(ScriptFileEntry* thisPtr);
		static MonoString* internal_GetUUID(ScriptFileEntry* thisPtr);
		static MonoObject* internal_GetIcon(ScriptFileEntry* thisPtr);
		static ScriptResourceType internal_GetResourceType(ScriptFileEntry* thisPtr);
	};
}