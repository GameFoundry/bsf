#include "BsDebugDraw.h"
#include "CmMesh.h"
#include "CmTime.h"
#include "CmVector2.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmApplication.h"
#include "CmRenderQueue.h"
#include "BsBuiltinMaterialManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	DebugDraw::DebugDraw()
	{
		mMaterial = BuiltinMaterialManager::instance().createDebugDrawMaterial();
	}

	void DebugDraw::quad2D(const Vector2& pos, const Vector2& size, UINT8* outVertices, UINT8* outColors, 
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, const Color& color)
	{
		outVertices += vertexOffset;
		outColors += vertexOffset;

		Vector3* vertices = (Vector3*)outVertices;
		(*vertices) = Vector3(pos.x, pos.y, 0.0f);

		vertices = (Vector3*)(outVertices + vertexStride);
		(*vertices) = Vector3(pos.x + size.x, pos.y, 0.0f);

		vertices = (Vector3*)(outVertices + vertexStride * 2);
		(*vertices) = Vector3(pos.x, pos.y + size.y, 0.0f);

		vertices = (Vector3*)(outVertices + vertexStride * 3);
		(*vertices) = Vector3(pos.x + size.x, pos.y + size.y, 0.0f);

		UINT32* colors = (UINT32*)outColors;
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(outColors + vertexStride);
		(*colors) = color.getAsRGBA();
		
		colors = (UINT32*)(outColors + vertexStride * 2);
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(outColors + vertexStride * 3);
		(*colors) = color.getAsRGBA();

		outIndices += indexOffset;
		outIndices[0] = vertexOffset + 0;
		outIndices[1] = vertexOffset + 2;
		outIndices[2] = vertexOffset + 1;
		outIndices[3] = vertexOffset + 1;
		outIndices[4] = vertexOffset + 2;
		outIndices[5] = vertexOffset + 3;
	}

	void DebugDraw::drawQuad2D(const Vector2& pos, const Vector2& size, const CM::Color& color, float timeout)
	{
		// TODO - DONT USE ONE MESH PER DRAW - Instead merge multiple elements into a single mesh

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(4);

		meshData->beginDesc();

		meshData->addSubMesh(6);
		meshData->addVertElem(VET_FLOAT3, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		quad2D(pos, size, positionData, colorData, 0, meshData->getVertexStride(), indexData, 0, color);

		HMesh mesh = Mesh::create();
		gMainSyncedCA().writeSubresource(mesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);

		// TODO - Timeout is ignored!
		mMeshes.push_back(mesh);
	}

	void DebugDraw::render(CM::RenderQueue& renderQueue)
	{
		if(mMaterial == nullptr || !mMaterial.isLoaded())
			return;

		for(auto& mesh : mMeshes)
		{
			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			// TODO - I'm not setting a world position
			renderQueue.add(mMaterial, mesh->getSubMeshData(), Vector3::ZERO);
		}
	}
}