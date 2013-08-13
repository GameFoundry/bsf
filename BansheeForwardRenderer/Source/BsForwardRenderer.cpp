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
		CoreAccessor& coreAccessor = gMainCA();
		const Vector<HCamera>::type& allCameras = gSceneManager().getAllCameras();

		struct RenderTargetRenderInfo
		{
			RenderTargetPtr target;
			Vector<HCamera>::type cameras;
		};

		// Find all unique render targets
		Vector<RenderTargetRenderInfo>::type camerasPerRenderTarget;
		for(auto& camera : allCameras)
		{
			RenderTargetPtr target = camera->getViewport()->getTarget();
			auto findIter = std::find_if(begin(camerasPerRenderTarget), end(camerasPerRenderTarget), [&target] (const RenderTargetRenderInfo& x) { return x.target == target; });

			if(findIter == camerasPerRenderTarget.end())
			{
				camerasPerRenderTarget.push_back(RenderTargetRenderInfo());
				camerasPerRenderTarget[camerasPerRenderTarget.size() - 1].target = target;
				camerasPerRenderTarget[camerasPerRenderTarget.size() - 1].cameras.push_back(camera);
			}
			else
				findIter->cameras.push_back(camera);
		}

		// Sort everything based on priority
		auto cameraComparer = [&] (const HCamera& a, const HCamera& b) { return a->getPriority() > b->getPriority(); };
		auto renderTargetInfoComparer = [&] (const RenderTargetRenderInfo& a, const RenderTargetRenderInfo& b) { return a.target->getPriority() > b.target->getPriority(); };
		std::sort(begin(camerasPerRenderTarget), end(camerasPerRenderTarget), renderTargetInfoComparer);

		for(auto& camerasPerTarget : camerasPerRenderTarget)
		{
			Vector<HCamera>::type& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		// Render everything, target by target
		for(auto& camerasPerTarget : camerasPerRenderTarget)
		{
			RenderTargetPtr target = camerasPerTarget.target;
			const Vector<HCamera>::type& cameras = camerasPerTarget.cameras;

			coreAccessor.setRenderTarget(target);
			coreAccessor.beginFrame();

			for(auto& camera : cameras)
			{
				ViewportPtr viewport = camera->getViewport();

				UINT32 clearBuffers = 0;
				if(viewport->getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport->getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport->getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					coreAccessor.clear(target, clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue(), viewport->getDimensions());

				// Render scene
				render(camera);

				// Render overlays
				OverlayManager::instance().render(camera->getViewport(), coreAccessor);

				// Render GUI elements
				GUIManager::instance().render(camera->getViewport(), coreAccessor);
			}

			coreAccessor.endFrame();
			coreAccessor.swapBuffers(target);
		}
	}

	void ForwardRenderer::render(const HCamera& camera) 
	{
		Vector<HRenderable>::type allRenderables;
		
		if(!camera->getIgnoreSceneRenderables())
			allRenderables = gSceneManager().getVisibleRenderables(camera);

		CoreAccessor& coreAccessor = gMainCA();
		coreAccessor.setViewport(camera->getViewport());

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
				pass->activate(coreAccessor);

				PassParametersPtr paramsPtr = material->getPassParameters(i);
				pass->bindParameters(coreAccessor, paramsPtr);

				coreAccessor.render(mesh->getRenderOperation());
			}
		}

		// TODO - Sort renderables
		// Render them
	}
}