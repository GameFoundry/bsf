#include "BsDrawHelper2D.h"
#include "CmFRect.h"
#include "CmMesh.h"
#include "CmTime.h"
#include "CmVector2.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmApplication.h"
#include "CmRenderQueue.h"
#include "BsCamera.h"
#include "BsBuiltinMaterialManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	DrawHelper2D::DrawHelper2D()
	{
		mMaterial2DClipSpace = BuiltinMaterialManager::instance().createDebugDraw2DClipSpaceMaterial();
	}

	void DrawHelper2D::quad(const CM::FRect& area, const MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);

		assert((vertexOffset + 4) <= meshData->getNumVertices());
		assert((indexOffset + 6) <= meshData->getNumIndices());

		Vector<Vector2>::type points;
		points.push_back(Vector2(area.x, area.y));
		points.push_back(Vector2(area.x + area.width, area.y));
		points.push_back(Vector2(area.x + area.width, area.y + area.height));
		points.push_back(Vector2(area.x, area.y + area.height));	

		polygonFill_Pixel(points, positionData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper2D::line_Pixel(const Vector2& a, const Vector2& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::line_Pixel(a, b, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::line_AA(a, b, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::lineList_Pixel(const CM::Vector<Vector2>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::lineList_Pixel(linePoints, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::lineList_AA(const CM::Vector<Vector2>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::lineList_AA(linePoints, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	/************************************************************************/
	/* 								2D - DRAW	                     		*/
	/************************************************************************/

	void DrawHelper2D::drawQuad(const HCamera& camera, const CM::FRect& area, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(4);

		meshData->beginDesc();

		meshData->addSubMesh(6);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		FRect actualArea = area;
		if(coordType == DebugDrawCoordType::Normalized)
			actualArea = normalizedCoordToClipSpace(area);

		quad(actualArea, meshData, 0, 0);

		UINT32 vertexStride = meshData->getVertexStride();
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		UINT32* colors = (UINT32*)colorData;
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride);
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride * 2);
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride * 3);
		(*colors) = color.getAsRGBA();

		HMesh mesh = Mesh::create();
		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.material = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLine_Pixel(const HCamera& camera, const Vector2& a, const Vector2& b, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(2);

		meshData->beginDesc();

		meshData->addSubMesh(2, 0, DOT_LINE_LIST);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		Vector2 actualA = a;
		Vector2 actualB = b;
		if(coordType == DebugDrawCoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line_Pixel(actualA, actualB, color, meshData, 0, 0);

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.material = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLine_AA(const HCamera& camera, const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(8);

		meshData->beginDesc();

		meshData->addSubMesh(30, 0, DOT_TRIANGLE_LIST);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		Vector2 actualA = a;
		Vector2 actualB = b;
		if(coordType == DebugDrawCoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line_AA(actualA, actualB, width, borderWidth, color, meshData, 0, 0);

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.material = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLineList_Pixel(const HCamera& camera, const Vector<Vector2>::type& linePoints, const Color& color, 
		DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>((UINT32)(linePoints.size() * 2));

		meshData->beginDesc();

		meshData->addSubMesh((UINT32)(linePoints.size() * 2), 0, DOT_LINE_LIST);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		if(coordType == DebugDrawCoordType::Normalized)
		{
			Vector<Vector2>::type points;
			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i++)
				points.push_back(normalizedCoordToClipSpace(linePoints[i]));

			lineList_Pixel(points, color, meshData, 0, 0);
		}
		else
		{
			lineList_Pixel(linePoints, color, meshData, 0, 0);
		}		

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.material = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLineList_AA(const HCamera& camera, const CM::Vector<CM::Vector2>::type& linePoints, float width, float borderWidth, 
		const CM::Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>((UINT32)(linePoints.size() * 4));

		meshData->beginDesc();

		meshData->addSubMesh((UINT32)(linePoints.size() * 15), 0, DOT_TRIANGLE_LIST);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		if(coordType == DebugDrawCoordType::Normalized)
		{
			Vector<Vector2>::type points;
			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i++)
				points.push_back(normalizedCoordToClipSpace(linePoints[i]));

			lineList_AA(points, width, borderWidth, color, meshData, 0, 0);
		}
		else
		{
			lineList_AA(linePoints, width, borderWidth, color, meshData, 0, 0);
		}		

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.material = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	FRect DrawHelper2D::normalizedCoordToClipSpace(const FRect& area) const
	{
		FRect clipSpaceRect;
		clipSpaceRect.x = area.x * 2.0f - 1.0f;
		clipSpaceRect.width = area.width * 2.0f;
		clipSpaceRect.y = -area.y * 2.0f + 1.0f;
		clipSpaceRect.height = area.height * -2.0f;

		return clipSpaceRect;
	}

	Vector2 DrawHelper2D::normalizedCoordToClipSpace(const Vector2& pos) const
	{
		return Vector2(pos.x * 2.0f - 1.0f, -pos.y * 2.0f + 1.0f);
	}
}