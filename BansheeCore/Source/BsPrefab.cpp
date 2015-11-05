#include "BsPrefab.h"
#include "BsPrefabRTTI.h"
#include "BsResources.h"
#include "BsSceneObject.h"
#include "BsPrefabUtility.h"

namespace BansheeEngine
{
	Prefab::Prefab()
		:Resource(false), mHash(0), mNextLinkId(0)
	{
		
	}

	Prefab::~Prefab()
	{
		if (mRoot != nullptr)
			mRoot->destroy(true);
	}

	HPrefab Prefab::create(const HSceneObject& sceneObject)
	{
		PrefabPtr newPrefab = createEmpty();
		newPrefab->initialize(sceneObject);

		HPrefab handle = static_resource_cast<Prefab>(gResources()._createResourceHandle(newPrefab));
		newPrefab->mUUID = handle.getUUID();
		sceneObject->mPrefabLinkUUID = newPrefab->mUUID;
		newPrefab->_getRoot()->mPrefabLinkUUID = newPrefab->mUUID;

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
		sceneObject->mPrefabDiff = nullptr;
		UINT32 newNextLinkId = PrefabUtility::generatePrefabIds(sceneObject, mNextLinkId);

		if (newNextLinkId < mNextLinkId)
		{
			BS_EXCEPT(InternalErrorException, "Prefab ran out of IDs to assign. " \
				"Consider increasing the size of the prefab ID data type.");
		}

		mNextLinkId = newNextLinkId;

		// If there are any child prefab instances, make sure to update their diffs so they are saved with this prefab
		Stack<HSceneObject> todo;
		todo.push(sceneObject);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			UINT32 childCount = current->getNumChildren();
			for (UINT32 i = 0; i < childCount; i++)
			{
				HSceneObject child = current->getChild(i);

				if (!child->mPrefabLinkUUID.empty())
					PrefabUtility::recordPrefabDiff(child);
				else
					todo.push(child);
			}
		}

		// Clone the hierarchy for internal storage
		sceneObject->setFlags(SOF_DontInstantiate);
		mRoot = sceneObject->clone();
		sceneObject->unsetFlags(SOF_DontInstantiate);

		mRoot->mParent = nullptr;

		// Remove objects with "dont save" flag
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
		sceneObject->mPrefabLinkUUID = mUUID;
		mRoot->mPrefabLinkUUID = mUUID;

		mHash++;
	}

	HSceneObject Prefab::instantiate(bool onlyClone)
	{
		if (mRoot == nullptr)
			return HSceneObject();

#if BS_EDITOR_BUILD
		// Update any child prefab instances in case their prefabs changed
		Stack<HSceneObject> todo;
		todo.push(mRoot);

		while (!todo.empty())
		{
			HSceneObject current = todo.top();
			todo.pop();

			UINT32 childCount = current->getNumChildren();
			for (UINT32 i = 0; i < childCount; i++)
			{
				HSceneObject child = current->getChild(i);

				if (!child->mPrefabLinkUUID.empty())
					PrefabUtility::updateFromPrefab(child);
				else
					todo.push(child);
			}
		}
#endif

		mRoot->mPrefabHash = mHash;

		HSceneObject clone = mRoot->clone();

		if (!onlyClone)
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