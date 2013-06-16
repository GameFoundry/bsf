#include "BsForwardRenderer.h"
#include "BsCamera.h"
#include "BsSceneManager.h"
#include "BsRenderable.h"
#include "CmMaterial.h"
#include "CmMesh.h"
#include "CmPass.h"
#include "CmBlendState.h"
#include "CmRasterizerState.h"
#include "CmDepthStencilState.h"
#include "CmApplication.h"
#include "CmViewport.h"
#include "CmRenderTarget.h"
#include "BsOverlayManager.h"
#include "BsGUIManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ForwardRenderer::ForwardRenderer()
	{
	}

	ForwardRenderer::~ForwardRenderer()
	{ }

	const String& ForwardRenderer::getName() const
	{
		static String name = "ForwardRenderer";
		return name;
	}

	void ForwardRenderer::renderAll() 
	{
		RenderContext& renderContext = gMainCA();
		const Vector<HCamera>::type& allCameras = gSceneManager().getAllCameras();

		// Find all unique render targets
		UnorderedSet<RenderTargetPtr>::type renderTargets;
		for(auto& camera : allCameras)
		{
			RenderTargetPtr target = camera->getViewport()->getTarget();
			auto findIter = renderTargets.find(target);

			if(findIter == renderTargets.end())
			{
				renderTargets.insert(target);
			}
		}

		// Clear all targets
		for(auto& target : renderTargets)
			renderContext.clear(target, FBT_COLOR | FBT_DEPTH, Color::Blue);

		renderContext.beginFrame();

		// TODO - Attempt to render all different elements in such a way that there is only 1 render target switch per render target

		// Render all cameras
		for(auto& camera : allCameras)
			render(camera);

		// Render overlays for all targets
		for(auto& camera : allCameras)
			OverlayManager::instance().render(camera->getViewport(), renderContext);

		// Render all GUI elements
		for(auto& camera : allCameras)
			GUIManager::instance().render(camera->getViewport(), renderContext);

		renderContext.endFrame();

		// Swap all targets
		for(auto& target : renderTargets)
			renderContext.swapBuffers(target);
	}

	void ForwardRenderer::render(const HCamera& camera) 
	{
		Vector<HRenderable>::type allRenderables;
		
		if(!camera->getIgnoreSceneRenderables())
			allRenderables = gSceneManager().getVisibleRenderables(camera);

		RenderContext& renderContext = gMainCA();
		renderContext.setViewport(camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		// TODO - sort renderables by material/pass/parameters to minimize state changes
		for(auto iter = allRenderables.begin(); iter != allRenderables.end(); ++iter)
		{
			HMaterial material = (*iter)->getMaterial();

			if(material == nullptr || !material.isLoaded())
				continue;

			HMesh mesh = (*iter)->getMesh();

			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			// TODO - Renderer should ensure shader is compatible with it, and it contains all the needed parameters
			// (probably at an earlier stage). e.g. I want the user to be warned if the shader doesn't contain matViewProjection param
			// (or should we just ignore such missing parameters?)
			material->setMat4("matViewProjection", viewProjMatrix);

			for(UINT32 i = 0; i < material->getNumPasses(); i++)
			{
				PassPtr pass = material->getPass(i);
				pass->activate(renderContext);

				PassParametersPtr paramsPtr = material->getPassParameters(i);
				pass->bindParameters(renderContext, paramsPtr);

				renderContext.render(mesh->getRenderOperation());
			}
		}

		// TODO - Sort renderables
		// Render them
	}
}