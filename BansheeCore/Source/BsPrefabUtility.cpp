#include "BsPrefabUtility.h"
#include "BsPrefabDiff.h"
#include "BsPrefab.h"
#include "BsSceneObject.h"
#include "BsResources.h"

namespace BansheeEngine
{
	void PrefabUtility::revertToPrefab(const HSceneObject& so)
	{
		String prefabLinkUUID = so->getPrefabLink();
		HPrefab prefabLink = static_resource_cast<Prefab>(gResources().loadFromUUID(prefabLinkUUID, false, false));

		if (prefabLink == nullptr)
			return;

		// Save IDs, destroy original, create new, restore IDs
		SceneObjectProxy soProxy;
		UnorderedMap<UINT32, GameObjectInstanceDataPtr> linkedInstanceData;
		recordInstanceData(so, soProxy, linkedInstanceData);

		HSceneObject parent = so->getParent();

		// This will destroy the object but keep it in the parent's child list
		HSceneObject currentSO = so;
		so->destroyInternal(currentSO, true);

		HSceneObject newInstance = prefabLink->instantiate();
		newInstance->mParent = parent;

		restoreLinkedInstanceData(newInstance, soProxy, linkedInstanceData);
	}

	void PrefabUtility::updateFromPrefab(const HSceneObject& so)
	{
		HSceneObject topLevelObject = so;

		while (topLevelObject != nullptr)
		{
			if (!topLevelObject->mPrefabLinkUUID.empty())
				break;

			if (topLevelObject->mParent != nullptr)
				topLevelObject = topLevelObject->mParent;
			else
				topLevelObject = nullptr;
		}

		Stack<HSceneObject> todo;
		todo.push(topLevelObject);

		// Find any prefab instances
		Vector<HSceneObject> prefabInstanceRoots;

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			if (!current->mPrefabLinkUUID.empty())
				prefabInstanceRoots.push_back(current);

			UINT32 childCount = current->getNumChildren();
			for (UINT32 i = 0; i < childCount; i++)
			{
				HSceneObject child = current->getChild(i);
				todo.push(child);
			}
		}

		// Stores data about the new prefab instance and its original parent and link id
		// (as those aren't stored in the prefab diff)
		struct RestoredPrefabInstance
		{
			HSceneObject newInstance;
			HSceneObject originalParent;
			UINT32 originalLinkId;
		};

		Vector<RestoredPrefabInstance> newPrefabInstances;

		// For each prefab instance load its reference prefab from the disk and check if it changed. If it has changed
		// instantiate the prefab and destroy the current instance. Then apply instance specific changes stored in a
		// prefab diff, if any, as well as restore the original parent and link id (link id of the root prefab instance
		// belongs to the parent prefab if any). Finally fix any handles pointing to the old objects so that they now point
		// to the newly instantiated objects. To the outside world it should be transparent that we just destroyed and then
		// re-created from scratch the entire hierarchy.

		// Need to do this bottom up to ensure I don't destroy the parents before children
		for (auto iter = prefabInstanceRoots.rbegin(); iter != prefabInstanceRoots.rend(); ++iter)
		{
			HSceneObject current = *iter;
			HPrefab prefabLink = static_resource_cast<Prefab>(gResources().loadFromUUID(current->mPrefabLinkUUID, false, false));

			if (prefabLink != nullptr /*&& prefabLink->getHash() != current->mPrefabHash*/)
			{
				// Save IDs, destroy original, create new, apply diff, restore IDs
				SceneObjectProxy soProxy;
				UnorderedMap<UINT32, GameObjectInstanceDataPtr> linkedInstanceData;
				recordInstanceData(current, soProxy, linkedInstanceData);

				HSceneObject parent = current->getParent();
				PrefabDiffPtr prefabDiff = current->mPrefabDiff;

				current->destroy(true);
				HSceneObject newInstance = prefabLink->instantiate(true);

				// When restoring instance IDs it is important to make all the new handles point to the old GameObjectInstanceData.
				// This is because old handles will have different GameObjectHandleData and we have no easy way of accessing it to
				// change to which GameObjectInstanceData it points. But the GameObjectManager ensures that all handles deserialized
				// at once (i.e. during the ::instantiate() call above) will share GameObjectHandleData so we can simply replace
				// to what they point to, affecting all of the handles to that object. (In another words, we can modify the
				// new handles at this point, but old ones must keep referencing what they already were.)
				restoreLinkedInstanceData(newInstance, soProxy, linkedInstanceData);
				restoreUnlinkedInstanceData(newInstance, soProxy);

				// Diff must be applied after the rename to ensure its game object handles point to valid objects
				if (prefabDiff != nullptr)
					prefabDiff->apply(newInstance);

				newInstance->mPrefabDiff = prefabDiff;

				newPrefabInstances.push_back({ newInstance, parent, newInstance->getLinkId() });
			}
		}

