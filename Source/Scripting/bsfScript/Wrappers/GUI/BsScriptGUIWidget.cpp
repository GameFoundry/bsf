//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Wrappers/GUI/BsScriptGUIWidget.h"
#include "Wrappers/BsScriptSceneObject.h"
#include "Wrappers/GUI/BsScriptGUILayout.h"
#include "GUI/BsGUIWidget.h"
#include "Scene/BsSceneObject.h"
#include "Scene/BsSceneManager.h"
#include "Resources/BsBuiltinResources.h"
#include "Components/BsCCamera.h"

#include "Generated/BsScriptCCamera.generated.h"
#include "Generated/BsScriptGUISkin.generated.h"

namespace bs
{
	MonoField* ScriptGUIWidget::sGUIPanelField = nullptr;

	ScriptGUIWidget::ScriptGUIWidget(MonoObject* managedInstance)
		:ScriptObject(managedInstance), mGUIWidget(nullptr)
	{
		SPtr<Camera> mainCamera = gSceneManager().getMainCamera();

		mGUIWidget = GUIWidget::create(mainCamera);
		mGUIWidget->setSkin(BuiltinResources::instance().getGUISkin());

		MonoObject* guiPanel = ScriptGUIPanel::createFromExisting(mGUIWidget->getPanel());
		mPanel = ScriptGUILayout::toNative(guiPanel);

		sGUIPanelField->set(managedInstance, guiPanel);
	}

	ScriptGUIWidget::~ScriptGUIWidget()
	{ }

	void ScriptGUIWidget::initRuntimeData()
	{
		sGUIPanelField = metaData.scriptClass->getField("panel");

		metaData.scriptClass->addInternalCall("Internal_Create", (void*)&ScriptGUIWidget::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", (void*)&ScriptGUIWidget::internal_UpdateTransform);
		metaData.scriptClass->addInternalCall("Internal_UpdateMainCamera", (void*)&ScriptGUIWidget::internal_UpdateMainCamera);
		metaData.scriptClass->addInternalCall("Internal_SetSkin", (void*)&ScriptGUIWidget::internal_SetSkin);
		metaData.scriptClass->addInternalCall("Internal_SetCamera", (void*)&ScriptGUIWidget::internal_SetCamera);
		metaData.scriptClass->addInternalCall("Internal_SetDepth", (void*)&ScriptGUIWidget::internal_SetDepth);
		metaData.scriptClass->addInternalCall("Internal_GetDepth", (void*)&ScriptGUIWidget::internal_GetDepth);
		metaData.scriptClass->addInternalCall("Internal_Destroy", (void*)&ScriptGUIWidget::internal_Destroy);
	}

	void ScriptGUIWidget::internal_Create(MonoObject* managedInstance)
	{
		new (bs_alloc<ScriptGUIWidget>()) ScriptGUIWidget(managedInstance);
	}

	void ScriptGUIWidget::internal_UpdateTransform(ScriptGUIWidget* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getHandle();

		SPtr<GUIWidget> widget = thisPtr->getInternal();
		if (!parentSO.isDestroyed() && widget != nullptr)
		{
			widget->_updateTransform(parentSO);
			widget->_updateRT();

			if (parentSO->getActive() != widget->getIsActive())
				widget->setIsActive(parentSO->getActive());
		}
	}

	void ScriptGUIWidget::internal_UpdateMainCamera(ScriptGUIWidget* instance, ScriptCCamera* camera)
	{
		SPtr<GUIWidget> widget = instance->getInternal();

		if (widget != nullptr)
		{
			SPtr<Camera> nativeCamera;
			if (camera != nullptr)
				nativeCamera = camera->getHandle()->_getCamera();

			widget->setCamera(nativeCamera);
		}
	}

	void ScriptGUIWidget::internal_SetSkin(ScriptGUIWidget* instance, ScriptGUISkin* skin)
	{
		HGUISkin guiSkin;
		if (skin != nullptr)
			guiSkin = skin->getHandle();

		if (!guiSkin.isLoaded())
			guiSkin = BuiltinResources::instance().getGUISkin();

		SPtr<GUIWidget> widget = instance->getInternal();
		if (widget != nullptr)
			widget->setSkin(guiSkin);
	}

	void ScriptGUIWidget::internal_SetCamera(ScriptGUIWidget* instance, ScriptCCamera* camera)
	{
		SPtr<Camera> nativeCamera;
		if (camera != nullptr)
			nativeCamera = camera->getHandle()->_getCamera();

		if(nativeCamera == nullptr)
			nativeCamera = gSceneManager().getMainCamera();

		SPtr<GUIWidget> widget = instance->getInternal();
		if(widget != nullptr)
			widget->setCamera(nativeCamera);
	}

	void ScriptGUIWidget::internal_SetDepth(ScriptGUIWidget* instance, INT8 value)
	{
		instance->getInternal()->setDepth(value);
	}

	INT8 ScriptGUIWidget::internal_GetDepth(ScriptGUIWidget* instance)
	{
		return instance->getInternal()->getDepth();
	}

	void ScriptGUIWidget::internal_Destroy(ScriptGUIWidget* instance)
	{
		instance->destroy();
	}

	void ScriptGUIWidget::destroy(bool destroyPanel)
	{
		if(mPanel != nullptr && destroyPanel)
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

	void ScriptGUIWidget::_onManagedInstanceDeleted(bool assemblyRefresh)
	{
		destroy(false);

		ScriptObject::_onManagedInstanceDeleted(assemblyRefresh);
	}
}
