#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptObject<ScriptComponent, ScriptGameObject>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Component")

		virtual HGameObject getNativeHandle() const { return mManagedComponent; }
		virtual void setNativeHandle(const HGameObject& gameObject);

	private:
		friend class ScriptGameObjectManager;

		static MonoObject* internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoArray* internal_getComponents(MonoObject* parentSceneObject);
		static void internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type);

		ScriptComponent(MonoObject* instance, const GameObjectHandle<ManagedComponent>& managedComponent);
		~ScriptComponent() {}

		void _onManagedInstanceDeleted();

		GameObjectHandle<ManagedComponent> mManagedComponent;
	};
}