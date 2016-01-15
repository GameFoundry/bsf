#include "BsScriptGUIWidget.h"
#include "BsScriptSceneObject.h"
#include "BsScriptGUISkin.h"
#include "BsScriptCamera.h"
#include "BsScriptGUILayout.h"
#include "BsGUIWidget.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	ScriptGUIWidget::ScriptGUIWidget(MonoObject* managedInstance)
		:ScriptObject(managedInstance), mGUIWidget(nullptr), mLastUpdateHash((UINT32)-1)
	{
		CameraPtr mainCamera = gSceneManager().getMainCamera().camera;

		mGUIWidget = GUIWidget::create(mainCamera);

		MonoObject* guiPanel = ScriptGUIPanel::createFromExisting(mGUIWidget->getPanel());
		mPanel = ScriptGUILayout::toNative(guiPanel);
	}

	ScriptGUIWidget::~ScriptGUIWidget()
	{ }

	void ScriptGUIWidget::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptGUIWidget::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", &ScriptGUIWidget::internal_UpdateTransform);
		metaData.scriptClass->addInternalCall("Internal_UpdateMainCamera", &ScriptGUIWidget::internal_UpdateMainCamera);
		metaData.scriptClass->addInternalCall("Internal_SetSkin", &ScriptGUIWidget::internal_SetSkin);
		metaData.scriptClass->addInternalCall("Internal_SetCamera", &ScriptGUIWidget::internal_SetCamera);
		metaData.scriptClass->addInternalCall("Internal_GetPanel", &ScriptGUIWidget::internal_GetPanel);
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIWidget::internal_Destroy);
	}

	void ScriptGUIWidget::internal_Create(MonoObject* managedInstance)
	{
		ScriptGUIWidget* nativeInstance = new (bs_alloc<ScriptGUIWidget>()) ScriptGUIWidget(managedInstance);
	}

	void ScriptGUIWidget::internal_UpdateTransform(ScriptGUIWidget* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		SPtr<GUIWidget> widget = thisPtr->getInternal();
		if (!parentSO.isDestroyed() && widget != nullptr)
		{
			widget->_updateTransform(parentSO);

			if (parentSO->getActive() != widget->getIsActive())
				widget->setIsActive(parentSO->getActive());
		}
	}

	void ScriptGUIWidget::internal_UpdateMainCamera(ScriptGUIWidget* instance, ScriptCamera* camera)
	{
		SPtr<GUIWidget> widget = instance->getInternal();

		if (widget != nullptr)
		{
			CameraPtr nativeCamera;
			if (camera != nullptr)
				nativeCamera = camera->getInternal();

			widget->setCamera(nativeCamera);
		}
	}

	void ScriptGUIWidget::internal_SetSkin(ScriptGUIWidget* instance, ScriptGUISkin* skin)
	{
		HGUISkin guiSkin;
		if (skin != nullptr)
			guiSkin = skin->getHandle();

		SPtr<GUIWidget> widget = instance->getInternal();
		if (widget != nullptr)
			widget->setSkin(guiSkin);
	}

	void ScriptGUIWidget::internal_SetCamera(ScriptGUIWidget* instance, ScriptCamera* camera)
	{
		CameraPtr nativeCamera;
		if (camera != nullptr)
			nativeCamera = camera->getInternal();

		if(nativeCamera == nullptr)
			nativeCamera = gSceneManager().getMainCamera().camera;

		SPtr<GUIWidget> widget = instance->getInternal();
		if(widget != nullptr)
			widget->setCamera(nativeCamera);
	}

	MonoObject* ScriptGUIWidget::internal_GetPanel(ScriptGUIWidget* instance)
	{
		if (instance->mPanel != nullptr)
			return instance->mPanel->getManagedInstance();

		return nullptr;
	}

	void ScriptGUIWidget::internal_Destroy(ScriptGUIWidget* instance)
	{
		instance->destroy();
	}

	void ScriptGUIWidget::destroy()
	{
		if(mPanel != nullptr)
		{
			mPanel->destroy();
			mPanel = nullptr;
		}

		if (mGUIWidget != nullptr)
		{
			mGUIWidget->_destroy();
			mGUIWidget = nullptr;
		}
	}

	void ScriptGUIWidget::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}
}