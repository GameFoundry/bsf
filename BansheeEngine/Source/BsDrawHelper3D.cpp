//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDrawHelper3D.h"
#include "BsRectF.h"
#include "BsMesh.h"
#include "BsTime.h"
#include "BsVector2.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsCoreApplication.h"
#include "BsRenderQueue.h"
#include "BsException.h"
#include "BsCamera.h"
#include "BsBuiltinMaterialManager.h"
#include "BsVertexDataDesc.h"

namespace BansheeEngine
{
	DrawHelper3D::DrawHelper3D()
	{
		mVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mVertexDesc->addVertElem(VET_FLOAT2, VES_POSITION);
		mVertexDesc->addVertElem(VET_COLOR, VES_COLOR);
	}

	void DrawHelper3D::aabox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);

		assert((vertexOffset + 8) <= meshData->getNumVertices());
		assert((indexOffset + 36) <= meshData->getNumIndices());	

		aabox(box, positionData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper3D::line_Pixel(const Vector3& a, const Vector3& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::line_Pixel(a, b, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::line_AA(const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::line_AA(a, b, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::lineList_Pixel(const Vector<Vector3>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::lineList_Pixel(linePoints, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper3D::lineList_AA(const Vector<Vector3>& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector3>::lineList_AA(linePoints, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	/************************************************************************/
	/* 								DRAW	                     			*/
	/************************************************************************/

	void DrawHelper3D::drawLine_Pixel(const HCamera& camera, const Vector3& a, const Vector3& b, const Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(2, 2, mVertexDesc);

		line_Pixel(a, b, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), mVertexDesc->getVertexStride());

		HMesh mesh = Mesh::create(meshData, MeshBufferType::Static, DOT_LINE_LIST);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.matInfo3D = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLine_AA(const HCamera& camera, const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(8, 30, mVertexDesc);

		line_AA(a, b, width, borderWidth, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), mVertexDesc->getVertexStride());

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.matInfo3D = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLineList_Pixel(const HCamera& camera, const Vector<Vector3>& linePoints, const Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(
			(UINT32)(linePoints.size() * 2), (UINT32)(linePoints.size() * 2), mVertexDesc);

		lineList_Pixel(linePoints, color, meshData, 0, 0);

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), mVertexDesc->getVertexStride());

		HMesh mesh = Mesh::create(meshData, MeshBufferType::Static, DOT_LINE_LIST);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.matInfo3D = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawLineList_AA(const HCamera& camera, const Vector<Vector3>& linePoints, float width, float borderWidth, 
		const Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>((UINT32)(linePoints.size() * 4), (UINT32)(linePoints.size() * 15), mVertexDesc);

		lineList_AA(linePoints, width, borderWidth, color, meshData, 0, 0);	

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), mVertexDesc->getVertexStride());

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.matInfo3D = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::drawAABox(const HCamera& camera, const AABox& box, const Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(8, 36, mVertexDesc);

		aabox(box, meshData, 0, 0);	

		UINT32 vertexStride = mVertexDesc->getVertexStride();
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		for(UINT32 i = 0; i < meshData->getNumVertices(); i++)
		{
			UINT32* colors = (UINT32*)colorData;
			(*colors) = color.getAsRGBA();

			colorData += vertexStride;
		}

		UINT8* positionData = meshData->getElementData(VES_POSITION);
		dbgCmd.worldCenter = calcCenter(positionData, meshData->getNumVertices(), mVertexDesc->getVertexStride());

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.type = DebugDrawType::WorldSpace;
		dbgCmd.matInfo3D = BuiltinMaterialManager::instance().createDebugDraw3DMaterial();
	}

	void DrawHelper3D::aabox(const AABox& box, UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		outVertices += (vertexOffset * vertexStride);

		Vector3 pt;

		pt = box.getCorner(AABox::FAR_LEFT_BOTTOM);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::FAR_RIGHT_BOTTOM);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::FAR_LEFT_TOP);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::FAR_RIGHT_TOP);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::NEAR_LEFT_BOTTOM);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::NEAR_RIGHT_BOTTOM);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::NEAR_LEFT_TOP);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		pt = box.getCorner(AABox::NEAR_RIGHT_TOP);
		memcpy(outVertices, &pt, sizeof(pt));
		outVertices += vertexStride;

		outIndices += indexOffset;

		// Front
		outIndices[0] = vertexOffset + 6;
		outIndices[1] = vertexOffset + 7;
		outIndices[2] = vertexOffset + 5;

		outIndices[3] = vertexOffset + 6;
		outIndices[4] = vertexOffset + 5;
		outIndices[5] = vertexOffset + 4;

		// Back
		outIndices[6] = vertexOffset + 2;
		outIndices[7] = vertexOffset + 1;
		outIndices[8] = vertexOffset + 3;

		outIndices[9] = vertexOffset + 2;
		outIndices[10] = vertexOffset + 0;
		outIndices[11] = vertexOffset + 1;

		// Left
		outIndices[12] = vertexOffset + 2;
		outIndices[13] = vertexOffset + 6;
		outIndices[14] = vertexOffset + 4;

		outIndices[15] = vertexOffset + 2;
		outIndices[16] = vertexOffset + 4;
		outIndices[17] = vertexOffset + 0;

		// Right
		outIndices[18] = vertexOffset + 7;
		outIndices[19] = vertexOffset + 3;
		outIndices[20] = vertexOffset + 1;

		outIndices[21] = vertexOffset + 7;
		outIndices[22] = vertexOffset + 1;
		outIndices[23] = vertexOffset + 5;

		// Top
		outIndices[24] = vertexOffset + 6;
		outIndices[25] = vertexOffset + 2;
		outIndices[26] = vertexOffset + 3;

		outIndices[27] = vertexOffset + 6;
		outIndices[28] = vertexOffset + 3;
		outIndices[29] = vertexOffset + 7;

		// Bottom
		outIndices[30] = vertexOffset + 5;
		outIndices[31] = vertexOffset + 1;
		outIndices[32] = vertexOffset + 0;

		outIndices[33] = vertexOffset + 5;
		outIndices[34] = vertexOffset + 0;
		outIndices[35] = vertexOffset + 4;
	}

	Vector3 DrawHelper3D::calcCenter(UINT8* vertices, UINT32 numVertices, UINT32 vertexStride)
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

	void DrawHelper3D::line_AA(const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		BS_EXCEPT(NotImplementedException, "3D AA line drawing not implemented.");
	}

	void DrawHelper3D::polygon_AA(const Vector<Vector3>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		BS_EXCEPT(NotImplementedException, "3D AA polygon drawing not implemented.");
	}
}