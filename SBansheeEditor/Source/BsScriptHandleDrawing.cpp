#include "BsScriptHandleDrawing.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsScriptSpriteTexture.h"
#include "BsSpriteTexture.h"
#include "BsHandleManager.h"
#include "BsHandleDrawManager.h"

namespace BansheeEngine
{
	void ScriptHandleDrawing::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_SetColor", &ScriptHandleDrawing::internal_SetColor);
		metaData.scriptClass->addInternalCall("Internal_SetTransform", &ScriptHandleDrawing::internal_SetTransform);
		metaData.scriptClass->addInternalCall("Internal_DrawCube", &ScriptHandleDrawing::internal_DrawCube);
		metaData.scriptClass->addInternalCall("Internal_DrawSphere", &ScriptHandleDrawing::internal_DrawSphere);
		metaData.scriptClass->addInternalCall("Internal_DrawWireCube", &ScriptHandleDrawing::internal_DrawWireCube);
		metaData.scriptClass->addInternalCall("Internal_DrawWireSphere", &ScriptHandleDrawing::internal_DrawWireSphere);
		metaData.scriptClass->addInternalCall("Internal_DrawLine", &ScriptHandleDrawing::internal_DrawLine);
		metaData.scriptClass->addInternalCall("Internal_DrawCone", &ScriptHandleDrawing::internal_DrawCone);
		metaData.scriptClass->addInternalCall("Internal_DrawDisc", &ScriptHandleDrawing::internal_DrawDisc);
		metaData.scriptClass->addInternalCall("Internal_DrawWireDisc", &ScriptHandleDrawing::internal_DrawWireDisc);
		metaData.scriptClass->addInternalCall("Internal_DrawArc", &ScriptHandleDrawing::internal_DrawArc);
		metaData.scriptClass->addInternalCall("Internal_DrawWireArc", &ScriptHandleDrawing::internal_DrawWireArc);
		metaData.scriptClass->addInternalCall("Internal_DrawRect", &ScriptHandleDrawing::internal_DrawRect);
	}

	void ScriptHandleDrawing::internal_SetColor(Color color)
	{
		HandleManager::instance().getDrawManager().setColor(color);
	}

	void ScriptHandleDrawing::internal_SetTransform(Matrix4 transform)
	{
		HandleManager::instance().getDrawManager().setTransform(transform);
	}

	void ScriptHandleDrawing::internal_DrawCube(Vector3 position, Vector3 extents)
	{
		HandleManager::instance().getDrawManager().drawCube(position, extents);
	}

	void ScriptHandleDrawing::internal_DrawSphere(Vector3 position, float radius)
	{
		HandleManager::instance().getDrawManager().drawSphere(position, radius);
	}

	void ScriptHandleDrawing::internal_DrawWireCube(Vector3 position, Vector3 extents)
	{
		HandleManager::instance().getDrawManager().drawWireCube(position, extents);
	}

	void ScriptHandleDrawing::internal_DrawWireSphere(Vector3 position, float radius)
	{
		HandleManager::instance().getDrawManager().drawWireSphere(position, radius);
	}

	void ScriptHandleDrawing::internal_DrawLine(Vector3 start, Vector3 end)
	{
		HandleManager::instance().getDrawManager().drawLine(start, end);
	}

	void ScriptHandleDrawing::internal_DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius)
	{
		HandleManager::instance().getDrawManager().drawCone(coneBase, normal, height, radius);
	}

	void ScriptHandleDrawing::internal_DrawDisc(Vector3 position, Vector3 normal, float radius)
	{
		HandleManager::instance().getDrawManager().drawDisc(position, normal, radius);
	}

	void ScriptHandleDrawing::internal_DrawWireDisc(Vector3 position, Vector3 normal, float radius)
	{
		HandleManager::instance().getDrawManager().drawWireDisc(position, normal, radius);
	}

	void ScriptHandleDrawing::internal_DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle)
	{
		HandleManager::instance().getDrawManager().drawArc(position, normal, radius, startAngle, amountAngle);
	}

	void ScriptHandleDrawing::internal_DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle)
	{
		HandleManager::instance().getDrawManager().drawWireArc(position, normal, radius, startAngle, amountAngle);
	}

	void ScriptHandleDrawing::internal_DrawRect(Rect3 area)
	{
		HandleManager::instance().getDrawManager().drawRect(area);
	}
}