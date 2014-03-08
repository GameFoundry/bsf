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

		ScriptEditorWindow(const CM::String& windowName, const CM::String& displayName);

		static MonoObject* internal_createOrGetInstance(MonoString* ns, MonoString* typeName);
		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptEditorWindow* nativeInstance);

		static void initRuntimeData();

		CM::String mName;
		EditorWidgetBase* mEditorWidget;

		// Global editor window management methods
		static void registerScriptEditorWindow(ScriptEditorWindow* editorWindow);
		static void unregisterScriptEditorWindow(ScriptEditorWindow* editorWindow);

		static CM::UnorderedMap<CM::String, EditorWindowHandle>::type OpenScriptEditorWindows;

		static EditorWidgetBase* openEditorWidgetCallback(CM::String ns, CM::String type);
	};

	class BS_SCR_BED_EXPORT ScriptEditorWidget : public EditorWidgetBase
	{
	public:
		ScriptEditorWidget(const CM::HString& displayName, ScriptEditorWindow* window);
		~ScriptEditorWidget();

	private:
		ScriptEditorWindow* mWindow;
	};
}