#include "BsScriptEditorSettings.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsEditorApplication.h"
#include "BsEditorSettings.h"

namespace BansheeEngine
{
	ScriptEditorSettings::ScriptEditorSettings(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorSettings::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetMoveHandleSnapActive", &ScriptEditorSettings::internal_GetMoveHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_SetMoveHandleSnapActive", &ScriptEditorSettings::internal_SetMoveHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_GetRotateHandleSnapActive", &ScriptEditorSettings::internal_GetRotateHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_SetRotateHandleSnapActive", &ScriptEditorSettings::internal_SetRotateHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_GetScaleHandleSnapActive", &ScriptEditorSettings::internal_GetScaleHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_SetScaleHandleSnapActive", &ScriptEditorSettings::internal_SetScaleHandleSnapActive);
		metaData.scriptClass->addInternalCall("Internal_GetMoveHandleSnapAmount", &ScriptEditorSettings::internal_GetMoveHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_SetMoveHandleSnapAmount", &ScriptEditorSettings::internal_SetMoveHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_GetRotateHandleSnapAmount", &ScriptEditorSettings::internal_GetRotateHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_SetRotateHandleSnapAmount", &ScriptEditorSettings::internal_SetRotateHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_GetScaleHandleSnapAmount", &ScriptEditorSettings::internal_GetScaleHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_SetScaleHandleSnapAmount", &ScriptEditorSettings::internal_SetScaleHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_GetDefaultHandleSize", &ScriptEditorSettings::internal_GetDefaultHandleSize);
		metaData.scriptClass->addInternalCall("Internal_SetDefaultHandleSize", &ScriptEditorSettings::internal_SetDefaultHandleSize);
	}

	bool ScriptEditorSettings::internal_GetMoveHandleSnapActive()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getMoveHandleSnapActive();
	}

	void ScriptEditorSettings::internal_SetMoveHandleSnapActive(bool value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setMoveHandleSnapActive(value);
	}

	bool ScriptEditorSettings::internal_GetRotateHandleSnapActive()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getRotateHandleSnapActive();
	}

	void ScriptEditorSettings::internal_SetRotateHandleSnapActive(bool value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setRotateHandleSnapActive(value);
	}

	bool ScriptEditorSettings::internal_GetScaleHandleSnapActive()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getScaleHandleSnapActive();
	}

	void ScriptEditorSettings::internal_SetScaleHandleSnapActive(bool value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setScaleHandleSnapActive(value);
	}

	float ScriptEditorSettings::internal_GetMoveHandleSnapAmount()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getMoveHandleSnap();
	}

	void ScriptEditorSettings::internal_SetMoveHandleSnapAmount(float value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setMoveHandleSnap(value);
	}

	float ScriptEditorSettings::internal_GetRotateHandleSnapAmount()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getRotationHandleSnap().valueDegrees();
	}

	void ScriptEditorSettings::internal_SetRotateHandleSnapAmount(float value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setRotationHandleSnap(Degree(value));
	}

	float ScriptEditorSettings::internal_GetScaleHandleSnapAmount()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getScaleHandleSnap();
	}

	void ScriptEditorSettings::internal_SetScaleHandleSnapAmount(float value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setScaleHandleSnap(value);
	}

	float ScriptEditorSettings::internal_GetDefaultHandleSize()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getHandleSize();
	}

	void ScriptEditorSettings::internal_SetDefaultHandleSize(float value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setHandleSize(value);
	}
}
