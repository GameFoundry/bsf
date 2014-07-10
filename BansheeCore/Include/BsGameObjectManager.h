//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
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

	private:
		UINT64 mNextAvailableID; // 0 is not a valid ID
		Map<UINT64, GameObjectHandleBase> mObjects;

		GameObject* mActiveDeserializedObject;
		bool mIsDeserializationActive;
		Map<UINT64, UINT64> mIdMapping;
		Vector<GameObjectHandleBase> mUnresolvedHandles;
		Vector<std::function<void()>> mEndCallbacks;
	};
}