		// Once everything is instantiated, restore old parents & link IDs for root
		for (auto& newInstanceData : newPrefabInstances)
		{
			newInstanceData.newInstance->setParent(newInstanceData.originalParent, false);
			newInstanceData.newInstance->mLinkId = newInstanceData.originalLinkId;
		}

		gResources().unloadAllUnused();
	}

	UINT32 PrefabUtility::generatePrefabIds(const HSceneObject& sceneObject, UINT32 startingId)
	{
		Stack<HSceneObject> todo;
		todo.push(sceneObject);

		while (!todo.empty())
		{
			HSceneObject currentSO = todo.top();
			todo.pop();

			for (auto& component : currentSO->mComponents)
			{
				if (component->getLinkId() == (UINT32)-1)
					component->mLinkId = startingId++;
			}

			UINT32 numChildren = (UINT32)currentSO->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = currentSO->getChild(i);

				if (!child->hasFlag(SOF_DontSave))
				{
					if (child->getLinkId() == (UINT32)-1)
						child->mLinkId = startingId++;

					if(child->mPrefabLinkUUID.empty())
						todo.push(currentSO->getChild(i));
				}
			}
		}

		return startingId;
	}

	void PrefabUtility::clearPrefabIds(const HSceneObject& sceneObject, bool recursive)
	{
		Stack<HSceneObject> todo;
		todo.push(sceneObject);

		while (!todo.empty())
		{
			HSceneObject currentSO = todo.top();
			todo.pop();

			for (auto& component : currentSO->mComponents)
				component->mLinkId = (UINT32)-1;

			if (recursive)
			{
				UINT32 numChildren = (UINT32)currentSO->getNumChildren();
				for (UINT32 i = 0; i < numChildren; i++)
				{
					HSceneObject child = currentSO->getChild(i);
					child->mLinkId = (UINT32)-1;

					if (child->mPrefabLinkUUID.empty())
						todo.push(child);
				}
			}
		}
	}

	void PrefabUtility::recordPrefabDiff(const HSceneObject& sceneObject)
	{
		HSceneObject topLevelObject = sceneObject;

		while (topLevelObject != nullptr)
		{
			if (!topLevelObject->mPrefabLinkUUID.empty())
				break;

			if (topLevelObject->mParent != nullptr)
				topLevelObject = topLevelObject->mParent;
			else
				topLevelObject = nullptr;
		}

		if (topLevelObject == nullptr)
			topLevelObject = sceneObject;

		Stack<HSceneObject> todo;
		todo.push(topLevelObject);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			if (!current->mPrefabLinkUUID.empty())
			{
				current->mPrefabDiff = nullptr;

				HPrefab prefabLink = static_resource_cast<Prefab>(gResources().loadFromUUID(current->mPrefabLinkUUID, false, false));
				if (prefabLink != nullptr)
					current->mPrefabDiff = PrefabDiff::create(prefabLink->_getRoot(), current->getHandle());
			}

			UINT32 childCount = current->getNumChildren();
			for (UINT32 i = 0; i < childCount; i++)
			{
				HSceneObject child = current->getChild(i);
				todo.push(child);
			}
		}

		gResources().unloadAllUnused();
	}

	void PrefabUtility::recordInstanceData(const HSceneObject& so, SceneObjectProxy& output,
		UnorderedMap<UINT32, GameObjectInstanceDataPtr>& linkedInstanceData)
	{
		struct StackData
		{
			HSceneObject so;
			SceneObjectProxy* proxy;
		};

		Stack<StackData> todo;
		todo.push({so, &output});

		output.instanceData = so->_getInstanceData();
		output.linkId = (UINT32)-1;

		while (!todo.empty())
		{
			StackData curData = todo.top();
			todo.pop();

			const Vector<HComponent>& components = curData.so->getComponents();
			for (auto& component : components)
			{
				curData.proxy->components.push_back(ComponentProxy());

				ComponentProxy& componentProxy = curData.proxy->components.back();
				componentProxy.instanceData = component->_getInstanceData();
				componentProxy.linkId = component->getLinkId();

				linkedInstanceData[componentProxy.linkId] = componentProxy.instanceData;
			}

			UINT32 numChildren = curData.so->getNumChildren();
			curData.proxy->children.resize(numChildren);

			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = curData.so->getChild(i);

				SceneObjectProxy& childProxy = curData.proxy->children[i];

				childProxy.instanceData = child->_getInstanceData();
				childProxy.linkId = child->getLinkId();

				linkedInstanceData[childProxy.linkId] = childProxy.instanceData;

				if (child->mPrefabLinkUUID.empty())
				{
					todo.push({ child, &curData.proxy->children[i] });
				}
			}
		}
	}

	void PrefabUtility::restoreLinkedInstanceData(const HSceneObject& so, SceneObjectProxy& proxy, 
		UnorderedMap<UINT32, GameObjectInstanceDataPtr>& linkedInstanceData)
	{
		Stack<HSceneObject> todo;
		todo.push(so);

		// Root is not in the instance data map because its link ID belongs to the parent prefab, if any
		so->_setInstanceData(proxy.instanceData);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			Vector<HComponent>& components = current->mComponents;
			for (auto& component : components)
			{
				if (component->getLinkId() != (UINT32)-1)
				{
					auto iterFind = linkedInstanceData.find(component->getLinkId());
					if (iterFind != linkedInstanceData.end())
					{
						component->_setInstanceData(iterFind->second);
						component._setHandleData(component.getInternalPtr());
					}
				}
			}

			UINT32 numChildren = current->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = current->getChild(i);

				if (child->getLinkId() != (UINT32)-1)
				{
					auto iterFind = linkedInstanceData.find(child->getLinkId());
					if (iterFind != linkedInstanceData.end())
						child->_setInstanceData(iterFind->second);
				}

				if (child->mPrefabLinkUUID.empty())
					todo.push(child);
			}
		}
	}

	void PrefabUtility::restoreUnlinkedInstanceData(const HSceneObject& so, SceneObjectProxy& proxy)
	{
		struct StackEntry
		{
			HSceneObject so;
			SceneObjectProxy* proxy;
		};

		Stack<StackEntry> todo;
		todo.push(StackEntry());

		StackEntry& topEntry = todo.top();
		topEntry.so = so;
		topEntry.proxy = &proxy;

		while (!todo.empty())
		{
			StackEntry current = todo.top();
			todo.pop();

			if (current.proxy->linkId == -1)
				current.so->_setInstanceData(current.proxy->instanceData);

			Vector<HComponent>& components = current.so->mComponents;
			UINT32 componentProxyIdx = 0;
			UINT32 numComponentProxies = (UINT32)current.proxy->components.size();
			for (auto& component : components)
			{
				if (component->getLinkId() == (UINT32)-1)
				{
					bool foundInstanceData = false;
					for (; componentProxyIdx < numComponentProxies; componentProxyIdx++)
					{
						if (current.proxy->components[componentProxyIdx].linkId != -1)
							continue;

						component->_setInstanceData(current.proxy->components[componentProxyIdx].instanceData);
						component._setHandleData(component.getInternalPtr());

						foundInstanceData = true;
						break;
					}

					assert(foundInstanceData);
				}
			}

			UINT32 numChildren = current.so->getNumChildren();
			UINT32 childProxyIdx = 0;
			UINT32 numChildProxies = (UINT32)current.proxy->children.size();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = current.so->getChild(i);

				if (child->getLinkId() == (UINT32)-1)
				{
					bool foundInstanceData = false;
					for (; childProxyIdx < numChildProxies; childProxyIdx++)
					{
						if (current.proxy->children[childProxyIdx].linkId != -1)
							continue;

						assert(current.proxy->children[childProxyIdx].linkId == -1);
						child->_setInstanceData(current.proxy->children[childProxyIdx].instanceData);

						if (child->mPrefabLinkUUID.empty())
						{
							todo.push(StackEntry());

							StackEntry& newEntry = todo.top();
							newEntry.so = child;
							newEntry.proxy = &current.proxy->children[childProxyIdx];
						}

						foundInstanceData = true;
						break;
					}

					assert(foundInstanceData);
				}
				else
				{
					if (!child->mPrefabLinkUUID.empty())
						continue;

					for (UINT32 j = 0; j < numChildProxies; j++)
					{
						if (child->getLinkId() == current.proxy->children[j].linkId)
						{
							todo.push(StackEntry());

							StackEntry& newEntry = todo.top();
							newEntry.so = child;
							newEntry.proxy = &current.proxy->children[j];
							break;
						}
					}
				}
			}
		}
	}
}