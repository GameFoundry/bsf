#include "BsGameObjectManager.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	GameObjectManager::GameObjectManager()
		:mNextAvailableID(1), mIsDeserializationActive(false), mGODeserializationMode(GODM_UseNewIds | GODM_BreakExternal)
	{

	}

	GameObjectManager::~GameObjectManager()
	{
		destroyQueuedObjects();
	}

	GameObjectHandleBase GameObjectManager::getObject(UINT64 id) const
	{
		auto iterFind = mObjects.find(id);

		if (iterFind != mObjects.end())
			return iterFind->second;

		return nullptr;
	}

	bool GameObjectManager::tryGetObject(UINT64 id, GameObjectHandleBase& object) const
	{
		auto iterFind = mObjects.find(id);

		if (iterFind != mObjects.end())
		{
			object = iterFind->second;
			return true;
		}

		return false;
	}

	bool GameObjectManager::objectExists(UINT64 id) const
	{
		return mObjects.find(id) != mObjects.end();
	}

	void GameObjectManager::remapId(UINT64 oldId, UINT64 newId)
	{
		if (oldId == newId)
			return;

		mObjects[newId] = mObjects[oldId];
		mObjects.erase(oldId);
	}

	void GameObjectManager::queueForDestroy(const GameObjectHandleBase& object)
	{
		if (object.isDestroyed())
			return;

		UINT64 instanceId = object->getInstanceId();
		mQueuedForDestroy[instanceId] = object;
	}

	void GameObjectManager::destroyQueuedObjects()
	{
		for (auto& objPair : mQueuedForDestroy)
			objPair.second->destroyInternal(objPair.second, true);

		mQueuedForDestroy.clear();
	}

	GameObjectHandleBase GameObjectManager::registerObject(const std::shared_ptr<GameObject>& object, UINT64 originalId)
	{
		object->initialize(object, mNextAvailableID);

		if (mIsDeserializationActive)
		{
			assert(originalId != 0 && "You must provide an original ID when registering a deserialized game object.");

			auto iterFind = mUnresolvedHandleData.find(originalId);
			if (iterFind != mUnresolvedHandleData.end())
			{
				GameObjectHandleBase handle;
				handle.mData = iterFind->second;
				handle._setHandleData(object);

				mObjects[mNextAvailableID] = handle;
				mIdMapping[originalId] = mNextAvailableID;
				mNextAvailableID++;

				return handle;
			}
			else
			{
				GameObjectHandleBase handle(object);

				mObjects[mNextAvailableID] = handle;
				mIdMapping[originalId] = mNextAvailableID;
				mNextAvailableID++;

				return handle;
			}
		}

		GameObjectHandleBase handle(object);
		mObjects[mNextAvailableID] = handle;
		mNextAvailableID++;

		return handle;
	}

	void GameObjectManager::unregisterObject(GameObjectHandleBase& object)
	{
		mObjects.erase(object->getInstanceId());

		onDestroyed(object);
		object.destroy();
	}

	void GameObjectManager::startDeserialization()
	{
		assert(!mIsDeserializationActive);

		mIsDeserializationActive = true;
	}

	void GameObjectManager::endDeserialization()
	{
		assert(mIsDeserializationActive);

		for (auto& unresolvedHandle : mUnresolvedHandles)
			resolveDeserializedHandle(unresolvedHandle, mGODeserializationMode);

		for (auto iter = mEndCallbacks.rbegin(); iter != mEndCallbacks.rend(); ++iter)
		{
			(*iter)();
		}

		mIsDeserializationActive = false;
		mActiveDeserializedObject = nullptr;
		mIdMapping.clear();
		mUnresolvedHandles.clear();
		mEndCallbacks.clear();
		mUnresolvedHandleData.clear();
	}

	void GameObjectManager::resolveDeserializedHandle(UnresolvedHandle& data, UINT32 flags)
	{
		assert(mIsDeserializationActive);

		UINT64 instanceId = data.originalInstanceId;

		bool isInternalReference = false;

		auto findIter = mIdMapping.find(instanceId);
		if (findIter != mIdMapping.end())
		{
			if ((flags & GODM_UseNewIds) != 0)
				instanceId = findIter->second;

			isInternalReference = true;
		}

		if (isInternalReference || (!isInternalReference && (flags & GODM_RestoreExternal) != 0))
		{
			auto findIterObj = mObjects.find(instanceId);

			if (findIterObj != mObjects.end())
				data.handle._resolve(findIterObj->second);
			else
			{
				if ((flags & GODM_KeepMissing) == 0)
					data.handle._resolve(nullptr);
			}
		}
		else
		{
			if ((flags & GODM_KeepMissing) == 0)
				data.handle._resolve(nullptr);
		}
	}

	void GameObjectManager::registerUnresolvedHandle(UINT64 originalId, GameObjectHandleBase& object)
	{
#if BS_DEBUG_MODE
		if (!mIsDeserializationActive)
		{
			BS_EXCEPT(InvalidStateException, "Unresolved handle queue only be modified while deserialization is active.");
		}
#endif

		// Update the provided handle to ensure all handles pointing to the same object share the same handle data
		bool foundHandleData = false;

		// Search object that are currently being deserialized
		auto iterFind = mIdMapping.find(originalId);
		if (iterFind != mIdMapping.end())
		{
			auto iterFind2 = mObjects.find(iterFind->second);
			if (iterFind2 != mObjects.end())
			{
				object.mData = iterFind2->second.mData;
				foundHandleData = true;
			}
		}

		// Search previously deserialized handles
		if (!foundHandleData)
		{
			auto iterFind = mUnresolvedHandleData.find(originalId);
			if (iterFind != mUnresolvedHandleData.end())
			{
				object.mData = iterFind->second;
				foundHandleData = true;
			}
		}

		// If still not found, this is the first such handle so register its handle data
		mUnresolvedHandleData[originalId] = object.mData;
		mUnresolvedHandles.push_back({ originalId, object });
	}

	void GameObjectManager::registerOnDeserializationEndCallback(std::function<void()> callback)
	{
#if BS_DEBUG_MODE
		if (!mIsDeserializationActive)
		{
			BS_EXCEPT(InvalidStateException, "Callback queue only be modified while deserialization is active.");
		}
#endif

		mEndCallbacks.push_back(callback);
	}

	void GameObjectManager::setDeserializationMode(UINT32 gameObjectDeserializationMode)
	{
#if BS_DEBUG_MODE
		if (mIsDeserializationActive)
		{
			BS_EXCEPT(InvalidStateException, "Deserialization modes can not be modified when deserialization is not active.");
		}
#endif

		mGODeserializationMode = gameObjectDeserializationMode;
	}
}