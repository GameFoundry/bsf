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
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "ModalWindow")

		~ScriptModalWindow();

	private:
		friend class ManagedModalWindow;

		ScriptModalWindow(ManagedModalWindow* editorWidget);

		static void internal_createInstance(MonoObject* instance, bool allowCloseButton);
		static void internal_close(ScriptModalWindow* thisPtr);
		static UINT32 internal_getWidth(ScriptModalWindow* thisPtr);
		static UINT32 internal_getHeight(ScriptModalWindow* thisPtr);
		static void internal_setWidth(ScriptModalWindow* thisPtr, UINT32 value);
		static void internal_setHeight(ScriptModalWindow* thisPtr, UINT32 value);
		static void internal_setTitle(ScriptModalWindow* thisPtr, MonoObject* title);
		static void internal_screenToWindowPos(ScriptModalWindow* thisPtr, Vector2I screenPos, Vector2I* windowPos);
		static void internal_windowToScreenPos(ScriptModalWindow* thisPtr, Vector2I windowPos, Vector2I* screenPos);

		void onAssemblyRefreshStarted();

		void _onManagedInstanceDeleted() override;
		ScriptObjectBackup beginRefresh() override;
		void endRefresh(const ScriptObjectBackup& backupData) override;
		MonoObject* _createManagedInstance(bool construct) override;
		void notifyWindowDestroyed();

		ManagedModalWindow* mModalWindow;
		HEvent mOnAssemblyRefreshStartedConn;
		bool mRefreshInProgress;

		static MonoField* guiPanelField;
	};

	class BS_SCR_BED_EXPORT ManagedModalWindow : public ModalWindow
	{
	public:
		ManagedModalWindow(bool allowCloseButton, MonoObject* managedInstance);
		~ManagedModalWindow();

		bool createManagedInstance();
		void releaseManagedInstance();

		void setParent(ScriptModalWindow* parent);
		void update() override;
		void reloadMonoTypes(MonoClass* windowClass);
		void triggerOnInitialize();
		void triggerOnDestroy();

		MonoObject* getManagedInstance() const { return mManagedInstance; }
	protected:
		virtual void resized() override;
		virtual void close() override;

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
		ScriptGUILayout* mContentsPanel;
	};
}