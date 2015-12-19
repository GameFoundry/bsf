#include "BsScriptSceneViewHandler.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsSceneViewHandler.h"
#include "BsScriptCamera.h"
#include "BsScriptEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	ScriptSceneViewHandler::ScriptSceneViewHandler(MonoObject* object, EditorWidgetBase* parentWidget, const SPtr<Camera>& camera)
		:ScriptObject(object), mHandler(nullptr)
	{ 
		mHandler = bs_new<SceneViewHandler>(parentWidget, camera);
	}

	ScriptSceneViewHandler::~ScriptSceneViewHandler()
	{
		bs_delete(mHandler);
	}

	void ScriptSceneViewHandler::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptSceneViewHandler::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Update", &ScriptSceneViewHandler::internal_Update);
		metaData.scriptClass->addInternalCall("Internal_UpdateHandle", &ScriptSceneViewHandler::internal_UpdateHandle);
		metaData.scriptClass->addInternalCall("Internal_UpdateSelection", &ScriptSceneViewHandler::internal_UpdateSelection);
		metaData.scriptClass->addInternalCall("Internal_TrySelectHandle", &ScriptSceneViewHandler::internal_TrySelectHandle);
		metaData.scriptClass->addInternalCall("Internal_IsHandleActive", &ScriptSceneViewHandler::internal_IsHandleActive);
		metaData.scriptClass->addInternalCall("Internal_ClearHandleSelection", &ScriptSceneViewHandler::internal_ClearHandleSelection);
		metaData.scriptClass->addInternalCall("Internal_PickObject", &ScriptSceneViewHandler::internal_PickObject);
	}

	void ScriptSceneViewHandler::internal_Create(MonoObject* managedInstance, ScriptEditorWindow* parentWindow, ScriptCamera* camera)
	{
		EditorWidgetBase* widget = nullptr;
		
		if (parentWindow != nullptr && !parentWindow->isDestroyed())
			widget = parentWindow->getEditorWidget();

		new (bs_alloc<ScriptSceneViewHandler>()) ScriptSceneViewHandler(managedInstance, widget, camera->getInternal());
	}

	void ScriptSceneViewHandler::internal_Update(ScriptSceneViewHandler* thisPtr)
	{
		thisPtr->mHandler->update();
	}

	void ScriptSceneViewHandler::internal_UpdateHandle(ScriptSceneViewHandler* thisPtr, Vector2I* inputPos, Vector2I* inputDelta)
	{
		thisPtr->mHandler->updateHandle(*inputPos, *inputDelta);
	}

	void ScriptSceneViewHandler::internal_UpdateSelection(ScriptSceneViewHandler* thisPtr)
	{
		thisPtr->mHandler->updateSelection();
	}

	void ScriptSceneViewHandler::internal_TrySelectHandle(ScriptSceneViewHandler* thisPtr, Vector2I* inputPos)
	{
		thisPtr->mHandler->trySelectHandle(*inputPos);
	}

	bool ScriptSceneViewHandler::internal_IsHandleActive(ScriptSceneViewHandler* thisPtr)
	{
		return thisPtr->mHandler->isHandleActive();
	}

	void ScriptSceneViewHandler::internal_ClearHandleSelection(ScriptSceneViewHandler* thisPtr)
	{
		thisPtr->mHandler->clearHandleSelection();
	}

	void ScriptSceneViewHandler::internal_PickObject(ScriptSceneViewHandler* thisPtr, Vector2I* inputPos, bool additive)
	{
		thisPtr->mHandler->pickObject(*inputPos, additive);
	}
}