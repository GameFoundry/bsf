#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public TScriptGUIElementBase<ScriptGUILayout>
	{
		struct ChildInfo
		{
			ScriptGUIElementBaseTBase* element;
			uint32_t gcHandle;
		};

	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUILayout")

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

		void addChild(ScriptGUIElementBaseTBase* element);
		void insertChild(UINT32 index, ScriptGUIElementBaseTBase* element);
		void removeChild(ScriptGUIElementBaseTBase* element);

		void destroy() override;

	private:
		friend class ScriptGUIPanel;

		static void internal_createInstanceX(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstanceY(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstancePanel(MonoObject* instance, INT16 depth, UINT16 depthRangeMin, UINT32 depthRangeMax, MonoArray* guiOptions);
		static void internal_addElement(ScriptGUILayout* instance, ScriptGUIElementBaseTBase* element);
		static void internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementBaseTBase* element);
		static UINT32 internal_getChildCount(ScriptGUILayout* instance);
		static MonoObject* internal_getChild(ScriptGUILayout* instance, UINT32 index);

		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);

		ScriptGUILayout(MonoObject* instance, GUILayout* layout);

		GUILayout* mLayout;
		Vector<ChildInfo> mChildren;

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