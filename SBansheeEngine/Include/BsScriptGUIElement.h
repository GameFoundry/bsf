#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIElementBase : public ScriptObjectBase
	{
	public:
		ScriptGUIElementBase(MonoObject* instance);
		virtual ~ScriptGUIElementBase() {}

		GUIElement* getGUIElement() const { return (GUIElement*)mElement; }

		virtual void destroy();

	protected:
		void initialize(GUIElement* element);

	private:
		bool mIsDestroyed;
		GUIElement* mElement;
	};

	template <class Type>
	class TScriptGUIElement : public ScriptObject<Type, ScriptGUIElementBase>
	{
	public:
		virtual ~TScriptGUIElement() {}

	protected:
		TScriptGUIElement(MonoObject* instance, GUIElement* element)
			:ScriptObject(instance)
		{ 
			initialize(element);
		}

		void _onManagedInstanceDeleted()
		{
			destroy();

			ScriptObject::_onManagedInstanceDeleted();
		}
	};

	class BS_SCR_BE_EXPORT ScriptGUIElement : public ScriptObject<ScriptGUIElement>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIElement")

	private:
		static void internal_destroy(ScriptGUIElementBase* nativeInstance);
		static void internal_setVisible(ScriptGUIElementBase* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIElementBase* nativeInstance, MonoObject* parentLayout);

		ScriptGUIElement(MonoObject* instance);
	};
}