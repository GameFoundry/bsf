#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsEditorWidget.h"

namespace BansheeEditor
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

		ScriptEditorWindow(const CM::String& windowName, const CM::String& displayName, EditorWidgetBase* editorWidget);

		static MonoObject* internal_createOrGetInstance(MonoString* ns, MonoString* typeName);
		static void internal_destroyInstance(ScriptEditorWindow* thisPtr);

		static void initRuntimeData();

		static CM::UINT32 internal_getWidth(ScriptEditorWindow* thisPtr);
		static CM::UINT32 internal_getHeight(ScriptEditorWindow* thisPtr);

		static void internal_initializeGUIPanel(ScriptEditorWindow* thisPtr, MonoObject* panel);

		void onWidgetMoved(CM::INT32 x, CM::INT32 y);
		void onWidgetResized(CM::UINT32 width, CM::UINT32 height);
		void onWidgetParentChanged(EditorWidgetContainer* newParent);

		CM::String mName;
		EditorWidgetBase* mEditorWidget;
		CM::Vector<BS::ScriptGUIPanel*>::type mPanels;
		boost::signals::connection mOnWidgetMovedConn;
		boost::signals::connection mOnWidgetResizedConn;
		boost::signals::connection mOnParentChangedConn;

		static BS::MonoMethod* onResizedMethod;

		// Global editor window management methods
		static void registerScriptEditorWindow(ScriptEditorWindow* editorWindow);
		static void unregisterScriptEditorWindow(const CM::String& windowTypeName);

		static CM::UnorderedMap<CM::String, EditorWindowHandle>::type OpenScriptEditorWindows;

		static EditorWidgetBase* openEditorWidgetCallback(CM::String ns, CM::String type, EditorWidgetContainer& parentContainer);
	};

	class BS_SCR_BED_EXPORT ScriptEditorWidget : public EditorWidgetBase
	{
	public:
		ScriptEditorWidget(const CM::String& windowTypeName, const CM::HString& displayName, EditorWidgetContainer& parentContainer);
		~ScriptEditorWidget();

	private:
		CM::String mWindowTypeName;
	};
}