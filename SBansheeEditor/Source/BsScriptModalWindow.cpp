#include "BsScriptModalWindow.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "BsCamera.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	ScriptModalWindow::ScriptModalWindow(INT32 left, INT32 top, UINT32 width, UINT32 height)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = width;
		renderWindowDesc.height = height;
		renderWindowDesc.left = left;
		renderWindowDesc.top = top;
		renderWindowDesc.title = "ModalWindow";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.toolWindow = true;

		mRenderWindow = RenderWindow::create(renderWindowDesc, gApplication().getPrimaryWindow());

		mSceneObject = SceneObject::create("ModalWindow");

		mCamera = mSceneObject->addComponent<Camera>();
		mCamera->initialize(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f);
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(1.0f);
		mCamera->setIgnoreSceneRenderables(true);

		mGUI = mSceneObject->addComponent<GUIWidget>(mCamera->getViewport().get());
		mGUI->setDepth(128);
	}

	void ScriptModalWindow::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEditor", "BansheeEditor", "ModalWindow", &ScriptModalWindow::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptModalWindow::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptModalWindow::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptModalWindow::internal_destroyInstance);
	}

	void ScriptModalWindow::internal_createInstance(MonoObject* instance, INT32 left, INT32 top, UINT32 width, UINT32 height)
	{
		ScriptModalWindow* nativeInstance = new (cm_alloc<ScriptModalWindow>()) ScriptModalWindow(left, top, width, height);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptModalWindow::internal_destroyInstance(ScriptModalWindow* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}