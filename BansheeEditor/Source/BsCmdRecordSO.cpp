#include "BsCmdRecordSO.h"
#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsMemorySerializer.h"

namespace BansheeEngine
{
	CmdRecordSO::CmdRecordSO(const WString& description, const HSceneObject& sceneObject)
		:EditorCommand(description), mSceneObject(sceneObject), mSerializedObject(nullptr), mSerializedObjectSize(0)
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
		if (mSceneObject == nullptr || mSceneObject.isDestroyed())
			return;

		HSceneObject parent = mSceneObject->getParent();

		UINT32 numChildren = mSceneObject->getNumChildren();
		Vector<HSceneObject> children(numChildren);
		for (UINT32 i = 0; i < numChildren; i++)
		{
			HSceneObject child = mSceneObject->getChild(i);
			children[i] = child;

			child->setParent(HSceneObject());
		}

		mSceneObject->destroy(true);

		GameObjectManager::instance().setDeserializationMode(GODM_RestoreExternal | GODM_UseNewIds);

		MemorySerializer serializer;
		std::shared_ptr<SceneObject> restored = std::static_pointer_cast<SceneObject>(serializer.decode(mSerializedObject, mSerializedObjectSize));

		CmdUtility::restoreIds(restored->getHandle(), mSceneObjectProxy);
		restored->setParent(parent);

		for (auto& child : children)
			child->setParent(restored->getHandle());
	}

	void CmdRecordSO::recordSO(const HSceneObject& sceneObject)
	{
		UINT32 numChildren = mSceneObject->getNumChildren();
		Vector<HSceneObject> children(numChildren);
		for (UINT32 i = 0; i < numChildren; i++)
		{
			HSceneObject child = mSceneObject->getChild(i);
			children[i] = child;

			child->setParent(HSceneObject());
		}

		MemorySerializer serializer;
		mSerializedObject = serializer.encode(mSceneObject.get(), mSerializedObjectSize);

		mSceneObjectProxy = CmdUtility::createProxy(mSceneObject);

		for (auto& child : children)
			child->setParent(sceneObject->getHandle());
	}
}