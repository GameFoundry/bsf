#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectManager : public CM::Module<ScriptGameObjectManager>
	{
	public:
		ScriptGameObjectManager();

		ScriptComponent* createScriptComponent(const CM::GameObjectHandle<ManagedComponent>& component);
		ScriptSceneObject* createScriptSceneObject(const CM::HSceneObject& sceneObject);
		ScriptSceneObject* createScriptSceneObject(MonoObject* existingInstance, const CM::HSceneObject& sceneObject);

		ScriptComponent* getScriptComponent(const CM::GameObjectHandle<ManagedComponent>& component);
		ScriptSceneObject* getScriptSceneObject(const CM::HSceneObject& sceneObject);

		void destroyScriptGameObject(ScriptGameObject* gameObject);

	private:
		CM::UnorderedMap<CM::UINT64, ScriptGameObject*>::type mScriptGameObjects;

		MonoClass* mSceneObjectClass;
	};
}