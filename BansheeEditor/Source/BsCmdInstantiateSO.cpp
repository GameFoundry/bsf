#include "BsCmdInstantiateSO.h"
#include "BsSceneObject.h"
#include "BsPrefab.h"

namespace BansheeEngine
{
	CmdInstantiateSO::CmdInstantiateSO(const WString& description, const HPrefab& prefab)
		:EditorCommand(description), mPrefab(prefab)
	{

	}

	CmdInstantiateSO::~CmdInstantiateSO()
	{

	}

	HSceneObject CmdInstantiateSO::execute(const HPrefab& prefab, const WString& description)
	{
		// Register command and commit it
		CmdInstantiateSO* command = new (bs_alloc<CmdInstantiateSO>()) CmdInstantiateSO(description, prefab);
		UndoRedo::instance().registerCommand(command);
		command->commit();

		return command->mSceneObject;
	}

	void CmdInstantiateSO::commit()
	{
		mSceneObject = mPrefab->instantiate();
	}

	void CmdInstantiateSO::revert()
	{
		if (!mSceneObject.isDestroyed())
			mSceneObject->destroy(true);

		mSceneObject = nullptr;
	}
}