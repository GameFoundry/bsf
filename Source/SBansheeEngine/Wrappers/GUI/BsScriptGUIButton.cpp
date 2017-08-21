//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/GUI/BsScriptGUIButton.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "2D/BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "GUI/BsGUILayout.h"
#include "GUI/BsGUIButton.h"
#include "GUI/BsGUIOptions.h"
#include "Wrappers/GUI/BsScriptGUIElementStyle.h"
#include "Wrappers/GUI/BsScriptGUILayout.h"
#include "Wrappers/BsScriptHString.h"
#include "Wrappers/GUI/BsScriptGUIContent.h"

namespace bs
{
	ScriptGUIButton::OnClickThunkDef ScriptGUIButton::onClickThunk;
	ScriptGUIButton::OnHoverThunkDef ScriptGUIButton::onHoverThunk;
	ScriptGUIButton::OnOutThunkDef ScriptGUIButton::onOutThunk;
	ScriptGUIButton::OnDoubleClickThunkDef ScriptGUIButton::onDoubleClickThunk;

	ScriptGUIButton::ScriptGUIButton(MonoObject* instance, GUIButton* button)
		:TScriptGUIElement(instance, button)
	{

	}

	void ScriptGUIButton::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIButton::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIButton::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIButton::internal_setTint);

		onClickThunk = (OnClickThunkDef)metaData.scriptClass->getMethod("DoOnClick")->getThunk();
		onDoubleClickThunk = (OnDoubleClickThunkDef)metaData.scriptClass->getMethod("DoOnDoubleClick")->getThunk();
		onHoverThunk = (OnHoverThunkDef)metaData.scriptClass->getMethod("DoOnHover")->getThunk();
		onOutThunk = (OnOutThunkDef)metaData.scriptClass->getMethod("DoOnOut")->getThunk();
	}

	void ScriptGUIButton::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		ScriptArray scriptArray(guiOptions);
		UINT32 arrayLen = scriptArray.size();
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(scriptArray.get<GUIOption>(i));

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIButton* guiButton = GUIButton::create(nativeContent, options, toString(MonoUtil::monoToWString(style)));

		guiButton->onClick.connect(std::bind(&ScriptGUIButton::onClick, instance));
		guiButton->onDoubleClick.connect(std::bind(&ScriptGUIButton::onDoubleClick, instance));
		guiButton->onHover.connect(std::bind(&ScriptGUIButton::onHover, instance));
		guiButton->onOut.connect(std::bind(&ScriptGUIButton::onOut, instance));

		new (bs_alloc<ScriptGUIButton>()) ScriptGUIButton(instance, guiButton);
	}

	void ScriptGUIButton::internal_setContent(ScriptGUIButton* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));

		GUIButton* button = (GUIButton*)nativeInstance->getGUIElement();
		button->setContent(nativeContent);
	}

	void ScriptGUIButton::internal_setTint(ScriptGUIButton* nativeInstance, Color* color)
	{
		GUIButton* button = (GUIButton*)nativeInstance->getGUIElement();
		button->setTint(*color);
	}

	void ScriptGUIButton::onClick(MonoObject* instance)
	{
		MonoUtil::invokeThunk(onClickThunk, instance);
	}

	void ScriptGUIButton::onDoubleClick(MonoObject* instance)
	{
		MonoUtil::invokeThunk(onDoubleClickThunk, instance);
	}

	void ScriptGUIButton::onHover(MonoObject* instance)
	{
		MonoUtil::invokeThunk(onHoverThunk, instance);
	}

	void ScriptGUIButton::onOut(MonoObject* instance)
	{
		MonoUtil::invokeThunk(onOutThunk, instance);
	}
}