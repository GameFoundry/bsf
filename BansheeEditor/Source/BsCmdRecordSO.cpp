#include "BsCmdRecordSO.h"
#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsMemorySerializer.h"

namespace BansheeEngine
{
	CmdRecordSO::CmdRecordSO(const HSceneObject& sceneObject)
		:mSceneObject(sceneObject), mSerializedObject(nullptr), mSerializedObjectParentId(0), mSerializedObjectSize(0)
	{

	}

	CmdRecordSO::~CmdRecordSO()
	{
		mSceneObject = nullptr;
		clear();
	}

	void CmdRecordSO::clear()
	{
		mSerializedObjectSize = 0;
		mSerializedObjectParentId = 0;

		if (mSerializedObject != nullptr)
		{
			bs_free(mSerializedObject);
			mSerializedObject = nullptr;
		}
	}

	void CmdRecordSO::execute(const HSceneObject& sceneObject)
	{
		// Register command and commit it
		CmdRecordSO* command = new (bs_alloc<CmdRecordSO>()) CmdRecordSO(sceneObject);
		UndoRedo::instance().registerCommand(command);
		command->commit();
	}

	void CmdRecordSO::commit()
	{
		clear();

		if (mSceneObject == nullptr || mSceneObject.isDestroyed())
			return;

		recordSO(mSceneObject);
	}

	void CmdRecordSO::revert()
	{
		if (mSceneObject == nullptr)
			return;

		HSceneObject parent;
		if (mSerializedObjectParentId != 0)
			parent = GameObjectManager::instance().getObject(mSerializedObjectParentId);

		GameObjectManager::instance().setDeserializationMode(GODM_RestoreExternal | GODM_UseNewIds);

		if (!mSceneObject.isDestroyed())
			mSceneObject->destroy();

		MemorySerializer serializer;
		std::shared_ptr<SceneObject> restored = std::static_pointer_cast<SceneObject>(serializer.decode(mSerializedObject, mSerializedObjectSize));

		restoreIds(restored->getHandle());
		restored->setParent(parent);
	}

	void CmdRecordSO::recordSO(const HSceneObject& sceneObject)
	{
		MemorySerializer serializer;
		mSerializedObject = serializer.encode(mSceneObject.get(), mSerializedObjectSize, &bs_alloc);

		HSceneObject parent = mSceneObject->getParent();
		if (parent != nullptr)
			mSerializedObjectParentId = parent->getInstanceId();

		mSceneObjectProxy = createProxy(mSceneObject);
	}

	CmdRecordSO::SceneObjProxy CmdRecordSO::createProxy(const HSceneObject& sceneObject)
	{
		struct TempData
		{
			TempData(SceneObjProxy& proxy, const HSceneObject& so)
				:proxy(proxy), obj(so)
			{ }

			SceneObjProxy& proxy;
			HSceneObject obj;
		};

		SceneObjProxy rootProxy;

		Stack<TempData> todo;
		todo.push(TempData(rootProxy, sceneObject));

		while (!todo.empty())
		{
			TempData data = todo.top();
			todo.pop();

			data.proxy.instanceData = data.obj->_getInstanceData();

			const Vector<HComponent>& components = data.obj->getComponents();
			for (auto& component : components)
				data.proxy.componentInstanceData.push_back(component->_getInstanceData());

			UINT32 numChildren = data.obj->getNumChildren();
			data.proxy.children.resize(numChildren);
			for (UINT32 i = 0; i < numChildren; i++)
			{
				todo.push(TempData(data.proxy.children[i], data.obj->getChild(i)));
			}
		}

		return rootProxy;
	}


	void CmdRecordSO::restoreIds(const HSceneObject& restored)
	{
		// Note: This method relies that all restored GameObject handles pointing to the
		// same object also have the same shared handle data (Since I only update instance
		// data on a single handle I know exists, and expect all others will be updated
		// by that as well).

		struct TempData
		{
			TempData(SceneObjProxy& proxy, const HSceneObject& restoredObj)
			:proxy(proxy), restoredObj(restoredObj)
			{ }

			SceneObjProxy& proxy;
			HSceneObject restoredObj;
		};

		Stack<TempData> todo;
		todo.push(TempData(mSceneObjectProxy, restored));

		while (!todo.empty())
		{
			TempData data = todo.top();
			todo.pop();

			data.restoredObj->_setInstanceData(data.proxy.instanceData);

			// Find components that are still active and swap the old ones with restored ones,
			// keep any other as is.
			const Vector<HComponent>& restoredComponents = data.restoredObj->getComponents();

			UINT32 idx = 0;
			for (auto& restoredComponent : restoredComponents)
			{
				restoredComponent->_setInstanceData(data.proxy.componentInstanceData[idx]);

				GameObjectPtr restoredPtr = std::static_pointer_cast<GameObject>(restoredComponent.getInternalPtr());
				HComponent restoredComponentCopy = restoredComponent; // To remove const
				restoredComponentCopy._setHandleData(restoredPtr);

				idx++;
			}
			
			// Find children that are still active and swap the old ones with restored ones,
			// keep any other as is
			UINT32 restoredNumChildren = data.restoredObj->getNumChildren();
			for (UINT32 i = 0; i < restoredNumChildren; i++)
			{
				todo.push(TempData(data.proxy.children[i], data.restoredObj->getChild(i)));
			}
		}
	}
}