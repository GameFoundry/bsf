#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector3.h"
#include "BsMatrix4.h"
#include "BsRect3.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleDrawing : public ScriptObject <ScriptHandleDrawing>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "HandleDrawing")

	private:
		static void internal_SetColor(Color color);
		static void internal_SetTransform(Matrix4 transform);

		static void internal_DrawCube(Vector3 position, Vector3 extents, float size);
		static void internal_DrawSphere(Vector3 position, float radius, float size);
		static void internal_DrawWireCube(Vector3 position, Vector3 extents, float size);
		static void internal_DrawWireSphere(Vector3 position, float radius, float size);
		static void internal_DrawLine(Vector3 start, Vector3 end, float size);
		static void internal_DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius, float size);
		static void internal_DrawDisc(Vector3 position, Vector3 normal, float radius, float size);
		static void internal_DrawWireDisc(Vector3 position, Vector3 normal, float radius, float size);
		static void internal_DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size);
		static void internal_DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size);
		static void internal_DrawRect(Rect3 area, float size);
	};
}