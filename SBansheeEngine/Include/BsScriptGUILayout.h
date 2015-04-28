#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public TScriptGUIElementBase<ScriptGUILayout>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUILayout")

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

	private:
		static void internal_createInstanceX(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstanceY(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstancePanel(MonoObject* instance, MonoArray* guiOptions);
		static void internal_addElement(ScriptGUILayout* instance, ScriptGUIElementTBase* element);
		static void internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementTBase* element);

		static void internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea);
		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);

		ScriptGUILayout(MonoObject* instance, GUILayout* layout);

		void destroy();

		GUILayout* mLayout;
		bool mIsDestroyed;
	};
}