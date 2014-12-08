#include "BsScriptSceneViewHandler.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsSceneViewHandler.h"
#include "BsScriptCameraHandler.h"
#include "BsScriptEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWindowBase.h"

namespace BansheeEngine
{
	ScriptSceneViewHandler::ScriptSceneViewHandler(MonoObject* object, const EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera)
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
		metaData.scriptClass->addInternalCall("Internal_PointerPressed", &ScriptSceneViewHandler::internal_PointerPressed);
		metaData.scriptClass->addInternalCall("Internal_PointerReleased", &ScriptSceneViewHandler::internal_PointerReleased);
	}

	void ScriptSceneViewHandler::internal_Create(MonoObject* managedInstance, ScriptEditorWindow* parentWindow, ScriptCameraHandler* camera)
	{
		EditorWidgetBase* widget = parentWindow->getEditorWidget();

		new (bs_alloc<ScriptSceneViewHandler>()) ScriptSceneViewHandler(managedInstance, widget, camera->getInternal());
	}

	void ScriptSceneViewHandler::internal_Update(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, Vector2I inputDelta)
	{
		thisPtr->mHandler->update(inputPos, inputDelta);
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