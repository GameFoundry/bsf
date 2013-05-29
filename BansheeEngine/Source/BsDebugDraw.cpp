#include "BsDebugDraw.h"
#include "CmMesh.h"
#include "CmTime.h"
#include "CmVector2.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmApplication.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	DebugDraw::DebugDraw()
	{
		// TODO - Create a fixed sized mesh and prevent updating the mesh completely every frame
		mTriangleMesh = Mesh::create();
		mLineMesh = Mesh::create();

		// TODO - Init materials
		//HMaterial mTriangleMaterial;
		//HMaterial mLineMaterial;
	}

	void DebugDraw::draw2DLine(const Vector2&a, const Vector2& b, const Color& color, float timeout)
	{
		UINT32* indices = CM_NEW_ARRAY(UINT32, 2, ScratchAlloc);
		UINT8* vertices = CM_NEW_BYTES(2 * VertexSize, ScratchAlloc);

		DebugDrawCommand command;
		command.indices = CM_NEW_ARRAY(UINT32, 2, ScratchAlloc);
		command.vertices = CM_NEW_BYTES(2 * VertexSize, ScratchAlloc);
		command.numElements = 1;
		command.type = DebugDrawType::Line;
		command.timeEnds = gTime().getTime() + timeout;

		command.indices[0] = 0;
		command.indices[1] = 1;

		UINT8* vertDst = command.vertices;
		RGBA color32 = color.getAsRGBA();

		Vector3 pointA(a.x, a.y, 0.0f);
		memcpy(vertDst, &pointA, sizeof(pointA));
		vertDst += sizeof(pointA);

		memcpy(vertDst, &color32, sizeof(color32));
		vertDst += sizeof(color32);

		Vector3 pointB(b.x, b.y, 0.0f);
		memcpy(vertDst, &pointB, sizeof(pointB));
		vertDst += sizeof(pointB);

		memcpy(vertDst, &color32, sizeof(color32));
		vertDst += sizeof(color32);

		mCommands.push_back(command);
	}

	void DebugDraw::updateMeshes()
	{
		if(mCommands.size() == 0)
			return;

		UINT32 totalNumLineIndices = 0;
		UINT32 totalNumLineVertices = 0;
		UINT32 totalNumTriangleIndices = 0;
		UINT32 totalNumTriangleVertices = 0;

		for(auto& command : mCommands)
		{
			if(command.type == DebugDrawType::Line)
			{
				totalNumLineIndices += command.numElements * 2;
				totalNumLineVertices += command.numElements * 2;
			}
			else if(command.type == DebugDrawType::Triangle)
			{
				totalNumTriangleIndices += command.numElements * 3;
				totalNumTriangleVertices += command.numElements * 3;
			}
		}

		// TODO - Somehow avoid creating these every frame?
		MeshDataPtr lineMeshData(CM_NEW(MeshData, ScratchAlloc) MeshData(totalNumLineVertices), &MemAllocDeleter<MeshData, ScratchAlloc>::deleter);
		MeshDataPtr triangleMeshData(CM_NEW(MeshData, ScratchAlloc) MeshData(totalNumTriangleVertices), &MemAllocDeleter<MeshData, ScratchAlloc>::deleter);

		lineMeshData->beginDesc();

		lineMeshData->addSubMesh(totalNumLineIndices);
		lineMeshData->addVertElem(VET_FLOAT3, VES_POSITION);
		lineMeshData->addVertElem(VET_COLOR, VES_COLOR);

		lineMeshData->endDesc();

		triangleMeshData->beginDesc();

		triangleMeshData->addSubMesh(totalNumTriangleIndices);
		triangleMeshData->addVertElem(VET_FLOAT3, VES_POSITION);
		triangleMeshData->addVertElem(VET_COLOR, VES_COLOR);

		triangleMeshData->endDesc();

		UINT32 lineIndexOffset = 0;
		UINT32 lineVertexOffset = 0;
		UINT32 triangleIndexOffset = 0;
		UINT32 triangleVertexOffset = 0;
		vector<DebugDrawCommand>::type newCommands;
		for(auto& command : mCommands)
		{
			if(command.type == DebugDrawType::Line)
			{
				UINT32 numIndices = command.numElements * 2;
				UINT32 numVertices = command.numElements * 2;

				UINT32* indexData = lineMeshData->getIndices32() + lineIndexOffset;
				memcpy(indexData, command.indices, numIndices * sizeof(UINT32));

				UINT8* vertexData = lineMeshData->getElementData(VES_POSITION) + lineVertexOffset * VertexSize;
				memcpy(vertexData, command.vertices, numVertices * VertexSize);

				lineIndexOffset += numIndices;
				lineVertexOffset += numVertices;
			}
			else if(command.type == DebugDrawType::Triangle)
			{
				UINT32 numIndices = command.numElements * 3;
				UINT32 numVertices = command.numElements * 3;

				UINT32* indexData = triangleMeshData->getIndices32() + triangleIndexOffset;
				memcpy(indexData, command.indices, numIndices * sizeof(UINT32));

				UINT8* vertexData = triangleMeshData->getElementData(VES_POSITION) + triangleVertexOffset * VertexSize;
				memcpy(vertexData, command.vertices, numVertices * VertexSize);

				triangleIndexOffset += numIndices;
				triangleVertexOffset += numVertices;
			}

			if(command.timeEnds < gTime().getTime())
				newCommands.push_back(command);
			else
			{
				UINT32 numIndices = 0;
				if(command.type == DebugDrawType::Line)
				{
					numIndices = command.numElements * 2;
				}
				else if(command.type == DebugDrawType::Triangle)
				{
					numIndices = command.numElements * 3;
				}

				CM_DELETE_ARRAY(command.indices, UINT32, numIndices, ScratchAlloc);
				CM_DELETE_BYTES(command.vertices, ScratchAlloc);
			}
		}

		gMainSyncedRC().writeSubresource(mLineMesh.getInternalPtr(), 0, *lineMeshData);
		gMainSyncedRC().writeSubresource(mTriangleMesh.getInternalPtr(), 0, *triangleMeshData);
		gMainSyncedRC().submitToGpu(true);
	}

	void DebugDraw::render(const Camera* camera, RenderContext& renderContext)
	{
		if(mCommands.size() == 0)
			return;

		updateMeshes();

		for(UINT32 i = 0; i < mLineMaterial->getNumPasses(); i++)
		{
			PassPtr pass = mLineMaterial->getPass(i);
			pass->activate(renderContext);

			PassParametersPtr paramsPtr = mLineMaterial->getPassParameters(i);
			pass->bindParameters(renderContext, paramsPtr);

			renderContext.render(mLineMesh->getRenderOperation());
		}

		for(UINT32 i = 0; i < mTriangleMaterial->getNumPasses(); i++)
		{
			PassPtr pass = mTriangleMaterial->getPass(i);
			pass->activate(renderContext);

			PassParametersPtr paramsPtr = mTriangleMaterial->getPassParameters(i);
			pass->bindParameters(renderContext, paramsPtr);

			renderContext.render(mTriangleMesh->getRenderOperation());
		}
	}

	//void DebugDraw::drawAABB(const AxisAlignedBox& aab, const Color& color, float timeout)
	//{

	//}

	//void DebugDraw::drawSphere(const Vector3& center, float radius, const Color& color, float timeout)
	//{

	//}
}