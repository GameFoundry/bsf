#include "BsBansheeRenderer.h"
#include "BsCamera.h"
#include "BsSceneObject.h"
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
#include "BsRenderQueue.h"
#include "BsOverlayManager.h"
#include "BsGUIManager.h"
#include "BsCoreThread.h"
#include "BsGpuParams.h"
#include "BsProfilerCPU.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsDrawList.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsShader.h"
#include "BsBansheeLitTexRenderableController.h"
#include "BsTime.h"
#include "BsFrameAlloc.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	BansheeRenderer::BansheeRenderer()
	{
		mRenderableRemovedConn = gSceneManager().onRenderableRemoved.connect(std::bind(&BansheeRenderer::renderableRemoved, this, _1));
	}

	BansheeRenderer::~BansheeRenderer()
	{
		mRenderableRemovedConn.disconnect();
	}

	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::_onActivated()
	{
		Renderer::_onActivated();

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::initializeCore, this));
	}

	void BansheeRenderer::_onDeactivated()
	{
		Renderer::_onDeactivated();

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::destroyCore, this));
		gCoreAccessor().submitToCoreThread(true);
	}

	void BansheeRenderer::initializeCore()
	{
		mLitTexHandler = bs_new<LitTexRenderableController>();
	}

	void BansheeRenderer::destroyCore()
	{
		if (mLitTexHandler != nullptr)
			bs_delete(mLitTexHandler);

		mRenderTargets.clear();
		mCameraData.clear();
	}

	void BansheeRenderer::addRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			mRenderableElements.push_back(element);
			mWorldTransforms.push_back(element->worldTransform);
			mWorldBounds.push_back(element->calculateWorldBounds());

			element->renderableType = proxy->renderableType;
			if (proxy->renderableType == RenType_LitTextured)
				element->handler = mLitTexHandler;
			else
				element->handler = nullptr;

			if (element->handler != nullptr)
				element->handler->initializeRenderElem(element);

			element->id = (UINT32)(mRenderableElements.size() - 1);
		}
	}

	void BansheeRenderer::removeRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			assert(mRenderableElements.size() > element->id && element->id >= 0);

			if (mRenderableElements.size() == 0)
				mRenderableElements.erase(mRenderableElements.begin());
			else
			{
				std::swap(mRenderableElements[element->id], mRenderableElements.back());
				mRenderableElements.erase(mRenderableElements.end() - 1);

				mRenderableElements[element->id]->id = element->id;
			}
		}
	}

	void BansheeRenderer::updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld)
	{
		for (auto& element : proxy->renderableElements)
		{
			element->worldTransform = localToWorld;

			mWorldTransforms[element->id] = localToWorld;
			mWorldBounds[element->id] = element->calculateWorldBounds();
		}
	}

	void BansheeRenderer::_notifyCameraAdded(const CameraHandlerCore* camera)
	{
		CameraData& camData = mCameraData[camera];
		camData.renderQueue = bs_shared_ptr<RenderQueue>();
	}

	void BansheeRenderer::_notifyCameraRemoved(const CameraHandlerCore* camera)
	{
		mCameraData.erase(camera);
	}

	void BansheeRenderer::renderableRemoved(const RenderableHandlerPtr& renderable)
	{
		if (renderable->_getActiveProxy() != nullptr)
		{
			mDeletedRenderableProxies.push_back(renderable->_getActiveProxy());
		}
	}

	void BansheeRenderer::renderAll() 
	{
		gSceneManager().updateRenderableTransforms();

		// Remove proxies from deleted Renderables
		for (auto& camera : mDeletedRenderableProxies)
		{
			if (camera != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, camera));
		}

		// Add or update Renderable proxies
		const Vector<SceneRenderableData>& allRenderables = gSceneManager().getAllRenderables();
		Vector<HSceneObject> dirtySceneObjects;
		Vector<RenderableHandlerPtr> dirtyRenderables;

		for (auto& renderableData : allRenderables)
		{
			RenderableHandlerPtr renderable = renderableData.renderable;
			HSceneObject renderableSO = renderableData.sceneObject;

			bool addedNewProxy = false;
			RenderableProxyPtr proxy = renderable->_getActiveProxy();

			if (renderableSO->getActive())
			{
				if (renderable->_isCoreDirty())
				{
					if (proxy != nullptr)
						gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));

					proxy = renderable->_createProxy(renderableSO->getWorldTfrm());
					renderable->_setActiveProxy(proxy);

					if (proxy != nullptr)
						gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addRenderableProxy, this, proxy));

					dirtyRenderables.push_back(renderable);
					dirtySceneObjects.push_back(renderableSO);
					addedNewProxy = true;
				}
				else if (renderableSO->_isCoreDirty())
				{
					if (proxy != nullptr)
						gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateRenderableProxy, this, proxy, renderableSO->getWorldTfrm()));

					dirtySceneObjects.push_back(renderableSO);
				}
			}
			else // If inactive we remove the proxy until re-activated
			{
				if (proxy != nullptr)
				{
					gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));
					renderable->_setActiveProxy(nullptr);
					renderable->_markCoreDirty();
				}
			}
		}

		// Mark all renderables as clean (needs to be done after all proxies are updated as
		// this will also clean materials & meshes which may be shared, so we don't want to clean them
		// too early.
		for (auto& renderable : dirtyRenderables)
		{
			renderable->_markCoreClean();
		}

		// Mark scene objects clean
		for (auto& dirtySO : dirtySceneObjects)
		{
			dirtySO->_markCoreClean();
		}

		// Populate direct draw lists
		const Vector<SceneCameraData>& allCameras = gSceneManager().getAllCameras();
		for (auto& cameraData : allCameras)
		{
			CameraHandlerPtr camera = cameraData.camera;
			HSceneObject cameraSO = cameraData.sceneObject;

			DrawListPtr drawList = bs_shared_ptr<DrawList>();

			// Get GUI render operations
			GUIManager::instance().render(camera->getViewport(), *drawList);

			// Get overlay render operations
			OverlayManager::instance().render(camera->getViewport(), *drawList);

			// Get any operations from hooked up callbacks
			const Viewport* viewportRawPtr = camera->getViewport().get();
			onRenderViewport(viewportRawPtr, *drawList);

			RenderQueuePtr renderQueue = bs_shared_ptr<RenderQueue>();
			const Vector<DrawOperation>& drawOps = drawList->getDrawOperations();

			for (auto& drawOp : drawOps)
			{
				SPtr<MaterialCore> materialCore = drawOp.material->getCore();
				SPtr<MeshCoreBase> meshCore = drawOp.mesh->getCore();
				SubMesh subMesh = meshCore->getProperties().getSubMesh(drawOp.submeshIdx);

				float distanceToCamera = (cameraSO->getPosition() - drawOp.worldPosition).length();
				renderQueue->add(materialCore, meshCore, subMesh, distanceToCamera);
			}

			gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addToRenderQueue, this, camera->getCore(), renderQueue));
		}

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::renderAllCore, this, gTime().getTime()));
	}

	void BansheeRenderer::addToRenderQueue(const SPtr<CameraHandlerCore>& camera, RenderQueuePtr renderQueue)
	{
		RenderQueuePtr cameraRenderQueue = mCameraData[camera.get()].renderQueue;
		cameraRenderQueue->add(*renderQueue);
	}

	void BansheeRenderer::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Update global hardware buffers
		mLitTexHandler->updateGlobalBuffers(time);

		// Sort cameras by render target
		for (auto& cameraData : mCameraData)
		{
			const CameraHandlerCore* camera = cameraData.first;
			SPtr<RenderTargetCore> renderTarget = camera->getViewport()->getTarget();

			auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });
			if (findIter != mRenderTargets.end())
			{
				findIter->cameras.push_back(camera);
			}
			else
			{
				mRenderTargets.push_back(RenderTargetData());
				RenderTargetData& renderTargetData = mRenderTargets.back();

				renderTargetData.target = renderTarget;
				renderTargetData.cameras.push_back(camera);
			}
		}

		// Sort everything based on priority
		auto cameraComparer = [&](const CameraHandlerCore* a, const CameraHandlerCore* b) { return a->getPriority() > b->getPriority(); };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b)
		{ return a.target->getProperties().getPriority() > b.target->getProperties().getPriority(); };
		std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : mRenderTargets)
		{
			Vector<const CameraHandlerCore*>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		// Render everything, target by target
		for (auto& renderTargetData : mRenderTargets)
		{
			SPtr<RenderTargetCore> target = renderTargetData.target;
			Vector<const CameraHandlerCore*>& cameras = renderTargetData.cameras;

			RenderSystem::instance().beginFrame();

			for(auto& camera : cameras)
			{
				SPtr<ViewportCore> viewport = camera->getViewport();
				RenderSystem::instance().setRenderTarget(target);
				RenderSystem::instance().setViewport(viewport->getNormArea());

				UINT32 clearBuffers = 0;
				if(viewport->getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport->getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport->getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					RenderSystem::instance().clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());

				render(*camera, mCameraData[camera].renderQueue);
			}

			RenderSystem::instance().endFrame();
			RenderSystem::instance().swapBuffers(target);
		}

		mRenderTargets.clear();
	}

	void BansheeRenderer::render(const CameraHandlerCore& camera, const RenderQueuePtr& renderQueue)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		Matrix4 projMatrixCstm = camera.getProjectionMatrixRS();
		Matrix4 viewMatrixCstm = camera.getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		onCorePreRenderViewport(camera);

		if (!camera.getIgnoreSceneRenderables())
		{
			ConvexVolume worldFrustum = camera.getWorldFrustum();

			// Update per-object param buffers and queue render elements
			for (auto& renderElem : mRenderableElements)
			{
				if (renderElem->handler != nullptr)
					renderElem->handler->bindPerObjectBuffers(renderElem);

				if (renderElem->renderableType == RenType_LitTextured)
				{
					Matrix4 worldViewProjMatrix = viewProjMatrix * mWorldTransforms[renderElem->id];;
					mLitTexHandler->updatePerObjectBuffers(renderElem, worldViewProjMatrix);
				}

				UINT32 numPasses = renderElem->material->getNumPasses();
				for (UINT32 i = 0; i < numPasses; i++)
				{
					SPtr<PassParametersCore> passParams = renderElem->material->getPassParameters(i);

					for (UINT32 j = 0; j < passParams->getNumParams(); j++)
					{
						SPtr<GpuParamsCore> params = passParams->getParamByIdx(j);
						if (params != nullptr)
							params->updateHardwareBuffers();
					}
				}

				// Do frustum culling
				// TODO - This is bound to be a bottleneck at some point. When it is ensure that intersect
				// methods use vector operations, as it is trivial to update them.
				const Sphere& boundingSphere = mWorldBounds[renderElem->id].getSphere();
				if (worldFrustum.intersects(boundingSphere))
				{
					// More precise with the box
					const AABox& boundingBox = mWorldBounds[renderElem->id].getBox();

					if (worldFrustum.intersects(boundingBox))
					{
						float distanceToCamera = (camera.getPosition() - boundingBox.getCenter()).length();

						renderQueue->add(renderElem, distanceToCamera);
					}
				}
			}
		}

		renderQueue->sort();
		const Vector<RenderQueueElement>& sortedRenderElements = renderQueue->getSortedElements();

		for(auto iter = sortedRenderElements.begin(); iter != sortedRenderElements.end(); ++iter)
		{
			SPtr<MaterialCore> material = iter->material;

			setPass(material, iter->passIdx);
			draw(iter->mesh, iter->subMesh);
		}

		renderQueue->clear();

		onCorePostRenderViewport(camera);
	}
}