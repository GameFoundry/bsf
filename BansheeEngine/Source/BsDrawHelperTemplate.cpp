#include "BsDrawHelperTemplate.h"
#include "BsRect2.h"
#include "BsMesh.h"
#include "BsTime.h"
#include "BsVector2.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsCoreApplication.h"
#include "BsDrawList.h"
#include "BsCamera.h"
#include "BsBuiltinResources.h"

namespace BansheeEngine
{
	const UINT32 DrawHelperTemplateBase::NUM_VERTICES_AA_LINE = 8;
	const UINT32 DrawHelperTemplateBase::NUM_INDICES_AA_LINE = 30;

	void DrawHelperTemplateBase::render(const HCamera& camera, DrawList& drawList)
	{
		const Viewport* viewport = camera->getViewport().get();
		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

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

				drawList.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::ScreenSpace)
			{
				HMaterial mat = cmd.matInfo2DScreenSpace.material;

				if(mat == nullptr || !mat.isLoaded() || !mat->isInitialized())
					continue;

				cmd.matInfo2DScreenSpace.invViewportWidth.set(invViewportWidth);
				cmd.matInfo2DScreenSpace.invViewportHeight.set(invViewportHeight);

				drawList.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
			}
			else if(cmd.type == DebugDrawType::WorldSpace)
			{
				HMaterial mat = cmd.matInfo3D.material;

				if(mat == nullptr || !mat.isLoaded() || !mat->isInitialized())
					continue;

				cmd.matInfo3D.matViewProj.set(viewProjMatrix);

				drawList.add(mat.getInternalPtr(), cmd.mesh.getInternalPtr(), 0, cmd.worldCenter);
			}
		}

		float curTime = gTime().getTime();
		Vector<DebugDrawCommand> newCommands;
		for(auto& cmd : commands)
		{
			if(cmd.endTime > curTime)
				newCommands.push_back(cmd);
		}

		commands.swap(newCommands);
	}
}