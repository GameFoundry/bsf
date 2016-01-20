//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCmdCloneSO.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	CmdCloneSO::CmdCloneSO(const WString& description, const Vector<HSceneObject>& originals)
		:EditorCommand(description), mOriginals(originals)
	{

	}

	CmdCloneSO::~CmdCloneSO()
	{

	}

	HSceneObject CmdCloneSO::execute(const HSceneObject& sceneObject, const WString& description)
	{
		// Register command and commit it
		CmdCloneSO* command = new (bs_alloc<CmdCloneSO>()) CmdCloneSO(description, { sceneObject });
		UndoRedo::instance().registerCommand(command);
		command->commit();

		if (command->mClones.size() > 0)
			return command->mClones[0];

		return HSceneObject();
	}

	Vector<HSceneObject> CmdCloneSO::execute(const Vector<HSceneObject>& sceneObjects, const WString& description)
	{
		// Register command and commit it
		CmdCloneSO* command = new (bs_alloc<CmdCloneSO>()) CmdCloneSO(description, sceneObjects);
		UndoRedo::instance().registerCommand(command);
		command->commit();

		return command->mClones;
	}

	void CmdCloneSO::commit()
	{
		mClones.clear();

		for (auto& original : mOriginals)
		{
			if (!original.isDestroyed())
				mClones.push_back(original->clone());
		}
	}

	void CmdCloneSO::revert()
	{
		for (auto& clone : mClones)
		{
			if (!clone.isDestroyed())
				clone->destroy(true);
		}

		mClones.clear();
	}
}