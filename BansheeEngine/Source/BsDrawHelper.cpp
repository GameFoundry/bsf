#include "BsDrawHelper.h"
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
	DrawHelper::DrawHelper()
	{
		mMaterial2DClipSpace = BuiltinMaterialManager::instance().createDebugDraw2DClipSpaceMaterial();
	}

	void DrawHelper::quad2D(const CM::FRect& area, const MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
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
		
		polygonFill2D_Pixel(points, positionData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper::line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		outVertices += (vertexOffset * vertexStride);
		outColors += (vertexOffset * vertexStride);

		Vector2* vertices = (Vector2*)outVertices;
		(*vertices) = a;

		vertices = (Vector2*)(outVertices + vertexStride);
		(*vertices) = b;

		UINT32* colors = (UINT32*)outColors;
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(outColors + vertexStride);
		(*colors) = color.getAsRGBA();

		outIndices += indexOffset;
		outIndices[0] = vertexOffset + 0;
		outIndices[1] = vertexOffset + 1;
	}

	void DrawHelper::line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, const MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		assert((vertexOffset + 2) <= meshData->getNumVertices());
		assert((indexOffset + 2) <= meshData->getNumIndices());

		line2D_Pixel(a, b, color, positionData, colorData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper::line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		Vector2 dir = b - a;
		dir.normalize();

		Vector2 nrm(dir.y, -dir.x);

		Vector<Vector2>::type points(4);

		float r = width - 1.0f;
		r *= 0.5f;
		if (r < 0.01f) 
			r = 0.01f;

		dir = dir * r;
		nrm = nrm * r;

		Vector2 v0 = a - dir - nrm;
		Vector2 v1 = a - dir + nrm;
		Vector2 v2 = b + dir + nrm;
		Vector2 v3 = b + dir - nrm;

		points[0] = v0;
		points[1] = v1;
		points[2] = v2;
		points[3] = v3;

		polygon2D_AA(points, borderWidth, color, outVertices, outColors, vertexOffset, vertexStride, outIndices, indexOffset);
	}

	void DrawHelper::line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, const MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		assert((vertexOffset + 8) <= meshData->getNumVertices());
		assert((indexOffset + 30) <= meshData->getNumIndices());

		line2D_AA(a, b, width, borderWidth, color, positionData, colorData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper::polygon2D_AA(const Vector<Vector2>::type& points, float borderWidth, const CM::Color& color, 
		UINT8* outVertices, CM::UINT8* outColors, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		UINT32 numCoords = (UINT32)points.size();

		outVertices += vertexOffset * vertexStride;
		Vector<Vector2>::type tempNormals(numCoords);

		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			const Vector2& v0 = points[j];
			const Vector2& v1 = points[i];

			Vector2 d = v1 - v0;
			d.normalize();

			// Rotate by 90 degrees
			std::swap(d.x, d.y);
			d.y = -d.y;

			tempNormals[j] = d;

			// Also start populating the vertex array
			Vector2* vertices = (Vector2*)outVertices;
			*vertices = v1;

			UINT32* colors = (UINT32*)outColors;
			*colors = color.getAsRGBA();

			outVertices += vertexStride;
			outColors += vertexStride;
		}

		Color transparentColor = color;
		transparentColor.a = 0.0f;

		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			const Vector2& n0 = tempNormals[j];
			const Vector2& n1 = tempNormals[i];

			Vector2 avgNrm = (n0 + n1) * 0.5f;
			float magSqrd = avgNrm.squaredLength();

			if (magSqrd > 0.000001f)
			{
				float scale = 1.0f / magSqrd;
				if (scale > 10.0f) 
					scale = 10.0f;

				avgNrm.x *= scale;
				avgNrm.y *= scale;
			}

			Vector2 tempCoord = points[i] + avgNrm * borderWidth;

			// Move it to the vertex array
			Vector2* vertices = (Vector2*)outVertices;
			*vertices = tempCoord;

			UINT32* colors = (UINT32*)outColors;
			*colors = transparentColor.getAsRGBA();

			outVertices += vertexStride;
			outColors += vertexStride;
		}

		// Populate index buffer
		outIndices += indexOffset;

		UINT32 idxCnt = 0;
		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			outIndices[idxCnt++] = i;
			outIndices[idxCnt++] = j;
			outIndices[idxCnt++] = numCoords + j;

			outIndices[idxCnt++] = numCoords + j;
			outIndices[idxCnt++] = numCoords + i;
			outIndices[idxCnt++] = i;
		}
		
		for(UINT32 i = 2; i < numCoords; ++i)
		{
			outIndices[idxCnt++] = 0;
			outIndices[idxCnt++] = i - 1;
			outIndices[idxCnt++] = i;
		}
	}

	void DrawHelper::polygonFill2D_Pixel(const CM::Vector<CM::Vector2>::type& points, CM::UINT8* outVertices, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		outVertices += (vertexOffset * vertexStride);

		for(auto& point : points)
		{
			Vector2* vertices = (Vector2*)outVertices;
			(*vertices) = Vector2(point.x, point.y);

			outVertices += vertexStride;
		}

		outIndices += indexOffset;
		INT32 numPoints = (INT32)points.size();
		UINT32 idxCnt = 0;
		for(int i = 2; i < numPoints; i++)
		{
			outIndices[idxCnt++] = vertexOffset;
			outIndices[idxCnt++] = vertexOffset + i - 1;
			outIndices[idxCnt++] = vertexOffset + i;
		}
	}

	void DrawHelper::polygonBorder2D_Pixel(const CM::Vector<CM::Vector2>::type& points, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
		CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
	{
		INT32 numPoints = (INT32)points.size();
		UINT32 curVertOffset = vertexOffset;
		UINT32 curIdxOffset = indexOffset;
		for(INT32 i = 0, j = numPoints - 1; i < numPoints; j = i++)
		{
			line2D_Pixel(points[j], points[i], color, outVertices, outColors, curVertOffset, vertexStride, outIndices, curIdxOffset);
			curVertOffset += 2;
			curIdxOffset += 2;
		}
	}

	void DrawHelper::drawQuad2D(const HCamera& camera, const CM::FRect& area, const Color& color, CoordType coordType, float timeout)
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
		if(coordType == CoordType::Normalized)
			actualArea = normalizedCoordToClipSpace(area);

		quad2D(actualArea, meshData, 0, 0);

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

		if(coordType == CoordType::Normalized)
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

	void DrawHelper::drawLine2D_Pixel(const HCamera& camera, const Vector2& a, const Vector2& b, const Color& color, CoordType coordType, float timeout)
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
		if(coordType == CoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line2D_Pixel(actualA, actualB, color, meshData, 0, 0);

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == CoordType::Normalized)
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

	void DrawHelper::drawLine2D_AA(const HCamera& camera, const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, CoordType coordType, float timeout)
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
		if(coordType == CoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line2D_AA(actualA, actualB, width, borderWidth, color, meshData, 0, 0);

		HMesh mesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == CoordType::Normalized)
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

	void DrawHelper::render(const HCamera& camera, CM::RenderQueue& renderQueue)
	{
		const Viewport* viewport = camera->getViewport().get();
		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();
		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		float invViewportWidth = 1.0f / (viewport->getWidth() * 0.5f);
		float invViewportHeight = 1.0f / (viewport->getHeight() * 0.5f);

		for(auto& cmd : commands)
		{
			if(cmd.mesh == nullptr || !cmd.mesh.isLoaded() || !cmd.mesh->isInitialized())
				continue;

			if(cmd.material == nullptr || !cmd.material.isLoaded() || !cmd.material->isInitialized())
				continue;

			if(cmd.type == DebugDrawType::ClipSpace)
			{
				renderQueue.add(cmd.material, cmd.mesh->getSubMeshData(), cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::ScreenSpace)
			{
				cmd.material->setFloat("invViewportWidth", invViewportWidth);
				cmd.material->setFloat("invViewportHeight", invViewportHeight);

				renderQueue.add(cmd.material, cmd.mesh->getSubMeshData(), cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::WorldSpace)
			{
				cmd.material->setMat4("matViewProj", viewProjMatrix);

				renderQueue.add(cmd.material, cmd.mesh->getSubMeshData(), cmd.worldCenter);
			}
		}

		float curTime = gTime().getTime();
		Vector<DebugDrawCommand>::type newCommands;
		for(auto& cmd : commands)
		{
			if(cmd.endTime > curTime)
				newCommands.push_back(cmd);
		}

		commands.swap(newCommands);
	}

	FRect DrawHelper::normalizedCoordToClipSpace(const FRect& area) const
	{
		FRect clipSpaceRect;
		clipSpaceRect.x = area.x * 2.0f - 1.0f;
		clipSpaceRect.width = area.width * 2.0f;
		clipSpaceRect.y = -area.y * 2.0f + 1.0f;
		clipSpaceRect.height = area.height * -2.0f;

		return clipSpaceRect;
	}

	Vector2 DrawHelper::normalizedCoordToClipSpace(const Vector2& pos) const
	{
		return Vector2(pos.x * 2.0f - 1.0f, -pos.y * 2.0f + 1.0f);
	}
}