#include "BsDrawHelper3D.h"
#include "CmFRect.h"
#include "CmMesh.h"
#include "CmTime.h"
#include "CmVector2.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmApplication.h"
#include "CmRenderQueue.h"
#include "CmException.h"
#include "BsCamera.h"
#include "BsBuiltinMaterialManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void DrawHelper3D::line_Pixel(const Vector3& a, const Vector3& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::line_Pixel(a, b, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::line_AA(const Vector3& a, const Vector3& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::line_AA(a, b, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::lineList_Pixel(const CM::Vector<Vector3>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::lineList_Pixel(linePoints, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::lineList_AA(const CM::Vector<Vector3>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::lineList_AA(linePoints, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	/************************************************************************/
	/* 								DRAW	                     			*/
	/************************************************************************/

	void DrawHelper3D::drawLine_Pixel(const HCamera& camera, const Vector3& a, const Vector3& b, const Color& color, float timeout)
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

		line_Pixel(a, b, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), meshData->getVertexStride());

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLine_AA(const HCamera& camera, const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, float timeout)
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

		line_AA(a, b, width, borderWidth, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), meshData->getVertexStride());

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLineList_Pixel(const HCamera& camera, const Vector<Vector3>::type& linePoints, const Color& color, float timeout)
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

		lineList_Pixel(linePoints, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), meshData->getVertexStride());

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLineList_AA(const HCamera& camera, const CM::Vector<CM::Vector3>::type& linePoints, float width, float borderWidth, 
		const CM::Color& color, float timeout)
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

		lineList_AA(linePoints, width, borderWidth, color, meshData, 0, 0);	

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), meshData->getVertexStride());

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.material = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	CM::Vector3 DrawHelper3D::calcCenter(UINT8* vertices, UINT32 numVertices, UINT32 vertexStride)
	{
		Vector3 center = Vector3::ZERO;
		for(UINT32 i = 0; i < numVertices; i++)
		{
			Vector3* curVert = (Vector3*)vertices;
			center += *curVert;

			vertices += vertexStride;
		}

		center /= (float)numVertices;
		return center;
	}

	void DrawHelper3D::line_AA(const CM::Vector3& a, const CM::Vector3& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		CM_EXCEPT(NotImplementedException, "3D AA line drawing not implemented.");
	}

	void DrawHelper3D::polygon_AA(const CM::Vector<Vector3>::type& points, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		CM_EXCEPT(NotImplementedException, "3D AA polygon drawing not implemented.");
	}
}