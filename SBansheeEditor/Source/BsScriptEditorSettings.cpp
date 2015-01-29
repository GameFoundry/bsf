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
		metaData.scriptClass->addInternalCall("Internal_GetMoveHandleSnapAmount", &ScriptEditorSettings::internal_GetMoveHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_SetMoveHandleSnapAmount", &ScriptEditorSettings::internal_SetMoveHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_GetRotateHandleSnapAmount", &ScriptEditorSettings::internal_GetRotateHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_SetRotateHandleSnapAmount", &ScriptEditorSettings::internal_SetRotateHandleSnapAmount);
		metaData.scriptClass->addInternalCall("Internal_GetDefaultHandleSize", &ScriptEditorSettings::internal_GetDefaultHandleSize);
		metaData.scriptClass->addInternalCall("Internal_SetDefaultHandleSize", &ScriptEditorSettings::internal_SetDefaultHandleSize);
		metaData.scriptClass->addInternalCall("Internal_GetActiveSceneTool", &ScriptEditorSettings::internal_GetActiveSceneTool);
		metaData.scriptClass->addInternalCall("Internal_SetActiveSceneTool", &ScriptEditorSettings::internal_SetActiveSceneTool);
		metaData.scriptClass->addInternalCall("Internal_GetActiveCoordinateMode", &ScriptEditorSettings::internal_GetActiveCoordinateMode);
		metaData.scriptClass->addInternalCall("Internal_SetActiveCoordinateMode", &ScriptEditorSettings::internal_SetActiveCoordinateMode);
		metaData.scriptClass->addInternalCall("Internal_GetActivePivotMode", &ScriptEditorSettings::internal_GetActivePivotMode);
		metaData.scriptClass->addInternalCall("Internal_SetActivePivotMode", &ScriptEditorSettings::internal_SetActivePivotMode);
		metaData.scriptClass->addInternalCall("Internal_GetHash", &ScriptEditorSettings::internal_GetHash);
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

	UINT32 ScriptEditorSettings::internal_GetActiveSceneTool()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getActiveSceneTool();
	}

	void ScriptEditorSettings::internal_SetActiveSceneTool(UINT32 value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setActiveSceneTool(value);
	}

	UINT32 ScriptEditorSettings::internal_GetActiveCoordinateMode()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getActiveCoordinateMode();
	}

	void ScriptEditorSettings::internal_SetActiveCoordinateMode(UINT32 value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setActiveCoordinateMode(value);
	}

	UINT32 ScriptEditorSettings::internal_GetActivePivotMode()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getActivePivotMode();
	}

	void ScriptEditorSettings::internal_SetActivePivotMode(UINT32 value)
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		settings->setActivePivotMode(value);
	}

	UINT32 ScriptEditorSettings::internal_GetHash()
	{
		EditorSettingsPtr settings = gEditorApplication().getEditorSettings();
		return settings->getHash();
	}
}
