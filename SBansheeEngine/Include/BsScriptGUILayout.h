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
		friend class ScriptGUIPanel;

		static void internal_createInstanceX(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstanceY(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstancePanel(MonoObject* instance, INT16 depth, UINT16 depthRangeMin, UINT32 depthRangeMax, MonoArray* guiOptions);
		static void internal_addElement(ScriptGUILayout* instance, ScriptGUIElementTBase* element);
		static void internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementTBase* element);

		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);

		ScriptGUILayout(MonoObject* instance, GUILayout* layout);

		void destroy();

		GUILayout* mLayout;
		bool mIsDestroyed;
	};

	class BS_SCR_BE_EXPORT ScriptGUIPanel : public ScriptObject<ScriptGUIPanel>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIPanel")

		static MonoObject* createFromExisting(GUIPanel* panel);

	private:
		ScriptGUIPanel(MonoObject* instance);
	};

}