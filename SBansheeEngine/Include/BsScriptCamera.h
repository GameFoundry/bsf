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
	/**
	 * @brief	Interop class between C++ & CLR for Camera.
	 */
	class BS_SCR_BE_EXPORT ScriptCamera : public ScriptObject<ScriptCamera>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "NativeCamera")

		/**
		 * @brief	Returns the wrapped native Camera object.
		 */
		SPtr<Camera> getInternal() const { return mCamera; }

	private:
		ScriptCamera(MonoObject* managedInstance, const HSceneObject& parentSO);
		~ScriptCamera();

		/**
		 * @brief	Updates the internal camera handler from the transform of the
		 *			provided scene object.
		 */
		void updateView(const HSceneObject& parent);

		SPtr<Camera> mCamera;
		UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* managedInstance, ScriptSceneObject* parentSO);

		static float internal_GetAspect(ScriptCamera* instance);
		static void internal_SetAspect(ScriptCamera* instance, float value);
		static float internal_GetNearClip(ScriptCamera* instance);
		static void internal_SetNearClip(ScriptCamera* instance, float value);

		static float internal_GetFarClip(ScriptCamera* instance);
		static void internal_SetFarClip(ScriptCamera* instance, float value);

		static void internal_GetFieldOfView(ScriptCamera* instance, Degree* value);
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

		static Vector3 internal_ScreenToWorld(ScriptCamera* instance, Vector2I value, float depth);
		static Vector3 internal_ScreenToView(ScriptCamera* instance, Vector2I value, float depth);
		static Vector2 internal_ScreenToClip(ScriptCamera* instance, Vector2I value);

		static Vector3 internal_ViewToWorld(ScriptCamera* instance, Vector3 value);
		static Vector2I internal_ViewToScreen(ScriptCamera* instance, Vector3 value);
		static Vector2 internal_ViewToClip(ScriptCamera* instance, Vector3 value);

		static Vector3 internal_ClipToWorld(ScriptCamera* instance, Vector2 value, float depth);
		static Vector3 internal_ClipToView(ScriptCamera* instance, Vector2 value, float depth);
		static Vector2I internal_ClipToScreen(ScriptCamera* instance, Vector2 value);

		static Ray internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I value);
		static Vector3 internal_ProjectPoint(ScriptCamera* instance, Vector3 value);
		static Vector3 internal_UnprojectPoint(ScriptCamera* instance, Vector3 value);

		static void internal_SetRenderTarget(ScriptCamera* instance, ScriptRenderTarget* target);

		static void internal_UpdateView(ScriptCamera* instance, ScriptSceneObject* parent);
		static void internal_OnDestroy(ScriptCamera* instance);
	};
}