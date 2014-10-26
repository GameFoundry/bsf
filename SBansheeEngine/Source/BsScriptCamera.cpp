#include "BsScriptCamera.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptCamera::ScriptCamera(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ 
		// TODO - Create handler
	}

	ScriptCamera::~ScriptCamera()
	{
		// TODO - Delete handler
	}

	void ScriptCamera::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetAspect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetAspect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetNearClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetNearClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetFarClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetFarClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetFieldOfView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetFieldOfView", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetViewportRect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetViewportRect", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetProjectionType", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetProjectionType", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicHeight", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetOrthographicHeight", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicWidth", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetClearColor", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetClearColor", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetDepthClearValue", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetDepthClearValue", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetStencilClearValue", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetStencilClearValue", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetClearFlags", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetClearFlags", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetPriority", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetProjMatrix", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetProjMatrixInv", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetViewMatrix", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetViewMatrixInv", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetWidthPixels", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetHeightPixels", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_WorldToScreen", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_WorldToClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_WorldToView", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ScreenToView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ScreenToClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ViewToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ViewToScreen", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ViewToClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ClipToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ClipToView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ClipToScreen", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorldRay", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ProjectPoint", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_UnprojectPoint", &ScriptCamera::internal_GetAspect);
	}

	float ScriptCamera::internal_GetAspect(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetAspect(ScriptCamera* instance, float value)
	{

	}

	float ScriptCamera::internal_GetNearClip(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetNearClip(ScriptCamera* instance, float value)
	{

	}

	float ScriptCamera::internal_GetFarClip(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetFarClip(ScriptCamera* instance, float value)
	{

	}

	Degree ScriptCamera::internal_GetFieldOfView(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetFieldOfView(ScriptCamera* instance, Degree value)
	{

	}

	Rect2 ScriptCamera::internal_GetViewportRect(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetViewportRect(ScriptCamera* instance, Rect2 value)
	{

	}

	UINT32 ScriptCamera::internal_GetProjectionType(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetProjectionType(ScriptCamera* instance, UINT32 value)
	{

	}

	float ScriptCamera::internal_GetOrthographicHeight(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetOrthographicHeight(ScriptCamera* instance, float value)
	{

	}

	float ScriptCamera::internal_GetOrthographicWidth(ScriptCamera* instance)
	{

	}

	Color ScriptCamera::internal_GetClearColor(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetClearColor(ScriptCamera* instance, Color value)
	{

	}

	int ScriptCamera::internal_GetDepthClearValue(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetDepthClearValue(ScriptCamera* instance, int value)
	{

	}

	UINT8 ScriptCamera::internal_GetStencilClearValue(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetStencilClearValue(ScriptCamera* instance, UINT8 value)
	{

	}

	UINT32 ScriptCamera::internal_GetClearFlags(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetClearFlags(ScriptCamera* instance, UINT32 value)
	{

	}

	int ScriptCamera::internal_GetPriority(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetPriority(ScriptCamera* instance, int value)
	{

	}

	UINT64 ScriptCamera::internal_GetLayers(ScriptCamera* instance)
	{

	}

	void ScriptCamera::internal_SetLayers(ScriptCamera* instance, UINT64 value)
	{

	}

	Matrix4 ScriptCamera::internal_GetProjMatrix(ScriptCamera* instance)
	{

	}

	Matrix4 ScriptCamera::internal_GetProjMatrixInv(ScriptCamera* instance)
	{

	}

	Matrix4 ScriptCamera::internal_GetViewMatrix(ScriptCamera* instance)
	{

	}

	Matrix4 ScriptCamera::internal_GetViewMatrixInv(ScriptCamera* instance)
	{

	}

	int ScriptCamera::internal_GetWidthPixels(ScriptCamera* instance)
	{

	}

	int ScriptCamera::internal_GetHeightPixels(ScriptCamera* instance)
	{

	}

	Vector2I ScriptCamera::internal_WorldToScreen(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector2 ScriptCamera::internal_WorldToClip(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector3 ScriptCamera::internal_WorldToView(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector3 ScriptCamera::internal_ScreenToWorld(ScriptCamera* instance, Vector2I value)
	{

	}

	Vector3 ScriptCamera::internal_ScreenToView(ScriptCamera* instance, Vector2I value)
	{

	}

	Vector2 ScriptCamera::internal_ScreenToClip(ScriptCamera* instance, Vector2I value)
	{

	}

	Vector3 ScriptCamera::internal_ViewToWorld(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector2I ScriptCamera::internal_ViewToScreen(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector2 ScriptCamera::internal_ViewToClip(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector3 ScriptCamera::internal_ClipToWorld(ScriptCamera* instance, Vector2 value)
	{

	}

	Vector3 ScriptCamera::internal_ClipToView(ScriptCamera* instance, Vector2 value)
	{

	}

	Vector2I ScriptCamera::internal_ClipToScreen(ScriptCamera* instance, Vector2 value)
	{

	}

	Ray ScriptCamera::internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I value)
	{

	}

	Vector3 ScriptCamera::internal_ProjectPoint(ScriptCamera* instance, Vector3 value)
	{

	}

	Vector3 ScriptCamera::internal_UnprojectPoint(ScriptCamera* instance, Vector3 value)
	{

	}
}