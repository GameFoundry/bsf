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
		UINT32* indices = cm_newN<UINT32, ScratchAlloc>(2);
		UINT8* vertices = (UINT8*)cm_alloc<ScratchAlloc>(2 * VertexSize);

		DebugDrawCommand command;
		command.indices = cm_newN<UINT32, ScratchAlloc>(2);
		command.vertices = (UINT8*)cm_alloc<ScratchAlloc>(2 * VertexSize);
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
		MeshDataPtr lineMeshData = cm_shared_ptr<MeshData, ScratchAlloc>(totalNumLineVertices);
		MeshDataPtr triangleMeshData = cm_shared_ptr<MeshData, ScratchAlloc>(totalNumTriangleVertices);

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
		Vector<DebugDrawCommand>::type newCommands;
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

				cm_deleteN<ScratchAlloc>(command.indices, numIndices);
				cm_free<ScratchAlloc>(command.vertices);
			}
		}

		gMainSyncedCA().writeSubresource(mLineMesh.getInternalPtr(), 0, *lineMeshData);
		gMainSyncedCA().writeSubresource(mTriangleMesh.getInternalPtr(), 0, *triangleMeshData);
		gMainSyncedCA().submitToCoreThread(true);
	}

	void DebugDraw::render(const Camera* camera, CoreAccessor& coreAccessor)
	{
		if(mCommands.size() == 0)
			return;

		updateMeshes();

		for(UINT32 i = 0; i < mLineMaterial->getNumPasses(); i++)
		{
			PassPtr pass = mLineMaterial->getPass(i);
			pass->activate(coreAccessor);

			PassParametersPtr paramsPtr = mLineMaterial->getPassParameters(i);
			pass->bindParameters(coreAccessor, paramsPtr);

			coreAccessor.render(mLineMesh->getRenderOperation());
		}

		for(UINT32 i = 0; i < mTriangleMaterial->getNumPasses(); i++)
		{
			PassPtr pass = mTriangleMaterial->getPass(i);
			pass->activate(coreAccessor);

			PassParametersPtr paramsPtr = mTriangleMaterial->getPassParameters(i);
			pass->bindParameters(coreAccessor, paramsPtr);

			coreAccessor.render(mTriangleMesh->getRenderOperation());
		}
	}

	//void DebugDraw::drawAABB(const AxisAlignedBox& aab, const Color& color, float timeout)
	//{

	//}

	//void DebugDraw::drawSphere(const Vector3& center, float radius, const Color& color, float timeout)
	//{

	//}
}