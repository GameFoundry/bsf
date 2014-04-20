#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsEditorWidget.h"

namespace BansheeEngine
{
	class ScriptEditorWidget;

	class BS_SCR_BED_EXPORT ScriptEditorWindow : public BansheeEngine::ScriptObject<ScriptEditorWindow>
	{
		struct EditorWindowHandle
		{
			uint32_t gcHandle;
			ScriptEditorWindow* nativeObj;
		};
	public:
		~ScriptEditorWindow();

		EditorWidgetBase* getEditorWidget() const { return mEditorWidget; }

		static void initMetaData();

		static void registerManagedEditorWindows();
	private:
		friend class ScriptEditorWidget;

		ScriptEditorWindow(const String& windowName, const String& displayName, EditorWidgetBase* editorWidget);

		static MonoObject* internal_createOrGetInstance(MonoString* ns, MonoString* typeName);
		static void internal_destroyInstance(ScriptEditorWindow* thisPtr);

		static void initRuntimeData();

		static UINT32 internal_getWidth(ScriptEditorWindow* thisPtr);
		static UINT32 internal_getHeight(ScriptEditorWindow* thisPtr);

		static void internal_initializeGUIPanel(ScriptEditorWindow* thisPtr, MonoObject* panel);

		void onWidgetMoved(INT32 x, INT32 y);
		void onWidgetResized(UINT32 width, UINT32 height);
		void onWidgetParentChanged(EditorWidgetContainer* newParent);

		String mName;
		EditorWidgetBase* mEditorWidget;
		Vector<ScriptGUIPanel*>::type mPanels;
		boost::signals::connection mOnWidgetMovedConn;
		boost::signals::connection mOnWidgetResizedConn;
		boost::signals::connection mOnParentChangedConn;

		static MonoMethod* onResizedMethod;

		// Global editor window management methods
		static void registerScriptEditorWindow(ScriptEditorWindow* editorWindow);
		static void unregisterScriptEditorWindow(const String& windowTypeName);

		static UnorderedMap<String, EditorWindowHandle>::type OpenScriptEditorWindows;

		static EditorWidgetBase* openEditorWidgetCallback(String ns, String type, EditorWidgetContainer& parentContainer);
	};

	class BS_SCR_BED_EXPORT ScriptEditorWidget : public EditorWidgetBase
	{
	public:
		ScriptEditorWidget(const String& windowTypeName, const HString& displayName, EditorWidgetContainer& parentContainer);
		~ScriptEditorWidget();

	private:
		String mWindowTypeName;
	};
}