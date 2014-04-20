#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectManager : public Module<ScriptGameObjectManager>
	{
	public:
		ScriptGameObjectManager();

		ScriptComponent* createScriptComponent(const GameObjectHandle<ManagedComponent>& component);
		ScriptSceneObject* createScriptSceneObject(const HSceneObject& sceneObject);
		ScriptSceneObject* createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject);

		ScriptComponent* getScriptComponent(const GameObjectHandle<ManagedComponent>& component);
		ScriptSceneObject* getScriptSceneObject(const HSceneObject& sceneObject);

		void destroyScriptGameObject(ScriptGameObject* gameObject);

	private:
		UnorderedMap<UINT64, ScriptGameObject*>::type mScriptGameObjects;

		MonoClass* mSceneObjectClass;
	};
}