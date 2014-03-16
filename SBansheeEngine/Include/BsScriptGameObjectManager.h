#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectManager : public CM::Module<ScriptGameObjectManager>
	{
	public:
		// TODO - Add create methods

		/**
		 * @note Returns nullptr if script SceneObject doesn't exist.
		 */
		//ScriptSceneObject* getScriptSceneObject(CM::UINT64 id);

		/**
		 * @note Returns nullptr if script Component doesn't exist.
		 */
		//ScriptComponent* getScriptComponent(CM::UINT64 id);

		/**
		 * @note Returns nullptr if script GameObject doesn't exist.
		 */
		//ScriptGameObject* getScriptGameObject(CM::UINT64 id);

		//void destroyScriptGameObject(ScriptGameObject* gameObject);

	private:
		CM::UnorderedMap<CM::UINT64, ScriptGameObject*>::type mScriptResources;
	};
}