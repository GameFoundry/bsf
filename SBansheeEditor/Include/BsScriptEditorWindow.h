#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsEditorWidget.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class ScriptEditorWidget;

	class BS_SCR_BED_EXPORT ScriptEditorWindow : public ScriptObject<ScriptEditorWindow, PersistentScriptObjectBase>
	{
		struct EditorWindowHandle
		{
			uint32_t gcHandle;
			ScriptEditorWindow* nativeObj;
		};
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorWindow")

		~ScriptEditorWindow();

		EditorWidgetBase* getEditorWidget() const;
		bool isDestroyed() const { return mIsDestroyed; }

		static void registerManagedEditorWindows();
		static void clearRegisteredEditorWindow();

	private:
		friend class ScriptEditorWidget;

		ScriptEditorWindow(ScriptEditorWidget* editorWidget);

		static MonoObject* internal_createOrGetInstance(MonoString* ns, MonoString* typeName);
		static MonoObject* internal_getInstance(MonoString* ns, MonoString* typeName);

		static bool internal_hasFocus(ScriptEditorWindow* thisPtr);
		static void internal_screenToWindowPos(ScriptEditorWindow* thisPtr, Vector2I screenPos, Vector2I* windowPos);
		static void internal_windowToScreenPos(ScriptEditorWindow* thisPtr, Vector2I windowPos, Vector2I* screenPos);
		static UINT32 internal_getWidth(ScriptEditorWindow* thisPtr);
		static UINT32 internal_getHeight(ScriptEditorWindow* thisPtr);

		void onWidgetResized(UINT32 width, UINT32 height);
		void onFocusChanged(bool inFocus);
		void onAssemblyRefreshStarted();

		void _onManagedInstanceDeleted();
		ScriptObjectBackup beginRefresh();
		void endRefresh(const ScriptObjectBackup& backupData);
		MonoObject* _createManagedInstance(bool construct);

		String mName;
		ScriptEditorWidget* mEditorWidget;
		HEvent mOnWidgetResizedConn;
		HEvent mOnFocusChangedConn;
		HEvent mOnAssemblyRefreshStartedConn;
		bool mRefreshInProgress;
		bool mIsDestroyed;

		static MonoMethod* onResizedMethod;
		static MonoMethod* onFocusChangedMethod;
		static MonoField* guiPanelField;

		// Global editor window management methods
		static void registerScriptEditorWindow(ScriptEditorWindow* editorWindow);
		static void unregisterScriptEditorWindow(const String& windowTypeName);

		static UnorderedMap<String, EditorWindowHandle> OpenScriptEditorWindows;
		static Vector<String> AvailableWindowTypes;

		static EditorWidgetBase* openEditorWidgetCallback(String ns, String type, EditorWidgetContainer& parentContainer);
	};

	class BS_SCR_BED_EXPORT ScriptEditorWidget : public EditorWidgetBase
	{
	public:
		ScriptEditorWidget(const String& ns, const String& type, EditorWidgetContainer& parentContainer);
		~ScriptEditorWidget();

		bool createManagedInstance();
		void update();
		void reloadMonoTypes(MonoClass* windowClass);
		void triggerOnInitialize();
		void triggerOnDestroy();

		void setScriptOwner(ScriptEditorWindow* owner) { mScriptOwner = owner; }
		MonoObject* getManagedInstance() const { return mManagedInstance; }

	private:
		typedef void(__stdcall *OnInitializeThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDestroyThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *UpdateThunkDef) (MonoObject*, MonoException**);

		String mNamespace;
		String mTypename;

		OnInitializeThunkDef mOnInitializeThunk;
		OnDestroyThunkDef mOnDestroyThunk;
		UpdateThunkDef mUpdateThunk;
		MonoObject* mManagedInstance;
		MonoMethod* mGetDisplayName;

		ScriptEditorWindow* mScriptOwner;
		ScriptGUILayout* mContentsPanel;
	};
}