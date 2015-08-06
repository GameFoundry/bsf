#include "BsCmdRecordSO.h"
#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsMemorySerializer.h"

namespace BansheeEngine
{
	CmdRecordSO::CmdRecordSO(const WString& description, const HSceneObject& sceneObject)
		:EditorCommand(description), mSceneObject(sceneObject), mSerializedObject(nullptr), mSerializedObjectParentId(0), mSerializedObjectSize(0)
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

	void CmdRecordSO::execute(const HSceneObject& sceneObject, const WString& description)
	{
		// Register command and commit it
		CmdRecordSO* command = new (bs_alloc<CmdRecordSO>()) CmdRecordSO(description, sceneObject);
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
			mSceneObject->destroy(true);

		MemorySerializer serializer;
		std::shared_ptr<SceneObject> restored = std::static_pointer_cast<SceneObject>(serializer.decode(mSerializedObject, mSerializedObjectSize));

		CmdUtility::restoreIds(restored->getHandle(), mSceneObjectProxy);
		restored->setParent(parent);
	}

	void CmdRecordSO::recordSO(const HSceneObject& sceneObject)
	{
		MemorySerializer serializer;
		mSerializedObject = serializer.encode(mSceneObject.get(), mSerializedObjectSize, &bs_alloc);

		HSceneObject parent = mSceneObject->getParent();
		if (parent != nullptr)
			mSerializedObjectParentId = parent->getInstanceId();

		mSceneObjectProxy = CmdUtility::createProxy(mSceneObject);
	}
}