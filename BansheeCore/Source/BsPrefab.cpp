#include "BsPrefab.h"
#include "BsPrefabRTTI.h"
#include "BsResources.h"
#include "BsSceneObject.h"
#include "BsPrefabUtility.h"

namespace BansheeEngine
{
	Prefab::Prefab()
		:Resource(false)
	{
		
	}

	HPrefab Prefab::create(const HSceneObject& sceneObject)
	{
		assert(sceneObject->mPrefabLink == nullptr);

		PrefabPtr newPrefab = createEmpty();
		newPrefab->initialize(sceneObject);

		HPrefab handle = static_resource_cast<Prefab>(gResources()._createResourceHandle(newPrefab));
		sceneObject->mPrefabLink = handle;

		return handle;
	}

	PrefabPtr Prefab::createEmpty()
	{
		PrefabPtr newPrefab = bs_core_ptr<Prefab, PoolAlloc>(new (bs_alloc<Prefab, PoolAlloc>()) Prefab());
		newPrefab->_setThisPtr(newPrefab);

		return newPrefab;
	}

	void Prefab::initialize(const HSceneObject& sceneObject)
	{
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

	HSceneObject Prefab::instantiate()
	{
		if (mRoot == nullptr)
			return HSceneObject();

		HSceneObject clone = mRoot->clone();
		clone->instantiate();

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