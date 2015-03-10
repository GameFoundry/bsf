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
	class BS_SCR_BE_EXPORT ScriptCameraHandler : public ScriptObject<ScriptCameraHandler>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "CameraHandler")

		SPtr<CameraHandler> getInternal() const { return mCameraHandler; }

	private:
		ScriptCameraHandler(MonoObject* managedInstance, const HSceneObject& parentSO);
		~ScriptCameraHandler();

		void updateView(const HSceneObject& parent);

		static void internal_Create(MonoObject* managedInstance, ScriptSceneObject* parentSO);

		static float internal_GetAspect(ScriptCameraHandler* instance);
		static void internal_SetAspect(ScriptCameraHandler* instance, float value);
		static float internal_GetNearClip(ScriptCameraHandler* instance);
		static void internal_SetNearClip(ScriptCameraHandler* instance, float value);

		static float internal_GetFarClip(ScriptCameraHandler* instance);
		static void internal_SetFarClip(ScriptCameraHandler* instance, float value);

		static Degree internal_GetFieldOfView(ScriptCameraHandler* instance);
		static void internal_SetFieldOfView(ScriptCameraHandler* instance, Degree value);

		static Rect2 internal_GetViewportRect(ScriptCameraHandler* instance);
		static void internal_SetViewportRect(ScriptCameraHandler* instance, Rect2 value);

		static UINT32 internal_GetProjectionType(ScriptCameraHandler* instance);
		static void internal_SetProjectionType(ScriptCameraHandler* instance, UINT32 value);

		static float internal_GetOrthographicHeight(ScriptCameraHandler* instance);
		static void internal_SetOrthographicHeight(ScriptCameraHandler* instance, float value);

		static float internal_GetOrthographicWidth(ScriptCameraHandler* instance);

		static Color internal_GetClearColor(ScriptCameraHandler* instance);
		static void internal_SetClearColor(ScriptCameraHandler* instance, Color value);

		static float internal_GetDepthClearValue(ScriptCameraHandler* instance);
		static void internal_SetDepthClearValue(ScriptCameraHandler* instance, float value);

		static UINT16 internal_GetStencilClearValue(ScriptCameraHandler* instance);
		static void internal_SetStencilClearValue(ScriptCameraHandler* instance, UINT16 value);

		static UINT32 internal_GetClearFlags(ScriptCameraHandler* instance);
		static void internal_SetClearFlags(ScriptCameraHandler* instance, UINT32 value);

		static int internal_GetPriority(ScriptCameraHandler* instance);
		static void internal_SetPriority(ScriptCameraHandler* instance, int value);

		static UINT64 internal_GetLayers(ScriptCameraHandler* instance);
		static void internal_SetLayers(ScriptCameraHandler* instance, UINT64 value);

		static Matrix4 internal_GetProjMatrix(ScriptCameraHandler* instance);
		static Matrix4 internal_GetProjMatrixInv(ScriptCameraHandler* instance);

		static Matrix4 internal_GetViewMatrix(ScriptCameraHandler* instance);
		static Matrix4 internal_GetViewMatrixInv(ScriptCameraHandler* instance);

		static int internal_GetWidthPixels(ScriptCameraHandler* instance);
		static int internal_GetHeightPixels(ScriptCameraHandler* instance);

		static Vector2I internal_WorldToScreen(ScriptCameraHandler* instance, Vector3 value);
		static Vector2 internal_WorldToClip(ScriptCameraHandler* instance, Vector3 value);
		static Vector3 internal_WorldToView(ScriptCameraHandler* instance, Vector3 value);

		static Vector3 internal_ScreenToWorld(ScriptCameraHandler* instance, Vector2I value);
		static Vector3 internal_ScreenToView(ScriptCameraHandler* instance, Vector2I value);
		static Vector2 internal_ScreenToClip(ScriptCameraHandler* instance, Vector2I value);

		static Vector3 internal_ViewToWorld(ScriptCameraHandler* instance, Vector3 value);
		static Vector2I internal_ViewToScreen(ScriptCameraHandler* instance, Vector3 value);
		static Vector2 internal_ViewToClip(ScriptCameraHandler* instance, Vector3 value);

		static Vector3 internal_ClipToWorld(ScriptCameraHandler* instance, Vector2 value);
		static Vector3 internal_ClipToView(ScriptCameraHandler* instance, Vector2 value);
		static Vector2I internal_ClipToScreen(ScriptCameraHandler* instance, Vector2 value);

		static Ray internal_ScreenToWorldRay(ScriptCameraHandler* instance, Vector2I value);
		static Vector3 internal_ProjectPoint(ScriptCameraHandler* instance, Vector3 value);
		static Vector3 internal_UnprojectPoint(ScriptCameraHandler* instance, Vector3 value);

		static void internal_SetRenderTarget(ScriptCameraHandler* instance, ScriptRenderTarget* target);

		static void internal_UpdateView(ScriptCameraHandler* instance, ScriptSceneObject* parent);
		static void internal_OnDestroy(ScriptCameraHandler* instance);

		SPtr<CameraHandler> mCameraHandler;
		UINT32 mLastUpdateHash;
	};
}