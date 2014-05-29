#include "BsCmdReparentSO.h"
#include "CmSceneObject.h"

namespace BansheeEngine
{
	CmdReparentSO::CmdReparentSO(const Vector<HSceneObject>& sceneObjects, const HSceneObject& newParent)
		:mSceneObjects(sceneObjects), mNewParent(newParent)
	{
		for(auto& sceneObject : mSceneObjects)
		{
			mOldParents.push_back(sceneObject->getParent());
		}
	}

	void CmdReparentSO::execute(const Vector<HSceneObject>& sceneObjects, const HSceneObject& newParent)
	{
		// Register command and commit it
		CmdReparentSO* command = new (bs_alloc<CmdReparentSO>()) CmdReparentSO(sceneObjects, newParent);
		UndoRedo::instance().registerCommand(command);
		command->commit();
	}

	void CmdReparentSO::commit()
	{
		if(mNewParent.isDestroyed())
			return;

		UINT32 cnt = 0;
		for(auto& sceneObject : mSceneObjects)
		{
			if(!sceneObject.isDestroyed())
				sceneObject->setParent(mNewParent);

			cnt++;
		}
	}

	void CmdReparentSO::revert()
	{
		UINT32 cnt = 0;
		for(auto& sceneObject : mSceneObjects)
		{
			if(!sceneObject.isDestroyed() && !mOldParents[cnt].isDestroyed())
				sceneObject->setParent(mOldParents[cnt]);

			cnt++;
		}
	}
}