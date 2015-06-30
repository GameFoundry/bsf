#include "BsPrefabUtility.h"
#include "BsPrefabDiff.h"
#include "BsPrefab.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	void PrefabUtility::revertToPrefab(const HSceneObject& so)
	{
		HPrefab prefabLink = so->getPrefabLink();
		if (prefabLink == nullptr)
			return;

		// Save IDs, destroy original, create new, restore IDs
		SceneObjectProxy soProxy;
		UnorderedMap<UINT32, GameObjectInstanceDataPtr> linkedInstanceData;
		recordInstanceData(so, soProxy, linkedInstanceData);

		so->destroy();

		HSceneObject newInstance = prefabLink->instantiate();
		restoreLinkedInstanceData(newInstance, linkedInstanceData);
	}

	void PrefabUtility::updateFromPrefab(const HSceneObject& so)
	{
		HPrefab prefabLink = so->getPrefabLink();
		if (prefabLink == nullptr)
			return;

		// Save IDs, destroy original, create new, apply diff, restore IDs
		SceneObjectProxy soProxy;
		UnorderedMap<UINT32, GameObjectInstanceDataPtr> linkedInstanceData;
		recordInstanceData(so, soProxy, linkedInstanceData);

		PrefabDiffPtr prefabDiff = so->mPrefabDiff;
		so->destroy();

		HSceneObject newInstance = prefabLink->instantiate();
		if (prefabDiff != nullptr)
			prefabDiff->apply(newInstance);

		restoreLinkedInstanceData(newInstance, linkedInstanceData);
		restoreUnlinkedInstanceData(newInstance, soProxy);
	}

	void PrefabUtility::generatePrefabIds(const HSceneObject& sceneObject)
	{
		Vector<HGameObject> objectsToId;
		Set<INT32> existingIds;

		Stack<HSceneObject> todo;
		todo.push(sceneObject);

		while (!todo.empty())
		{
			HSceneObject currentSO = todo.top();
			todo.pop();

			if (currentSO->mLinkId == -1)
				objectsToId.push_back(currentSO);
			else
				existingIds.insert(currentSO->mLinkId);

			for (auto& component : currentSO->mComponents)
			{
				if (component->mLinkId == -1)
					objectsToId.push_back(component);
				else
					existingIds.insert(component->mLinkId);
			}

			UINT32 numChildren = (UINT32)currentSO->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = currentSO->getChild(i);

				if (child->mPrefabLink == nullptr)
					todo.push(currentSO->getChild(i));
			}
		}

		auto setIter = existingIds.begin();
		INT32 nextId = 0;
		for (auto& object : objectsToId)
		{
			INT32 freeId = -1;
			for (; setIter != existingIds.end(); ++setIter)
			{
				if (nextId < (*setIter))
					freeId = nextId++;
				else
					nextId++;
			}

			if (freeId == -1)
				freeId = nextId++;

			object->mLinkId = freeId;
		}
	}

	void PrefabUtility::clearPrefabIds(const HSceneObject& sceneObject)
	{
		Stack<HSceneObject> todo;
		todo.push(sceneObject);

		while (!todo.empty())
		{
			HSceneObject currentSO = todo.top();
			todo.pop();

			currentSO->mLinkId = -1;
			for (auto& component : currentSO->mComponents)
				component->mLinkId = -1;

			UINT32 numChildren = (UINT32)currentSO->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = currentSO->getChild(i);

				if (child->mPrefabLink == nullptr)
					todo.push(child);
			}
		}
	}

	void PrefabUtility::recordInstanceData(const HSceneObject& so, SceneObjectProxy& output,
		UnorderedMap<UINT32, GameObjectInstanceDataPtr>& linkedInstanceData)
	{
		struct StackEntry
		{
			HSceneObject so;
			bool isPartOfPrefab;
		};

		Stack<StackEntry> todo;
		todo.push(StackEntry());

		StackEntry& topEntry = todo.top();
		topEntry.so = so;
		topEntry.isPartOfPrefab = true;

		while (!todo.empty())
		{
			StackEntry current = todo.top();
			todo.pop();

			output.instanceData = current.so->_getInstanceData();

			if (current.isPartOfPrefab)
			{
				output.linkId = current.so->getLinkId();
				linkedInstanceData[output.linkId] = output.instanceData;
			}
			else
				output.linkId = -1;

			const Vector<HComponent>& components = current.so->getComponents();
			for (auto& component : components)
			{
				output.components.push_back(ComponentProxy());

				ComponentProxy& componentProxy = output.components.back();
				componentProxy.instanceData = component->_getInstanceData();

				if (current.isPartOfPrefab)
				{
					componentProxy.linkId = component->getLinkId();
					linkedInstanceData[componentProxy.linkId] = componentProxy.instanceData;
				}
				else
					componentProxy.linkId = -1;
			}

			UINT32 numChildren = current.so->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = current.so->getChild(i);

				todo.push(StackEntry());
				StackEntry& newEntry = todo.top();

				newEntry.so = child;
				newEntry.isPartOfPrefab = current.isPartOfPrefab && (child->mPrefabLink == nullptr);
			}
		}
	}

	void PrefabUtility::restoreLinkedInstanceData(const HSceneObject& so, UnorderedMap<UINT32, GameObjectInstanceDataPtr>& linkedInstanceData)
	{
		Stack<HSceneObject> todo;
		todo.push(so);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			if (current->getLinkId() != -1)
			{
				auto iterFind = linkedInstanceData.find(current->getLinkId());
				if (iterFind != linkedInstanceData.end())
					current->_setInstanceData(iterFind->second);
			}

			const Vector<HComponent>& components = current->getComponents();
			for (auto& component : components)
			{
				if (component->getLinkId() != -1)
				{
					auto iterFind = linkedInstanceData.find(component->getLinkId());
					if (iterFind != linkedInstanceData.end())
						component->_setInstanceData(iterFind->second);
				}
			}

			UINT32 numChildren = current->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = current->getChild(i);

				if (child->mPrefabLink == nullptr)
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

		assert(so->getLinkId() == proxy.linkId);
		
		StackEntry& topEntry = todo.top();
		topEntry.so = so;
		topEntry.proxy = &proxy;

		while (!todo.empty())
		{
			StackEntry current = todo.top();
			todo.pop();

			if (current.proxy->linkId == -1)
				current.so->_setInstanceData(current.proxy->instanceData);

			const Vector<HComponent>& components = current.so->getComponents();
			UINT32 componentProxyIdx = 0;
			UINT32 numComponentProxies = (UINT32)current.proxy->components.size();
			for (auto& component : components)
			{
				if (component->getLinkId() == -1)
				{
					bool foundInstanceData = false;
					for (; componentProxyIdx < numComponentProxies; componentProxyIdx++)
					{
						if (current.proxy->components[componentProxyIdx].linkId != -1)
							continue;

						assert(current.proxy->components[componentProxyIdx].linkId == -1);
						component->_setInstanceData(current.proxy->components[componentProxyIdx].instanceData);
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

				if (child->getLinkId() == -1)
				{
					bool foundInstanceData = false;
					for (; childProxyIdx < numChildProxies; childProxyIdx++)
					{
						if (current.proxy->children[childProxyIdx].linkId != -1)
							continue;

						assert(current.proxy->children[childProxyIdx].linkId == -1);
						child->_setInstanceData(current.proxy->children[childProxyIdx].instanceData);

						todo.push(StackEntry());

						StackEntry& newEntry = todo.top();
						newEntry.so = child;
						newEntry.proxy = &current.proxy->children[childProxyIdx];

						foundInstanceData = true;
						break;
					}

					assert(foundInstanceData);
				}
				else
				{
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