#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public TScriptGUIElementBase<ScriptGUILayout>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUILayout")

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

	private:
		static void internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea);
		static void internal_createInstanceXFromLayoutAdd(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromLayoutAdd(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceXFromLayoutInsert(MonoObject* instance, MonoObject* parentLayout, UINT32 index);
		static void internal_createInstanceYFromLayoutInsert(MonoObject* instance, MonoObject* parentLayout, UINT32 index);
		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);
		static void internal_addElement(ScriptGUILayout* instance, ScriptGUIElementTBase* element);
		static void internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementTBase* element);

		ScriptGUILayout(MonoObject* instance, GUILayout* layout, GUILayout* parentLayout);

		void destroy();

		GUILayout* mLayout;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}