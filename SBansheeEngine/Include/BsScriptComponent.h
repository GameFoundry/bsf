#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptObject<ScriptComponent, ScriptGameObjectBase>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Component")

		virtual HGameObject getNativeHandle() const { return mManagedComponent; }
		virtual void setNativeHandle(const HGameObject& gameObject);

		void setManagedComponent(const GameObjectHandle<ManagedComponent>& managedComponent);

	private:
		friend class ScriptGameObjectManager;

		static void internal_createInstance(MonoObject* instance);
		static MonoObject* internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoArray* internal_getComponents(MonoObject* parentSceneObject);
		static void internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getSceneObject(ScriptComponent* nativeInstance);

		static bool checkIfDestroyed(const GameObjectHandleBase& handle);

		ScriptComponent(MonoObject* instance);
		~ScriptComponent() {}

		void _onManagedInstanceDeleted();

		GameObjectHandle<ManagedComponent> mManagedComponent;
	};
}