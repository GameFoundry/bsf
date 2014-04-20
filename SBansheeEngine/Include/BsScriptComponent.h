#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "CmFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptGameObject, public ScriptObject<ScriptComponent>
	{
	public:
		static void initMetaData();

		virtual CM::HGameObject getNativeHandle() const { return mManagedComponent; }
		virtual void setNativeHandle(const CM::HGameObject& gameObject);

	private:
		friend class ScriptGameObjectManager;

		static MonoObject* internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoObject* internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static MonoArray* internal_getComponents(MonoObject* parentSceneObject);
		static void internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type);
		static void internal_destroyInstance(ScriptComponent* nativeInstance);

		static void initRuntimeData();

		ScriptComponent(const CM::GameObjectHandle<ManagedComponent>& managedComponent);
		~ScriptComponent() {}

		CM::GameObjectHandle<ManagedComponent> mManagedComponent;
	};
}