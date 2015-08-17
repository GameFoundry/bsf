#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector3.h"
#include "BsMatrix4.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GizmoManager.
	 */
	class BS_SCR_BED_EXPORT ScriptGizmos : public ScriptObject<ScriptGizmos>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "Gizmos")

	private:
		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_SetColor(Color color);
		static void internal_GetColor(Color* color);
		static void internal_SetTransform(Matrix4 transform);
		static void internal_GetTransform(Matrix4* transform);
		static void internal_DrawCube(Vector3 position, Vector3 extents);
		static void internal_DrawSphere(Vector3 position, float radius);
		static void internal_DrawWireCube(Vector3 position, Vector3 extents);
		static void internal_DrawWireSphere(Vector3 position, float radius);
		static void internal_DrawLine(Vector3 start, Vector3 end);
		static void internal_DrawFrustum(Vector3 position, float aspect, Degree FOV, float near, float far);
		static void internal_DrawIcon(Vector3 position, MonoObject* image, bool fixedScale);
	};
}