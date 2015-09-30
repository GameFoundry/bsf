#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Possible modes to use when deserializing games objects.
	 */
	enum GameObjectHandleDeserializationMode
	{
		/** All handles will point to old ID that were restored from the deserialized file. */
		GODM_UseOriginalIds = 0x01, 
		/** All handles will point to new IDs that were given to the deserialized GameObjects. */
		GODM_UseNewIds = 0x02,
		/** Handles pointing to GameObjects outside of the currently deserialized set
		will attempt to be restored in case those objects are still active. */
		GODM_RestoreExternal = 0x04,
		/** Handles pointing to GameObjects outside of the currently deserialized set
		will be broken. */
		GODM_BreakExternal = 0x08,
		/** Handles pointing to GameObjects that cannot be found will not be set to null. */
		GODM_KeepMissing = 0x10
	};

	/**
	 * @brief	Tracks GameObject creation and destructions. Also resolves
	 *			GameObject references from GameObject handles.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GameObjectManager : public Module<GameObjectManager>
	{
	public:
		GameObjectManager();
		~GameObjectManager();

		/**
		 * @brief	Registers a new GameObject and returns the handle to the object.
		 */
		GameObjectHandleBase registerObject(const std::shared_ptr<GameObject>& object);

		/**
		 * @brief	Unregisters a GameObject.
		 */
		void unregisterObject(const GameObjectHandleBase& object);

		/**
		 * @brief	Attempts to find a GameObject handle based on the GameObject instance ID.
		 *			Returns empty handle if ID cannot be found.
		 */
		GameObjectHandleBase getObject(UINT64 id) const;

		/**
		 * @brief	Attempts to find a GameObject handle based on the GameObject instance ID.
		 *			Returns true if object with the specified ID is found, false otherwise.
		 */
		bool tryGetObject(UINT64 id, GameObjectHandleBase& object) const;

		/**
		 * @brief	Checks if the GameObject with the specified instance ID exists.
		 */
		bool objectExists(UINT64 id) const;

		/**
		 * @brief	Changes the instance ID by which an object can be retrieved by. 
		 *
		 * @note	Caller is required to update the object itself with the new ID.
		 */
		void remapId(UINT64 oldId, UINT64 newId);

		/**
		 * @brief	Queues the object to be destroyed at the end of a GameObject update cycle.
		 */
		void queueForDestroy(const GameObjectHandleBase& object);

		/**
		 * @brief	Destroys any GameObjects that were queued for destruction.
		 */
		void destroyQueuedObjects();

		/**
		 * @brief	Triggered when a game object is being destroyed.
		 */
		Event<void(const HGameObject&)> onDestroyed;

		/************************************************************************/
		/* 							DESERIALIZATION                      		*/
		/************************************************************************/
		// Note: GameObjects need a bit of special handling when it comes to deserialization,
		// which is what this part of the code is used for. It performs two main actions:
		//  - 1. Resolves all GameObjectHandles on deserialization
		//    - We can't just resolve them as we go because during deserialization not all objects
		//      have necessarily been created.
		//  - 2. Maps serialized IDs to actual in-engine ids. 

		/**
		 * @brief	Needs to be called whenever GameObject deserialization starts. Must be followed
		 * 			by endDeserialization call.
		 */
		void startDeserialization();

		/**
		 * @brief	Needs to be called whenever GameObject deserialization ends. Must be preceded
		 * 			by startDeserialization call.
		 */
		void endDeserialization();

		/**
		 * @brief	Returns true if GameObject deserialization is currently in progress.
		 */
		bool isGameObjectDeserializationActive() const { return mIsDeserializationActive; }

		/**
		 * @brief	Registers an id that was deserialized, and has been remapped to
		 * 			an actual in-engine ID. This will be used when resolving GameObjectHandles
		 * 			(since they might point to the invalid deserialized id).
		 */
		void registerDeserializedId(UINT64 deserializedId, UINT64 actualId);

		/**
		 * @brief	Queues the specified handle and resolves it when deserialization ends.
		 */
		void registerUnresolvedHandle(const GameObjectHandleBase& object);

		/**
		 * @brief	Registers a callback that will be triggered when GameObject serialization ends.
		 */
		void registerOnDeserializationEndCallback(std::function<void()> callback);

		/**
		 * @brief	Changes the deserialization mode for any following GameObject handle.
		 *
		 * @param	gameObjectDeserializationMode		Mode that controls how are GameObjects handles resolved when being deserialized.
		 */
		void setDeserializationMode(UINT32 gameObjectDeserializationMode);

		/**
		 * @brief	Attempts to update the ID of the provided handle by mapping its old ID to
		 *			the newly deserialized object and its new ID. Game object deserialization
		 *			must be active.
		 */
		void resolveDeserializedHandle(GameObjectHandleBase& handle, UINT32 flags);

		/**
		 * @brief	Gets the currently active flags that control how are game object handles deserialized.
		 */
		UINT32 getDeserializationFlags() const { return mGODeserializationMode; }

	private:
		UINT64 mNextAvailableID; // 0 is not a valid ID
		Map<UINT64, GameObjectHandleBase> mObjects;
		Map<UINT64, GameObjectHandleBase> mQueuedForDestroy;

		GameObject* mActiveDeserializedObject;
		bool mIsDeserializationActive;
		Map<UINT64, UINT64> mIdMapping;
		Vector<GameObjectHandleBase> mUnresolvedHandles;
		Vector<std::function<void()>> mEndCallbacks;
		UINT32 mGODeserializationMode;
	};
}