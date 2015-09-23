#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsProjectLibrary.h"
#include "BsScriptResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ProjectLibrary.
	 */
	class BS_SCR_BED_EXPORT ScriptProjectLibrary : public ScriptObject<ScriptProjectLibrary>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "ProjectLibrary")

		/**
		 * @brief	Initializes the project library callbacks. Must be called on library load.
		 */
		void static startUp();

		/**
		 * @brief	Cleans up project library callbacks. Must be called before library shutdown.
		 */
		void static shutDown();

	private:
		ScriptProjectLibrary(MonoObject* instance);

		/**
		 * @brief	Triggered when a new entry has been added to the library.
		 *
		 * @param	path	Absolute path to the new entry.
		 */
		static void onEntryAdded(const Path& path);

		/**
		 * @brief	Triggered when a new entry has been removed to the library.
		 *
		 * @param	path	Absolute path to the removed entry.
		 */
		static void onEntryRemoved(const Path& path);

		static HEvent mOnEntryAddedConn;
		static HEvent mOnEntryRemovedConn;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		typedef void(__stdcall *OnEntryChangedThunkDef) (MonoString*, MonoException**);

		static OnEntryChangedThunkDef OnEntryAddedThunk;
		static OnEntryChangedThunkDef OnEntryRemovedThunk;

		static MonoArray* internal_Refresh(MonoString* path, bool import);
		static void internal_Create(MonoObject* resource, MonoString* path);
		static MonoObject* internal_Load(MonoString* path);
		static void internal_Save(MonoObject* resource);
		static MonoObject* internal_GetRoot();
		static void internal_Reimport(MonoString* path, MonoObject* options, bool force);
		static MonoObject* internal_GetEntry(MonoString* path);
		static MonoString* internal_GetPathFromUUID(MonoString* uuid);
		static MonoString* internal_GetPath(MonoObject* resource);
		static MonoArray* internal_Search(MonoString* pattern, MonoArray* types);
		static void internal_Delete(MonoString* path);
		static void internal_CreateFolder(MonoString* path);
		static void internal_Rename(MonoString* path, MonoString* name, bool overwrite);
		static void internal_Move(MonoString* oldPath, MonoString* newPath, bool overwrite);
		static void internal_Copy(MonoString* source, MonoString* destination, bool overwrite);
		static MonoString* internal_GetResourceFolder();
		static void internal_SetIncludeInBuild(MonoString* path, bool include);
	};

	/**
	 * @brief	Base class for C++/CLR interop objects used for wrapping
	 *			LibraryEntry implementations.
	 */
	class BS_SCR_BED_EXPORT ScriptLibraryEntryBase : public ScriptObjectBase
	{
	public:
		/**
		 * @brief	Returns the asset path of the library entry.
		 */
		const Path& getAssetPath() const { return mAssetPath; }

	protected:
		ScriptLibraryEntryBase(MonoObject* instance);
		virtual ~ScriptLibraryEntryBase() {}

		Path mAssetPath;
	};

	/**
	 * @brief	Interop class between C++ & CLR for LibraryEntry.
	 */
	class BS_SCR_BED_EXPORT ScriptLibraryEntry : public ScriptObject <ScriptLibraryEntry>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "LibraryEntry")

	private:
		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoString* internal_GetPath(ScriptLibraryEntryBase* thisPtr);
		static MonoString* internal_GetName(ScriptLibraryEntryBase* thisPtr);
		static ProjectLibrary::LibraryEntryType internal_GetType(ScriptLibraryEntryBase* thisPtr);
		static MonoObject* internal_GetParent(ScriptLibraryEntryBase* thisPtr);
	};

	/**
	 * @brief	Interop class between C++ & CLR for DirectoryEntry.
	 */
	class BS_SCR_BED_EXPORT ScriptDirectoryEntry : public ScriptObject <ScriptDirectoryEntry, ScriptLibraryEntryBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "DirectoryEntry")

		ScriptDirectoryEntry(MonoObject* instance, const Path& assetPath);

		/**
		 * @brief	Creates a new interop object that wraps the provided
		 *			native directory entry object.
		 */
		static MonoObject* create(const ProjectLibrary::DirectoryEntry* entry);

	private:
		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoArray* internal_GetChildren(ScriptDirectoryEntry* thisPtr);
	};

	/**
	 * @brief	Interop class between C++ & CLR for ResourceEntry.
	 */
	class BS_SCR_BED_EXPORT ScriptFileEntry : public ScriptObject <ScriptFileEntry, ScriptLibraryEntryBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "FileEntry")

		ScriptFileEntry(MonoObject* instance, const Path& assetPath);

		/**
		 * @brief	Creates a new interop object that wraps the provided
		 *			native resource entry object.
		 */
		static MonoObject* create(const ProjectLibrary::ResourceEntry* entry);

	private:
		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_GetImportOptions(ScriptFileEntry* thisPtr);
		static MonoString* internal_GetUUID(ScriptFileEntry* thisPtr);
		static MonoObject* internal_GetIcon(ScriptFileEntry* thisPtr);
		static ScriptResourceType internal_GetResourceType(ScriptFileEntry* thisPtr);
		static bool internal_GetIncludeInBuild(ScriptFileEntry* thisPtr);
	};
}