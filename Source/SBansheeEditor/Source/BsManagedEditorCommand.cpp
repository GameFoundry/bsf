//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsManagedEditorCommand.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	MonoMethod* ScriptCmdManaged::sCommitMethod = nullptr;
	MonoMethod* ScriptCmdManaged::sRevertMethod = nullptr;

	ScriptCmdManaged::ScriptCmdManaged(MonoObject* managedInstance)
		:ScriptObject(managedInstance), mManagedCommand(nullptr)
	{
		mManagedCommand = new (bs_alloc<CmdManaged>()) CmdManaged(this);
	}

	ScriptCmdManaged::~ScriptCmdManaged()
	{
		if(mManagedCommand != nullptr)
		{
			mManagedCommand->notifyScriptInstanceDestroyed();
			mManagedCommand = nullptr;
		}
	}

	void ScriptCmdManaged::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptCmdManaged::internal_CreateInstance);

		sCommitMethod = metaData.scriptClass->getMethod("Commit");
		sRevertMethod = metaData.scriptClass->getMethod("Revert");
	}

	void ScriptCmdManaged::internal_CreateInstance(MonoObject* managedInstance)
	{
		new (bs_alloc<ScriptCmdManaged>()) ScriptCmdManaged(managedInstance);
	}

	void ScriptCmdManaged::triggerCommit()
	{
		if (sCommitMethod == nullptr)
			return;

		sCommitMethod->invokeVirtual(mManagedInstance, nullptr);
	}

	void ScriptCmdManaged::triggerRevert()
	{
		if (sRevertMethod == nullptr)
			return;

		sRevertMethod->invokeVirtual(mManagedInstance, nullptr);
	}

	void ScriptCmdManaged::notifyCommandDestroyed()
	{
		mManagedCommand = nullptr;
	}

	CmdManaged::CmdManaged(ScriptCmdManaged* scriptObj)
		: EditorCommand(L""), mScriptObj(scriptObj)
	{

	}

	CmdManaged::~CmdManaged()
	{
		if (mScriptObj != nullptr)
			mScriptObj->notifyCommandDestroyed();
	}

	void CmdManaged::commit()
	{
		if(mScriptObj == nullptr)
		{
			LOGWRN("Trying to execute a managed undo/redo command whose managed object has been destroyed, ignoring.");
			// Note: This can most likely happen if managed undo commands are queued on a global undo/redo stack. When
			// assembly refresh happens those commands will be destroyed but not removed from the stack. To fix the issue
			// implement assembly refresh handling for such commands.

			return;
		}

		mScriptObj->triggerCommit();
	}

	void CmdManaged::revert()
	{
		if (mScriptObj == nullptr)
		{
			LOGWRN("Trying to execute a managed undo/redo command whose managed object has been destroyed, ignoring.");
			// Note: This can most likely happen if managed undo commands are queued on a global undo/redo stack. When
			// assembly refresh happens those commands will be destroyed but not removed from the stack. To fix the issue
			// implement assembly refresh handling for such commands.

			return;
		}

		mScriptObj->triggerRevert();
	}

	void CmdManaged::notifyScriptInstanceDestroyed()
	{
		mScriptObj = nullptr;
	}
}