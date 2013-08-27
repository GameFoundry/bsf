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
#include "CmRenderOperation.h"
#include "CmDefaultRenderQueue.h"
#include "BsOverlayManager.h"
#include "BsDrawHelper2D.h"
#include "BsGUIManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ForwardRenderer::ForwardRenderer()
	{
		mRenderQueue = cm_new<DefaultRenderQueue>();
	}

	ForwardRenderer::~ForwardRenderer()
	{
		cm_delete(mRenderQueue);
	}

	const String& ForwardRenderer::getName() const
	{
		static String name = "ForwardRenderer";
		return name;
	}

	void ForwardRenderer::renderAll() 
	{
		gSceneManager().updateRenderableBounds();

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

			coreAccessor.beginFrame();

			for(auto& camera : cameras)
			{
				ViewportPtr viewport = camera->getViewport();
				coreAccessor.setViewport(viewport);

				UINT32 clearBuffers = 0;
				if(viewport->getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport->getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport->getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					coreAccessor.clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());

				render(camera);
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

		mRenderQueue->clear();

		// Get scene render operations
		for(auto iter = allRenderables.begin(); iter != allRenderables.end(); ++iter)
		{
			UINT32 numMaterials = (*iter)->getNumMaterials();

			for(UINT32 i = 0; i < numMaterials; i++)
			{
				// TODO - Do different things depending on material and renderable settings
				
				// TODO - Renderer should ensure shader is compatible with it, and it contains all the needed parameters
				// (probably at an earlier stage). e.g. I want the user to be warned if the shader doesn't contain matViewProjection param
				// (or should we just ignore such missing parameters?)
				HMaterial& material = (*iter)->getMaterial(i);
				material->setMat4("matViewProjection", viewProjMatrix);
			}

			(*iter)->render(*mRenderQueue);
		}

		// Get GUI render operations
		GUIManager::instance().render(camera->getViewport(), *mRenderQueue);

		// Get overlay render operations
		OverlayManager::instance().render(camera->getViewport(), *mRenderQueue);

		// Get debug render operations
		DrawHelper2D::instance().render(camera, *mRenderQueue);

		// TODO - Material queue is completely ignored
		mRenderQueue->sort();
		const Vector<SortedRenderOp>::type& sortedROps =  mRenderQueue->getSortedRenderOps();

		for(auto iter = sortedROps.begin(); iter != sortedROps.end(); ++iter)
		{
			const RenderOperation& renderOp = *iter->baseOperation;
			HMaterial material = renderOp.material;

			PassPtr pass = material->getPass(iter->passIdx);
			pass->activate(coreAccessor);

			PassParametersPtr paramsPtr = material->getPassParameters(iter->passIdx);
			pass->bindParameters(coreAccessor, paramsPtr);

			coreAccessor.render(renderOp.meshData);
		}
	}
}