#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIFoldout : public ScriptObject<ScriptGUIFoldout>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIFoldout")

		GUIFoldout* getInternalValue() const { return mFoldout; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIFoldout* nativeInstance, MonoObject* content);

		static void internal_destroy(ScriptGUIFoldout* nativeInstance);
		static void internal_setVisible(ScriptGUIFoldout* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIFoldout* nativeInstance, MonoObject* parentLayout);

		static void onToggled(MonoObject* instance, bool expanded);

		ScriptGUIFoldout(MonoObject* instance, GUIFoldout* foldout);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIFoldout* mFoldout;
		bool mIsDestroyed;

		typedef void (__stdcall *OnToggledThunkDef) (MonoObject*, bool, MonoException**);

		static OnToggledThunkDef onToggledThunk;
	};
}