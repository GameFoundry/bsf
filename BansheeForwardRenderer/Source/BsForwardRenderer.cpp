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
#include "BsDrawHelper3D.h"
#include "BsGUIManager.h"

#include "CmProfiler.h"

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
		gProfiler().beginSample("renderA");

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

		gProfiler().endSample("renderA");
		gProfiler().beginSample("renderB");

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

		gProfiler().endSample("renderB");
	}

	void ForwardRenderer::render(const HCamera& camera) 
	{
		gProfiler().beginSample("renderC");

		Vector<HRenderable>::type allRenderables;
		
		if(!camera->getIgnoreSceneRenderables())
			allRenderables = gSceneManager().getVisibleRenderables(camera);

		CoreAccessor& coreAccessor = gMainCA();
		coreAccessor.setViewport(camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		mRenderQueue->clear();

		gProfiler().endSample("renderC");
		gProfiler().beginSample("renderD");

		// Get scene render operations
		for(auto iter = allRenderables.begin(); iter != allRenderables.end(); ++iter)
		{
			(*iter)->render(*mRenderQueue, viewProjMatrix);
		}

		gProfiler().endSample("renderD");
		gProfiler().beginSample("renderE");

		// Get GUI render operations
		GUIManager::instance().render(camera->getViewport(), *mRenderQueue);

		// Get overlay render operations
		OverlayManager::instance().render(camera->getViewport(), *mRenderQueue);

		// Get debug render operations
		DrawHelper3D::instance().render(camera, *mRenderQueue);
		DrawHelper2D::instance().render(camera, *mRenderQueue);

		gProfiler().endSample("renderE");
		gProfiler().beginSample("renderF");

		// Get any operations from hooked up callbacks
		const Viewport* viewportRawPtr = camera->getViewport().get();
		auto callbacksForViewport = mRenderCallbacks[viewportRawPtr];

		for(auto& callback : callbacksForViewport)
			callback(viewportRawPtr, *mRenderQueue);

		// TODO - Material queue is completely ignored
		mRenderQueue->sort();
		const Vector<SortedRenderOp>::type& sortedROps =  mRenderQueue->getSortedRenderOps();

		gProfiler().endSample("renderF");

		for(auto iter = sortedROps.begin(); iter != sortedROps.end(); ++iter)
		{
			gProfiler().beginSample("renderG");

			const RenderOperation& renderOp = *iter->baseOperation;
			MaterialPtr material = renderOp.material;

			PassPtr pass = material->getPass(iter->passIdx);
			pass->activate(coreAccessor);

			gProfiler().endSample("renderG");
			gProfiler().beginSample("renderH");

			PassParametersPtr paramsPtr = material->getPassParameters(iter->passIdx);
			pass->bindParameters(coreAccessor, paramsPtr);

			gProfiler().endSample("renderH");
			gProfiler().beginSample("renderI");

			const SubMesh& subMesh = renderOp.mesh->getSubMesh(renderOp.submeshIdx);
			coreAccessor.render(renderOp.mesh, subMesh.indexOffset, subMesh.indexCount, true, subMesh.drawOp);

			gProfiler().endSample("renderI");
		}
	}
}