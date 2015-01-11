#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectManager : public Module<ScriptGameObjectManager>
	{
		struct ScriptGameObjectEntry
		{
			ScriptGameObjectEntry();
			ScriptGameObjectEntry(ScriptGameObjectBase* instance, bool isComponent);

			ScriptGameObjectBase* instance;
			bool isComponent;
		};

	public:
		ScriptGameObjectManager();
		~ScriptGameObjectManager();

		void registerScriptComponent(ScriptComponent* nativeInstance, const GameObjectHandle<ManagedComponent>& component);
		ScriptSceneObject* createScriptSceneObject(const HSceneObject& sceneObject);
		ScriptSceneObject* createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject);

		ScriptComponent* getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const;
		ScriptSceneObject* getScriptSceneObject(const HSceneObject& sceneObject) const;
		ScriptGameObjectBase* getScriptGameObject(UINT64 instanceId) const;

		void destroyScriptGameObject(ScriptGameObjectBase* gameObject);

	private:
		void sendComponentResetEvents();

		UnorderedMap<UINT64, ScriptGameObjectEntry> mScriptGameObjects;
		HEvent mOnAssemblyReloadDoneConn;
	};
}