#include "BsDebugDraw.h"
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
	DebugDraw::DebugDraw()
	{
		mMaterial2D = BuiltinMaterialManager::instance().createDebugDraw2DMaterial();
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

	void DebugDraw::drawQuad2D(const HCamera& camera, const Vector2& pos, const Vector2& size, const CM::Color& color, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.type = DebugDrawType::ScreenSpace;
		dbgCmd.endTime = gTime().getTime() + timeout;

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

		dbgCmd.mesh = mesh;
		dbgCmd.material = mMaterial2D;
		dbgCmd.worldCenter = Vector3::ZERO;
	}

	void DebugDraw::render(const HCamera& camera, CM::RenderQueue& renderQueue)
	{
		const Viewport* viewport = camera->getViewport().get();
		Vector<DebugDrawCommand>::type& commands = mCommandsPerViewport[viewport];

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();
		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		for(auto& cmd : commands)
		{
			if(cmd.mesh == nullptr || !cmd.mesh.isLoaded())
				continue;

			if(cmd.material == nullptr || !cmd.material.isLoaded())
				continue;

			if(cmd.type == DebugDrawType::ScreenSpace)
			{
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
}