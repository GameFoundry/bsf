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

			if(cmd.type == DebugDrawType::ClipSpace)
			{
				HMaterial mat = cmd.matInfo2DClipSpace.material;

				if(mat == nullptr || !mat.isLoaded() || !mat->isInitialized())
					continue;

				renderQueue.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::ScreenSpace)
			{
				HMaterial mat = cmd.matInfo2DScreenSpace.material;

				if(mat == nullptr || !mat.isLoaded() || !mat->isInitialized())
					continue;

				cmd.matInfo2DScreenSpace.invViewportWidth.set(invViewportWidth);
				cmd.matInfo2DScreenSpace.invViewportHeight.set(invViewportHeight);

				renderQueue.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::WorldSpace)
			{
				HMaterial mat = cmd.matInfo3D.material;

				if(mat == nullptr || !mat.isLoaded() || !mat->isInitialized())
					continue;

				cmd.matInfo3D.matViewProj.set(viewProjMatrix);

				renderQueue.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
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