#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptDropTarget.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptDropTarget : public ScriptObject <ScriptDropTarget>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "DropTarget")

	private:
		OSDropTarget* mDropTarget;
		Rect2I mParentArea;
		Rect2I mArea;
		bool mIsDestroyed;

		HEvent mDropTargetEnterConn;
		HEvent mDropTargetMoveConn;
		HEvent mDropTargetLeaveConn;
		HEvent mDropTargetDroppedConn;
		HEvent mWidgetParentChangedConn;
		HEvent mWidgetResizedConn;

		ScriptDropTarget(MonoObject* instance, EditorWidgetBase* parent);
		~ScriptDropTarget();

		void destroy();
		void setDropTarget(const RenderWindowPtr& parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		void setBounds(const Rect2I& bounds);

		void widgetParentChanged(EditorWidgetContainer* parent);
		void widgetResized(UINT32 width, UINT32 height);

		typedef void(__stdcall *OnEnterThunkDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnMoveDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnLeaveDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDropThunkDef) (MonoObject*, INT32, INT32, MonoException**);

		static void internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* editorWindow);
		static void internal_Destroy(ScriptDropTarget* nativeInstance);
		static void internal_SetBounds(ScriptDropTarget* nativeInstance, Rect2I bounds);
		static MonoArray* internal_GetFilePaths(ScriptDropTarget* nativeInstance);

		static void dropTargetDragEnter(MonoObject* instance, INT32 x, INT32 y);
		static void dropTargetDragMove(MonoObject* instance, INT32 x, INT32 y);
		static void dropTargetDragLeave(MonoObject* instance);
		static void dropTargetDragDropped(MonoObject* instance, INT32 x, INT32 y);

		static OnEnterThunkDef onEnterThunk;
		static OnMoveDef onMoveThunk;
		static OnLeaveDef onLeaveThunk;
		static OnDropThunkDef onDropThunk;
	};
}