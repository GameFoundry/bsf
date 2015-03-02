#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsDragAndDropManager.h"

namespace BansheeEngine
{
	class ScriptSceneObjectDragDropData;
	class ScriptResourceDragDropData;

	class BS_SCR_BED_EXPORT ScriptDragDrop : public ScriptObject<ScriptDragDrop>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "DragDrop");

	private:
		ScriptDragDrop(MonoObject* instance);

		static bool internal_IsDragInProgress();
		static bool internal_IsDropInProgress();
		static MonoObject* internal_GetData();
		static void internal_StartSceneObjectDrag(ScriptSceneObjectDragDropData* dragData);
		static void internal_StartResourceDrag(ScriptResourceDragDropData* dragData);
	};

	class BS_SCR_BED_EXPORT ScriptSceneObjectDragDropData : public ScriptObject<ScriptSceneObjectDragDropData>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "SceneObjectDragDropData");

		static MonoObject* create(const Vector<HSceneObject>& sceneObjects);

		const Vector<HSceneObject>& getSceneObjects() const { return mSceneObjects; }

	private:
		ScriptSceneObjectDragDropData(MonoObject* instance, const Vector<HSceneObject>& sceneObjects);

		static void internal_CreateInstance(MonoObject* instance, MonoArray* objects);
		static MonoArray* internal_GetObjects(ScriptSceneObjectDragDropData* instance);

		Vector<HSceneObject> mSceneObjects;
	};

	class BS_SCR_BED_EXPORT ScriptResourceDragDropData : public ScriptObject < ScriptResourceDragDropData >
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "ResourceDragDropData");

		static MonoObject* create(const Vector<Path>& paths);

		const Vector<Path>& getPaths() const { return mPaths; }

	private:
		ScriptResourceDragDropData(MonoObject* instance, const Vector<Path>& paths);

		static void internal_CreateInstance(MonoObject* instance, MonoArray* paths);
		static MonoArray* internal_GetPaths(ScriptResourceDragDropData* instance);

		Vector<Path> mPaths;
	};

	// Note: Must be equal to C# DragDropType enum
	enum class ScriptDragDropType
	{
		Resource,
		SceneObject,
		None
	};

	class BS_SCR_BED_EXPORT ScriptDragDropManager : public Module<ScriptDragDropManager>
	{
	public:
		ScriptDragDropManager();
		~ScriptDragDropManager();

		void update();

		bool isDropInProgress() const { return mIsDropInProgress; }
		MonoObject* getDropData() const;

	private:
		void onMouseDragEnded(const PointerEvent& evt, DragCallbackInfo& callbackInfo);

		HEvent mDragEndedConn;

		bool mIsDropInProgress;
		UINT64 mDroppedFrameIdx;
		ScriptDragDropType mDropType;
		Vector<Path> mDroppedPaths;
		Vector<HSceneObject> mDroppedSceneObjects;
	};
}