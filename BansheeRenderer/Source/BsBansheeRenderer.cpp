#include "BsBansheeRenderer.h"
#include "BsCamera.h"
#include "BsSceneManager.h"
#include "BsRenderable.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsCoreApplication.h"
#include "BsViewport.h"
#include "BsRenderTarget.h"
#include "BsRenderOperation.h"
#include "BsDefaultRenderQueue.h"
#include "BsOverlayManager.h"
#include "BsDrawHelper2D.h"
#include "BsDrawHelper3D.h"
#include "BsGUIManager.h"
#include "BsCoreThread.h"

#include "BsProfiler.h"

namespace BansheeEngine
{
	BansheeRenderer::BansheeRenderer()
	{
		mRenderQueue = bs_new<DefaultRenderQueue>();
	}

	BansheeRenderer::~BansheeRenderer()
	{
		bs_delete(mRenderQueue);
	}

	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::renderAll() 
	{
		gProfiler().beginSample("renderA");

		gBsSceneManager().updateRenderableBounds();

		CoreAccessor& coreAccessor = gCoreAccessor();
		const Vector<HCamera>& allCameras = gBsSceneManager().getAllCameras();

		struct RenderTargetRenderInfo
		{
			RenderTargetPtr target;
			Vector<HCamera> cameras;
		};

		// Find all unique render targets
		Vector<RenderTargetRenderInfo> camerasPerRenderTarget;
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
			Vector<HCamera>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		gProfiler().endSample("renderA");
		gProfiler().beginSample("renderB");

		// Render everything, target by target
		for(auto& camerasPerTarget : camerasPerRenderTarget)
		{
			RenderTargetPtr target = camerasPerTarget.target;
			const Vector<HCamera>& cameras = camerasPerTarget.cameras;

			coreAccessor.beginRender();

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

			coreAccessor.endRender();
			coreAccessor.swapBuffers(target);
		}

		gProfiler().endSample("renderB");
	}

	void BansheeRenderer::render(const HCamera& camera) 
	{
		gProfiler().beginSample("renderC");

		Vector<HRenderable> allRenderables;
		
		if(!camera->getIgnoreSceneRenderables())
			allRenderables = gBsSceneManager().getVisibleRenderables(camera);

		CoreAccessor& coreAccessor = gCoreAccessor();
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
		const Vector<SortedRenderOp>& sortedROps =  mRenderQueue->getSortedRenderOps();

		gProfiler().endSample("renderF");

		for(auto iter = sortedROps.begin(); iter != sortedROps.end(); ++iter)
		{
			gProfiler().beginSample("renderG");

			const RenderOperation& renderOp = *iter->baseOperation;
			MaterialPtr material = renderOp.material;

			gProfiler().endSample("renderG");
			gProfiler().beginSample("renderH");

			PassPtr pass = material->getPass(iter->passIdx);
			PassParametersPtr paramsPtr = material->getPassParameters(iter->passIdx);

			coreAccessor.setPass(pass, paramsPtr);

			gProfiler().endSample("renderH");
			gProfiler().beginSample("renderI");

			const SubMesh& subMesh = renderOp.mesh->getSubMesh(renderOp.submeshIdx);
			coreAccessor.render(renderOp.mesh, subMesh.indexOffset, subMesh.indexCount, true, subMesh.drawOp);

			gProfiler().endSample("renderI");
		}
	}
}