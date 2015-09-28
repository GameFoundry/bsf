#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Manages all active GameObject interop objects. GameObjects can be created from native
	 *			code and used in managed code therefore we need to keep a dictionary or all the native
	 *			objects we have mapped to managed objects.
	 *
	 * @note	Thread safe.
	 */
	class BS_SCR_BE_EXPORT ScriptGameObjectManager : public Module<ScriptGameObjectManager>
	{
		/**
		 * @brief	Contains information about a single interop object containing a game object.
		 */
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

		/**
		 * @brief	Attempts to find the interop object for the specified SceneObject. If one cannot be found
		 *			new one is created and returned.
		 */
		ScriptSceneObject* getOrCreateScriptSceneObject(const HSceneObject& sceneObject);

		/**
		 * @brief	Creates a new interop object for the specified SceneObject. Throws an exception if one
		 *			already exists.
		 */
		ScriptSceneObject* createScriptSceneObject(const HSceneObject& sceneObject);

		/**
		 * @brief	Connects an existing managed SceneObject instance with the native SceneObject by creating
		 *			the interop object. Throws an exception if the interop object already exists.
		 */
		ScriptSceneObject* createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject);

		/**
		 * @brief	Connects an existing managed Component instance with the native Component by creating
		 *			the interop object. Throws an exception if the interop object already exists.
		 */
		ScriptComponent* createScriptComponent(MonoObject* existingInstance, const GameObjectHandle<ManagedComponent>& component);

		/**
		 * @brief	Attempts to find the interop object for the specified managed component. 
		 *			If one cannot be found null is returned.
		 */
		ScriptComponent* getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const;

		/**
		 * @brief	Attempts to find the interop object for a managed component with the specified instance ID. 
		 *			If one cannot be found null is returned.
		 */
		ScriptComponent* getScriptComponent(UINT64 instanceId) const;

		/**
		 * @brief	Attempts to find the interop object for the specified SceneObject. If one cannot be found
		 *			null is returned.
		 */
		ScriptSceneObject* getScriptSceneObject(const HSceneObject& sceneObject) const;

		/**
		 * @brief	Attempts to find the interop object for a GameObject with the specified instance ID. 
		 *			If one cannot be found null is returned.
		 */
		ScriptGameObjectBase* getScriptGameObject(UINT64 instanceId) const;

		/**
		 * @brief	Destroys and unregisters the specified GameObject interop object.
		 */
		void destroyScriptGameObject(ScriptGameObjectBase* gameObject);

	private:
		/**
		 * @brief	Triggers OnReset methods on all registered managed components.
		 *
		 * @note	Usually this happens after an assembly reload.
		 */
		void sendComponentResetEvents();

		UnorderedMap<UINT64, ScriptGameObjectEntry> mScriptGameObjects;
		HEvent mOnAssemblyReloadDoneConn;

		// Mutex needed as we need to be able unregister game objects as soon as they're finalized, and that happens on
		// a separate finalizer thread. 
		mutable Mutex mMutex;
	};
}