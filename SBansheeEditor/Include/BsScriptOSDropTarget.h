#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptOSDropTarget : public ScriptObject <ScriptOSDropTarget>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "OSDropTarget")

	private:
		ScriptEditorWindow* mParent;
		OSDropTarget* mDropTarget;
		Rect2I mParentArea;
		Rect2I mArea;
		bool mIsDestroyed;

		HEvent mDropTargetEnterConn;
		HEvent mDropTargetMoveConn;
		HEvent mDropTargetLeaveConn;
		HEvent mDropTargetDroppedConn;
		HEvent mWidgetParentChangedConn;
		HEvent mWidgetMovedConn;
		HEvent mWidgetResizedConn;

		ScriptOSDropTarget(MonoObject* instance, ScriptEditorWindow* parent);
		~ScriptOSDropTarget();

		void destroy();
		void setDropTarget(const RenderWindowPtr& parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		void setBounds(const Rect2I& bounds);

		void widgetParentChanged(EditorWidgetContainer* parent);
		void widgetResized(UINT32 width, UINT32 height);
		void widgetMoved(INT32 x, INT32 y);

		EditorWidgetBase* getParentWidget() const;
		Rect2I getDropTargetArea() const;

		typedef void(__stdcall *OnEnterThunkDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnMoveDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnLeaveDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDropThunkDef) (MonoObject*, INT32, INT32, MonoException**);

		static void internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* editorWindow);
		static void internal_Destroy(ScriptOSDropTarget* nativeInstance);
		static void internal_SetBounds(ScriptOSDropTarget* nativeInstance, Rect2I bounds);
		static MonoArray* internal_GetFilePaths(ScriptOSDropTarget* nativeInstance);

		static void dropTargetDragEnter(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);
		static void dropTargetDragMove(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);
		static void dropTargetDragLeave(ScriptOSDropTarget* thisPtr);
		static void dropTargetDragDropped(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);

		static OnEnterThunkDef onEnterThunk;
		static OnMoveDef onMoveThunk;
		static OnLeaveDef onLeaveThunk;
		static OnDropThunkDef onDropThunk;
	};
}