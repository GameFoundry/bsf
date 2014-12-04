#include "BsScriptSceneViewHandler.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsSceneViewHandler.h"
#include "BsScriptCamera.h"

namespace BansheeEngine
{
	ScriptSceneViewHandler::ScriptSceneViewHandler(MonoObject* object, const SPtr<CameraHandler>& camera)
		:ScriptObject(object), mHandler(nullptr)
	{ 
		mHandler = bs_new<SceneViewHandler>(camera);
	}

	ScriptSceneViewHandler::~ScriptSceneViewHandler()
	{
		bs_delete(mHandler);
	}

	void ScriptSceneViewHandler::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptSceneViewHandler::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Update", &ScriptSceneViewHandler::internal_Update);
		metaData.scriptClass->addInternalCall("Internal_PointerPressed", &ScriptSceneViewHandler::internal_PointerPressed);
		metaData.scriptClass->addInternalCall("Internal_PointerReleased", &ScriptSceneViewHandler::internal_PointerReleased);
	}

	void ScriptSceneViewHandler::internal_Create(MonoObject* managedInstance, ScriptCamera* camera)
	{
		new (bs_alloc<ScriptSceneViewHandler>()) ScriptSceneViewHandler(managedInstance, camera->getHandler());
	}

	void ScriptSceneViewHandler::internal_Update(ScriptSceneViewHandler* thisPtr, Vector2I inputPos)
	{
		thisPtr->mHandler->update(inputPos);
	}

	void ScriptSceneViewHandler::internal_PointerPressed(ScriptSceneViewHandler* thisPtr, Vector2I inputPos)
	{
		thisPtr->mHandler->pointerPressed(inputPos);
	}

	void ScriptSceneViewHandler::internal_PointerReleased(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, bool controlPressed)
	{
		thisPtr->mHandler->pointerReleased(inputPos, controlPressed);
	}
}