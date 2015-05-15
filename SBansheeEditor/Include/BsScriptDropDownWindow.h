#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsDropDownWindow.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class ManagedDropDownWindow;

	class BS_SCR_BED_EXPORT ScriptDropDownWindow : public ScriptObject <ScriptDropDownWindow>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "DropDownWindow")

		~ScriptDropDownWindow();

	private:
		friend class ManagedDropDownWindow;

		ScriptDropDownWindow(ManagedDropDownWindow* window);

		static void internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* parentWindow, Vector2I position, int width, int height);
		static void internal_Close(ScriptDropDownWindow* nativeInstance);
		static void internal_SetWidth(ScriptDropDownWindow* nativeInstance, UINT32 value);
		static void internal_SetHeight(ScriptDropDownWindow* nativeInstance, UINT32 value);
		static void internal_ScreenToWindowPos(ScriptDropDownWindow* nativeInstance, Vector2I position, Vector2I* windowPos);
		static void internal_WindowToScreenPos(ScriptDropDownWindow* nativeInstance, Vector2I position, Vector2I* screenPos);

		void onAssemblyRefreshStarted();
		void notifyWindowClosed();

		ManagedDropDownWindow* mDropDownWindow;
		HEvent mOnAssemblyRefreshStartedConn;

		static MonoField* guiPanelField;
	};

	class BS_SCR_BED_EXPORT ManagedDropDownWindow : public DropDownWindow
	{
	public:
		ManagedDropDownWindow(const RenderWindowPtr& parent, Viewport* target,
			const Vector2I& position, MonoObject* managedInstance, UINT32 width, UINT32 height);
		~ManagedDropDownWindow();

		void initialize(ScriptDropDownWindow* parent);
		void update() override;
		void reloadMonoTypes(MonoClass* windowClass);
		void triggerOnInitialize();
		void triggerOnDestroy();

		MonoObject* getManagedInstance() const { return mManagedInstance; }

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

		MonoObject* mManagedInstance;
		uint32_t mGCHandle;

		ScriptDropDownWindow* mScriptParent;
	};
}