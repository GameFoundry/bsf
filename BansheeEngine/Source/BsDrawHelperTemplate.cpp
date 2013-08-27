#include "BsDrawHelperTemplate.h"
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
	void DrawHelperTemplateBase::render(const HCamera& camera, CM::RenderQueue& renderQueue)
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
}