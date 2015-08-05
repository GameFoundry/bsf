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

		if(iterFind != mObjects.end())
			return iterFind->second;
		
		return nullptr;
	}

	bool GameObjectManager::tryGetObject(UINT64 id, GameObjectHandleBase& object) const
	{
		auto iterFind = mObjects.find(id);

		if(iterFind != mObjects.end())
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

	GameObjectHandleBase GameObjectManager::registerObject(const std::shared_ptr<GameObject>& object)
	{
		object->initialize(object, mNextAvailableID);

		GameObjectHandleBase handle(object);
		mObjects[mNextAvailableID] = handle;
		mNextAvailableID++;

		return handle;
	}

	void GameObjectManager::unregisterObject(const GameObjectHandleBase& object)
	{
		mObjects.erase(object->getInstanceId());
	}

	void GameObjectManager::startDeserialization()
	{
		assert(!mIsDeserializationActive);

		mIsDeserializationActive = true;
	}

	void GameObjectManager::endDeserialization()
	{
		assert(mIsDeserializationActive);

		for(auto& unresolvedHandle : mUnresolvedHandles)
			resolveDeserializedHandle(unresolvedHandle, mGODeserializationMode);

		for(auto iter = mEndCallbacks.rbegin(); iter != mEndCallbacks.rend(); ++iter)
		{
			(*iter)();
		}

		mIsDeserializationActive = false;
		mActiveDeserializedObject = nullptr;
		mIdMapping.clear();
		mUnresolvedHandles.clear();
		mEndCallbacks.clear();
	}

	void GameObjectManager::resolveDeserializedHandle(GameObjectHandleBase& handle, UINT32 flags)
	{
		assert(mIsDeserializationActive);

		UINT64 instanceId = handle.getInstanceId();

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
				handle._resolve(findIterObj->second);
			else
			{
				if ((flags & GODM_KeepMissing) == 0)
					handle._resolve(nullptr);
			}
		}
		else
		{
			if ((flags & GODM_KeepMissing) == 0)
				handle._resolve(nullptr);
		}
	}

	void GameObjectManager::registerDeserializedId(UINT64 serializedId, UINT64 actualId)
	{
#if BS_DEBUG_MODE
		if(!mIsDeserializationActive)
		{
			BS_EXCEPT(InvalidStateException, "ID mapping may only be modified while deserialization is active.");
		}
#endif

		mIdMapping[serializedId] = actualId;
	}

	void GameObjectManager::registerUnresolvedHandle(const GameObjectHandleBase& object)
	{
#if BS_DEBUG_MODE
		if(!mIsDeserializationActive)
		{
			BS_EXCEPT(InvalidStateException, "Unresolved handle queue only be modified while deserialization is active.");
		}
#endif

		mUnresolvedHandles.push_back(object);
	}

	void GameObjectManager::registerOnDeserializationEndCallback(std::function<void()> callback)
	{
#if BS_DEBUG_MODE
		if(!mIsDeserializationActive)
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