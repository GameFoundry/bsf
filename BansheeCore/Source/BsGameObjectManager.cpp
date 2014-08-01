#include "BsGameObjectManager.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	GameObjectManager::GameObjectManager()
		:mNextAvailableID(1), mIsDeserializationActive(false), mGODeserializationMode(GODM_UseNewIds | GODM_BreakExternal)
	{

	}

	GameObjectManager::~GameObjectManager()
	{ }

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
		{
			UINT64 instanceId = unresolvedHandle.getInstanceId();
			
			bool isInternalReference = false;
			
			auto findIter = mIdMapping.find(instanceId);
			if (findIter != mIdMapping.end())
			{
				if ((mGODeserializationMode & GODM_UseNewIds) != 0)
					instanceId = findIter->second;

				isInternalReference = true;
			}

			if (isInternalReference || (!isInternalReference && (mGODeserializationMode & GODM_RestoreExternal) != 0))
			{
				auto findIterObj = mObjects.find(instanceId);

				if (findIterObj != mObjects.end())
					unresolvedHandle._resolve(findIterObj->second);
				else
					unresolvedHandle._resolve(nullptr);
			}
			else
				unresolvedHandle._resolve(nullptr);
		}

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