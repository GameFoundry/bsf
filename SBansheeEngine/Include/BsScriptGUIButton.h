#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIButton : public ScriptObject<ScriptGUIButton>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIButton")

		GUIButton* getInternalValue() const { return mButton; }
		void* getNativeRaw() const { return mButton; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIButton* nativeInstance, MonoObject* content);

		static void internal_destroy(ScriptGUIButton* nativeInstance);
		static void internal_setVisible(ScriptGUIButton* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIButton* nativeInstance, MonoObject* parentLayout);

		static void onClick(MonoObject* instance);
		static void onHover(MonoObject* instance);
		static void onOut(MonoObject* instance);

		ScriptGUIButton(MonoObject* instance, GUIButton* button);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIButton* mButton;
		bool mIsDestroyed;

		typedef void (__stdcall *OnClickThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnHoverThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnOutThunkDef) (MonoObject*, MonoException**);

		static OnClickThunkDef onClickThunk;
		static OnHoverThunkDef onHoverThunk;
		static OnOutThunkDef onOutThunk;
	};
}