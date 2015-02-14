#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsModalWindow.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class ManagedModalWindow;

	class BS_SCR_BED_EXPORT ScriptModalWindow : public ScriptObject <ScriptModalWindow, PersistentScriptObjectBase>
	{
		struct ModalWindowHandle
		{
			uint32_t gcHandle;
			ManagedModalWindow* nativeObj;
		};
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "ModalWindow")

		~ScriptModalWindow();

	private:
		friend class ManagedModalWindow;

		ScriptModalWindow(ManagedModalWindow* editorWidget);

		static MonoObject* internal_createInstance(MonoString* ns, MonoString* typeName);
		static void internal_close(ScriptModalWindow* thisPtr);
		static UINT32 internal_getWidth(ScriptModalWindow* thisPtr);
		static UINT32 internal_getHeight(ScriptModalWindow* thisPtr);
		static void internal_initializeGUIPanel(ScriptModalWindow* thisPtr, MonoObject* panel);

		void onAssemblyRefreshStarted();

		void _onManagedInstanceDeleted();
		ScriptObjectBackup beginRefresh();
		void endRefresh(const ScriptObjectBackup& backupData);
		MonoObject* _createManagedInstance(bool construct);

		ManagedModalWindow* mModalWindow;
		ScriptGUIPanel* mGUIPanel;
		HEvent mOnAssemblyRefreshStartedConn;
		bool mRefreshInProgress;

		static MonoMethod* onInitializedInternalMethod;
		static MonoMethod* onDestroyInternalMethod;
	};

	class BS_SCR_BED_EXPORT ManagedModalWindow : public ModalWindow
	{
	public:
		ManagedModalWindow(const String& ns, const String& type);
		~ManagedModalWindow();

		bool createManagedInstance();
		void releaseManagedInstance();

		void update();
		void reloadMonoTypes(MonoClass* windowClass);
		void triggerOnInitialize();
		void triggerOnDestroy();

		MonoObject* getManagedInstance() const { return mManagedInstance; }
	protected:
		virtual void resized();

	private:
		friend class ScriptModalWindow;

		typedef void(__stdcall *OnInitializeThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *OnDestroyThunkDef) (MonoObject*, MonoException**);
		typedef void(__stdcall *UpdateThunkDef) (MonoObject*, MonoException**);

		String mNamespace;
		String mTypename;

		OnInitializeThunkDef mOnInitializeThunk;
		OnDestroyThunkDef mOnDestroyThunk;
		UpdateThunkDef mUpdateThunk;
		MonoMethod* mOnWindowResizedMethod;

		MonoObject* mManagedInstance;
		uint32_t mGCHandle;

		ScriptModalWindow* mScriptParent;
	};
}