#include "BsCmdCreateSO.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	CmdCreateSO::CmdCreateSO(const WString& description, const String& name, UINT32 flags)
		:EditorCommand(description), mName(name), mFlags(flags)
	{

	}

	CmdCreateSO::~CmdCreateSO()
	{

	}

	HSceneObject CmdCreateSO::execute(const String& name, UINT32 flags, const WString& description)
	{
		// Register command and commit it
		CmdCreateSO* command = new (bs_alloc<CmdCreateSO>()) CmdCreateSO(description, name, flags);
		UndoRedo::instance().registerCommand(command);
		command->commit();

		return command->mSceneObject;
	}

	void CmdCreateSO::commit()
	{
		mSceneObject = SceneObject::create(mName, mFlags);
	}

	void CmdCreateSO::revert()
	{
		if (mSceneObject == nullptr)
			return;

		if (!mSceneObject.isDestroyed())
			mSceneObject->destroy(true);
		mSceneObject = nullptr;
	}
}