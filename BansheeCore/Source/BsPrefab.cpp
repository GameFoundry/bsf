#include "BsPrefab.h"
#include "BsPrefabRTTI.h"
#include "BsResources.h"
#include "BsSceneObject.h"
#include "BsPrefabUtility.h"

namespace BansheeEngine
{
	Prefab::Prefab()
		:Resource(false), mHash(0)
	{
		
	}

	HPrefab Prefab::create(const HSceneObject& sceneObject)
	{
		PrefabPtr newPrefab = createEmpty();
		newPrefab->initialize(sceneObject);

		HPrefab handle = static_resource_cast<Prefab>(gResources()._createResourceHandle(newPrefab));
		sceneObject->mPrefabLinkUUID = handle.getUUID();

		return handle;
	}

	PrefabPtr Prefab::createEmpty()
	{
		PrefabPtr newPrefab = bs_core_ptr<Prefab>(new (bs_alloc<Prefab>()) Prefab());
		newPrefab->_setThisPtr(newPrefab);

		return newPrefab;
	}

	void Prefab::initialize(const HSceneObject& sceneObject)
	{
		sceneObject->breakPrefabLink();
		PrefabUtility::generatePrefabIds(sceneObject);

		sceneObject->setFlags(SOF_DontInstantiate);
		mRoot = sceneObject->clone();
		sceneObject->unsetFlags(SOF_DontInstantiate);

		// Remove objects with "dont save" flag
		Stack<HSceneObject> todo;
		todo.push(mRoot);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			if (current->hasFlag(SOF_DontSave))
				current->destroy();
			else
			{
				UINT32 numChildren = current->getNumChildren();
				for (UINT32 i = 0; i < numChildren; i++)
					todo.push(current->getChild(i));
			}
		}
	}

	void Prefab::update(const HSceneObject& sceneObject)
	{
		initialize(sceneObject);
		mHash++;
	}

	HSceneObject Prefab::instantiate()
	{
		if (mRoot == nullptr)
			return HSceneObject();

		HSceneObject clone = mRoot->clone();
		clone->instantiate();
		clone->mPrefabHash = mHash;

#if BS_EDITOR_BUILD
		// Update any child prefab instances in case their prefabs changed
		Stack<HSceneObject> todo;
		todo.push(clone);

		Vector<HSceneObject> prefabInstanceRoots;

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			UINT32 childCount = current->getNumChildren();
			for (UINT32 i = 0; i < childCount; i++)
			{
				HSceneObject child = current->getChild(i);

				String prefabLinkUUID = child->getPrefabLink();
				if (!prefabLinkUUID.empty())
					PrefabUtility::updateFromPrefab(child);
				else
					todo.push(child);
			}
		}
#endif

		return clone;
	}

	RTTITypeBase* Prefab::getRTTIStatic()
	{
		return PrefabRTTI::instance();
	}

	RTTITypeBase* Prefab::getRTTI() const
	{
		return Prefab::getRTTIStatic();
	}
}