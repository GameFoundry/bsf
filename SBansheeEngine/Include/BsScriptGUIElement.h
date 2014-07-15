#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIOptions.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIElementBaseTBase : public ScriptObjectBase
	{
	public:
		ScriptGUIElementBaseTBase(MonoObject* instance);
		virtual ~ScriptGUIElementBaseTBase() {}

		GUIElementBase* getGUIElement() const { return (GUIElementBase*)mElement; }

		virtual void destroy() = 0;
		virtual void setParent(GUILayout* layout) = 0;
		virtual void setLayoutOptions(GUIOptions options) { }

	protected:
		void initialize(GUIElementBase* element);

		bool mIsDestroyed;
		GUIElementBase* mElement;
	};

	template <class Type>
	class TScriptGUIElementBase : public ScriptObject<Type, ScriptGUIElementBaseTBase>
	{
	public:
		virtual ~TScriptGUIElementBase() {}

	protected:
		TScriptGUIElementBase(MonoObject* instance, GUIElementBase* element)
			:ScriptObject(instance)
		{ 
			initialize(element);
		}

		void _onManagedInstanceDeleted()
		{
			// Elements with a GUIWidget parent are destroyed automatically when widget is destroyed, but those without one
			// we need to destroy manually.
			if (getGUIElement()->_getParentWidget() == nullptr) 
				destroy();

			ScriptObject::_onManagedInstanceDeleted();
		}
	};

	class BS_SCR_BE_EXPORT ScriptGUIElementTBase : public ScriptGUIElementBaseTBase
	{
	public:
		ScriptGUIElementTBase(MonoObject* instance);
		virtual ~ScriptGUIElementTBase() {}

		virtual void destroy();
		virtual void setParent(GUILayout* layout);
		virtual void setLayoutOptions(GUIOptions options);
	};

	template <class Type>
	class TScriptGUIElement : public ScriptObject<Type, ScriptGUIElementTBase>
	{
	public:
		virtual ~TScriptGUIElement() {}

	protected:
		TScriptGUIElement(MonoObject* instance, GUIElementBase* element)
			:ScriptObject(instance)
		{
			initialize(element);
		}

		void _onManagedInstanceDeleted()
		{
			// Elements with a GUIWidget parent are destroyed automatically when widget is destroyed, but those without one
			// we need to destroy manually.
			if (getGUIElement()->_getParentWidget() == nullptr)
				destroy();

			ScriptObject::_onManagedInstanceDeleted();
		}
	};

	class BS_SCR_BE_EXPORT ScriptGUIElement : public ScriptObject<ScriptGUIElement>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIElement")

	private:
		static void internal_destroy(ScriptGUIElementBaseTBase* nativeInstance);
		static void internal_setVisible(ScriptGUIElementBaseTBase* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIElementBaseTBase* nativeInstance, MonoObject* parentLayout);
		static void internal_setLayoutOptions(ScriptGUIElementBaseTBase* nativeInstance, MonoArray* guiOptions);

		ScriptGUIElement(MonoObject* instance);
	};
}