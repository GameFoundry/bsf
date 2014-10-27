#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector2I.h"
#include "BsRay.h"
#include "BsDegree.h"
#include "BsMatrix4.h"
#include "BsRect2.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptCamera : public ScriptObject<ScriptCamera>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Camera")

	private:
		ScriptCamera(MonoObject* managedInstance);
		~ScriptCamera();

		static float internal_GetAspect(ScriptCamera* instance);
		static void internal_SetAspect(ScriptCamera* instance, float value);
		static float internal_GetNearClip(ScriptCamera* instance);
		static void internal_SetNearClip(ScriptCamera* instance, float value);

		static float internal_GetFarClip(ScriptCamera* instance);
		static void internal_SetFarClip(ScriptCamera* instance, float value);

		static Degree internal_GetFieldOfView(ScriptCamera* instance);
		static void internal_SetFieldOfView(ScriptCamera* instance, Degree value);

		static Rect2 internal_GetViewportRect(ScriptCamera* instance);
		static void internal_SetViewportRect(ScriptCamera* instance, Rect2 value);

		static UINT32 internal_GetProjectionType(ScriptCamera* instance);
		static void internal_SetProjectionType(ScriptCamera* instance, UINT32 value);

		static float internal_GetOrthographicHeight(ScriptCamera* instance);
		static void internal_SetOrthographicHeight(ScriptCamera* instance, float value);

		static float internal_GetOrthographicWidth(ScriptCamera* instance);

		static Color internal_GetClearColor(ScriptCamera* instance);
		static void internal_SetClearColor(ScriptCamera* instance, Color value);

		static float internal_GetDepthClearValue(ScriptCamera* instance);
		static void internal_SetDepthClearValue(ScriptCamera* instance, float value);

		static UINT16 internal_GetStencilClearValue(ScriptCamera* instance);
		static void internal_SetStencilClearValue(ScriptCamera* instance, UINT16 value);

		static UINT32 internal_GetClearFlags(ScriptCamera* instance);
		static void internal_SetClearFlags(ScriptCamera* instance, UINT32 value);

		static int internal_GetPriority(ScriptCamera* instance);
		static void internal_SetPriority(ScriptCamera* instance, int value);

		static UINT64 internal_GetLayers(ScriptCamera* instance);
		static void internal_SetLayers(ScriptCamera* instance, UINT64 value);

		static Matrix4 internal_GetProjMatrix(ScriptCamera* instance);
		static Matrix4 internal_GetProjMatrixInv(ScriptCamera* instance);

		static Matrix4 internal_GetViewMatrix(ScriptCamera* instance);
		static Matrix4 internal_GetViewMatrixInv(ScriptCamera* instance);

		static int internal_GetWidthPixels(ScriptCamera* instance);
		static int internal_GetHeightPixels(ScriptCamera* instance);

		static Vector2I internal_WorldToScreen(ScriptCamera* instance, Vector3 value);
		static Vector2 internal_WorldToClip(ScriptCamera* instance, Vector3 value);
		static Vector3 internal_WorldToView(ScriptCamera* instance, Vector3 value);

		static Vector3 internal_ScreenToWorld(ScriptCamera* instance, Vector2I value);
		static Vector3 internal_ScreenToView(ScriptCamera* instance, Vector2I value);
		static Vector2 internal_ScreenToClip(ScriptCamera* instance, Vector2I value);

		static Vector3 internal_ViewToWorld(ScriptCamera* instance, Vector3 value);
		static Vector2I internal_ViewToScreen(ScriptCamera* instance, Vector3 value);
		static Vector2 internal_ViewToClip(ScriptCamera* instance, Vector3 value);

		static Vector3 internal_ClipToWorld(ScriptCamera* instance, Vector2 value);
		static Vector3 internal_ClipToView(ScriptCamera* instance, Vector2 value);
		static Vector2I internal_ClipToScreen(ScriptCamera* instance, Vector2 value);

		static Ray internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I value);
		static Vector3 internal_ProjectPoint(ScriptCamera* instance, Vector3 value);
		static Vector3 internal_UnprojectPoint(ScriptCamera* instance, Vector3 value);

		static MonoObject* internal_GetRenderTexture(ScriptCamera* instance);
		static void internal_SetRenderTexture(ScriptCamera* instance, MonoObject* textureObj);

		CameraHandler* mCameraHandler;
	};
}