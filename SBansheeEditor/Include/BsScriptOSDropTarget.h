#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for OSDropTarget. Managed drop target
	 *			is always associated with a managed EditorWindow.
	 */
	class BS_SCR_BED_EXPORT ScriptOSDropTarget : public ScriptObject <ScriptOSDropTarget>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "OSDropTarget")

	private:
		ScriptOSDropTarget(MonoObject* instance, ScriptEditorWindow* parent);
		~ScriptOSDropTarget();

		/**
		 * @brief	Destroys the internal native drop target.
		 */
		void destroy();

		/**
		 * @brief	Creates an internal native drop target over the specified window. Any previous
		 *			drop target is overwritten.
		 * 
		 * @param	parentWindow	Window the drop target is located on.
		 * @param	x				X position of the drop target, relative to window, in pixels.
		 * @param	y				Y position of the drop target, relative to window, in pixels.
		 * @param	width			Width of the drop target in pixels.
		 * @param	height			Height of the drop target in pixels.
		 */
		void setDropTarget(const RenderWindowPtr& parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Updates bounds of an existing drop target.
		 *
		 * @param	bounds	Area of the drop target relative to 
		 *			the editor widget (EditorWindow in managed terms).
		 */
		void setBounds(const Rect2I& bounds);

		/**
		 * @brief	Triggered when editor widget (EditorWindow in managed terms) parent
		 *			changes. This might mean we need to re-create the drop target
		 *			as the parent render window might have changed.
		 */
		void widgetParentChanged(EditorWidgetContainer* parent);

		/**
		 * @brief	Triggered when the parent editor widget (EditorWindow in managed terms) is resized.
		 */
		void widgetResized(UINT32 width, UINT32 height);

		/**
		 * @brief	Triggered when the parent editor widget (EditorWindow in managed terms) is moved.
		 */
		void widgetMoved(INT32 x, INT32 y);

		/**
		 * @brief	Returns the editor widget (EditorWindow in managed terms) this drop target belongs to.
		 */
		EditorWidgetBase* getParentWidget() const;

		/**
		 * @brief	Returns the bounds of the drop target, relative to the parent window. This depends
		 *			of set bounds using ::setBounds and the current position and size of the editor widget.
		 */
		Rect2I getDropTargetArea() const;

		/**
		 * @brief	Triggered when the drag and drop operation has entered the area over an OS drop target.
		 *
		 * @param	thisPtr		C++/CLR interop object that contains the native OSDropTarget
		 *						that triggered the event.
		 * @param	x			X coordinate of the pointer, relative to parent window, in pixels.
		 * @param	y			Y coordinate of the pointer, relative to parent window, in pixels.
		 */
		static void dropTargetDragEnter(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);

		/**
		 * @brief	Triggered every frame that pointer moves while over the area over an OS drop target.
		 *
		 * @param	thisPtr		C++/CLR interop object that contains the native OSDropTarget
		 *						that triggered the event.
		 * @param	x			X coordinate of the pointer, relative to parent window, in pixels.
		 * @param	y			Y coordinate of the pointer, relative to parent window, in pixels.
		 */
		static void dropTargetDragMove(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);

		/**
		 * @brief	Triggered when the drag and drop operation has left the area over an OS drop target.
		 *
		 * @param	thisPtr		C++/CLR interop object that contains the native OSDropTarget
		 *						that triggered the event.
		 */
		static void dropTargetDragLeave(ScriptOSDropTarget* thisPtr);

		/**
		 * @brief	Triggered when the drag and drop operation has finished over an OS drop target.
		 *
		 * @param	thisPtr		C++/CLR interop object that contains the native OSDropTarget
		 *						that triggered the event.
		 * @param	x			X coordinate of the pointer, relative to parent window, in pixels.
		 * @param	y			Y coordinate of the pointer, relative to parent window, in pixels.
		 */
		static void dropTargetDragDropped(ScriptOSDropTarget* thisPtr, INT32 x, INT32 y);

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

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		typedef void(__stdcall *OnEnterThunkDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnMoveDef) (MonoObject*, INT32, INT32, MonoException**);
		typedef void(__stdcall *OnLeaveDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDropThunkDef) (MonoObject*, INT32, INT32, MonoException**);

		static OnEnterThunkDef onEnterThunk;
		static OnMoveDef onMoveThunk;
		static OnLeaveDef onLeaveThunk;
		static OnDropThunkDef onDropThunk;

		static void internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* editorWindow);
		static void internal_Destroy(ScriptOSDropTarget* nativeInstance);
		static void internal_SetBounds(ScriptOSDropTarget* nativeInstance, Rect2I bounds);
		static MonoArray* internal_GetFilePaths(ScriptOSDropTarget* nativeInstance);
	};
}