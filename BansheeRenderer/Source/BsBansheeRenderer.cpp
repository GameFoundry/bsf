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
		mCameraRemovedConn = gSceneManager().onCameraRemoved.connect(std::bind(&BansheeRenderer::cameraRemoved, this, _1));
	}

	BansheeRenderer::~BansheeRenderer()
	{
		mRenderableRemovedConn.disconnect();
		mCameraRemovedConn.disconnect();
	}

	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::_onActivated()
	{
		Renderer::_onActivated();

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::createController, this));
	}

	void BansheeRenderer::_onDeactivated()
	{
		Renderer::_onDeactivated();

		gCoreAccessor().queueCommand(std::bind(&destroyController, mLitTexHandler));
	}

	void BansheeRenderer::createController()
	{
		mLitTexHandler = bs_new<LitTexRenderableController>();
	}

	void BansheeRenderer::destroyController(LitTexRenderableController* controller)
	{
		if (controller != nullptr)
			bs_delete(controller);
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

	void BansheeRenderer::addCameraProxy(CameraProxyPtr proxy)
	{
		SPtr<RenderTargetCore> renderTarget = proxy->renderTarget;
		auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });

		if (findIter != mRenderTargets.end())
		{
			proxy->renderQueue = bs_shared_ptr<RenderQueue>();

			findIter->cameras.push_back(proxy);
		}
		else
		{
			mRenderTargets.push_back(RenderTargetData());
			RenderTargetData& renderTargetData = mRenderTargets.back();

			proxy->renderQueue = bs_shared_ptr<RenderQueue>();
			renderTargetData.target = renderTarget;
			renderTargetData.cameras.push_back(proxy);
		}

		// Sort everything based on priority
		auto cameraComparer = [&](const CameraProxyPtr& a, const CameraProxyPtr& b) { return a->priority > b->priority; };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b) 
		{ return a.target->getProperties().getPriority() > b.target->getProperties().getPriority(); };
		std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : mRenderTargets)
		{
			Vector<CameraProxyPtr>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		proxy->calcWorldFrustum();
	}

	void BansheeRenderer::removeCameraProxy(CameraProxyPtr proxy)
	{
		SPtr<RenderTargetCore> renderTarget = proxy->renderTarget;
		auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });

		if (findIter != mRenderTargets.end())
		{
			auto findIter2 = std::find(findIter->cameras.begin(), findIter->cameras.end(), proxy);

			if (findIter2 != findIter->cameras.end())
			{
				findIter->cameras.erase(findIter2);
			}

			if (findIter->cameras.size() == 0)
				mRenderTargets.erase(findIter);
		}
	}

	void BansheeRenderer::updateCameraProxy(CameraProxyPtr proxy, Vector3 worldPosition, Matrix4 worldMatrix, Matrix4 viewMatrix)
	{
		proxy->viewMatrix = viewMatrix;
		proxy->worldPosition = worldPosition;
		proxy->worldMatrix = worldMatrix;
		proxy->calcWorldFrustum();
	}

	void BansheeRenderer::renderableRemoved(const RenderableHandlerPtr& renderable)
	{
		if (renderable->_getActiveProxy() != nullptr)
		{
			mDeletedRenderableProxies.push_back(renderable->_getActiveProxy());
		}
	}

	void BansheeRenderer::cameraRemoved(const CameraHandlerPtr& camera)
	{
		if (camera->_getActiveProxy() != nullptr)
		{
			mDeletedCameraProxies.push_back(camera->_getActiveProxy());
		}
	}

	void BansheeRenderer::renderAll() 
	{
		gSceneManager().updateRenderableTransforms();

		// Remove proxies from deleted Renderables
		for (auto& proxy : mDeletedRenderableProxies)
		{
			if (proxy != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));
		}

		// Add or update Renderable proxies
		const Vector<SceneRenderableData>& allRenderables = gSceneManager().getAllRenderables();
		Vector<HSceneObject> dirtySceneObjects;
		Vector<RenderableHandlerPtr> dirtyRenderables;

		FrameAlloc* frameAlloc = gCoreThread().getFrameAlloc();
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

		// Remove proxies from deleted Cameras
		for (auto& proxy : mDeletedCameraProxies)
		{
			if (proxy != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeCameraProxy, this, proxy));
		}

		// Add or update Camera proxies
		const Vector<SceneCameraData>& allCameras = gSceneManager().getAllCameras();
		for (auto& cameraData : allCameras)
		{
			CameraHandlerPtr camera = cameraData.camera;
			HSceneObject cameraSO = cameraData.sceneObject;

			if (camera->_isCoreDirty())
			{
				CameraProxyPtr proxy = camera->_getActiveProxy();

				if (proxy != nullptr)
					gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeCameraProxy, this, proxy));

				proxy = camera->_createProxy();
				camera->_setActiveProxy(proxy);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addCameraProxy, this, proxy));

				camera->_markCoreClean();
				dirtySceneObjects.push_back(cameraSO);
			}
			else if (cameraSO->_isCoreDirty())
			{
				CameraProxyPtr proxy = camera->_getActiveProxy();
				assert(proxy != nullptr);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateCameraProxy, this, 
					proxy, cameraSO->getWorldPosition(), cameraSO->getWorldTfrm(), camera->getViewMatrix()));

				dirtySceneObjects.push_back(cameraSO);
			}
		}

		// Mark scene objects clean
		for (auto& dirtySO : dirtySceneObjects)
		{
			dirtySO->_markCoreClean();
		}

		// Populate direct draw lists
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

			// Note: It is important that draw ops update happens after renderables are updated, so that
			// renderable proxies properly update in case they both share the same material/mesh
			for (auto& drawOp : drawOps)
			{
				// Note: It is assumed render operations queued in the draw list is going
				// to change every frame so we create new proxies using frame allocator
				// every frame. It /might/ be more efficient not to use frame allocator
				// and only update when they actually change. That might also cause
				// issue if material/mesh is used both in draw list and a Renderable

				if (drawOp.material->_isCoreDirty(MaterialDirtyFlag::Proxy))
				{
					drawOp.material->_setActiveProxy(drawOp.material->_createProxy());
					drawOp.material->_markCoreClean(MaterialDirtyFlag::Proxy);
					drawOp.material->_markCoreClean(MaterialDirtyFlag::Material);
				}

				MaterialProxyPtr materialProxy = drawOp.material->_getActiveProxy();
				SPtr<MeshCoreBase> meshCore = drawOp.mesh->getCore();
				SubMesh subMesh = meshCore->getProperties().getSubMesh(drawOp.submeshIdx);

				float distanceToCamera = (cameraSO->getPosition() - drawOp.worldPosition).length();
				renderQueue->add(materialProxy, meshCore, subMesh, distanceToCamera);
			}

			gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addToRenderQueue, this, camera->_getActiveProxy(), renderQueue));
		}

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::renderAllCore, this, gTime().getTime()));
	}

	void BansheeRenderer::addToRenderQueue(CameraProxyPtr proxy, RenderQueuePtr renderQueue)
	{
		RenderQueuePtr cameraRenderQueue = proxy->renderQueue;
		cameraRenderQueue->add(*renderQueue);
	}

	void BansheeRenderer::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Update global hardware buffers
		mLitTexHandler->updateGlobalBuffers(time);

		// Render everything, target by target
		for (auto& renderTargetData : mRenderTargets)
		{
			SPtr<RenderTargetCore> target = renderTargetData.target;
			Vector<CameraProxyPtr>& cameras = renderTargetData.cameras;

			RenderSystem::instance().beginFrame();

			for(auto& camera : cameras)
			{
				Viewport viewport = camera->viewport;
				RenderSystem::instance().setRenderTarget(target);
				RenderSystem::instance().setViewport(viewport.getNormArea());

				UINT32 clearBuffers = 0;
				if(viewport.getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport.getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport.getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					RenderSystem::instance().clearViewport(clearBuffers, viewport.getClearColor(), viewport.getClearDepthValue(), viewport.getClearStencilValue());

				render(*camera, camera->renderQueue);
			}

			RenderSystem::instance().endFrame();
			RenderSystem::instance().swapBuffers(target);
		}
	}

	void BansheeRenderer::render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue) 
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		Matrix4 projMatrixCstm = cameraProxy.projMatrix;
		Matrix4 viewMatrixCstm = cameraProxy.viewMatrix;

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		onCorePreRenderViewport(cameraProxy);

		if (!cameraProxy.ignoreSceneRenderables)
		{
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

				for (auto& param : renderElem->material->params)
				{
					param->updateHardwareBuffers();
				}

				// Do frustum culling
				// TODO - This is bound to be a bottleneck at some point. When it is ensure that intersect
				// methods use vector operations, as it is trivial to update them.
				const Sphere& boundingSphere = mWorldBounds[renderElem->id].getSphere();
				if (cameraProxy.worldFrustum.intersects(boundingSphere))
				{
					// More precise with the box
					const AABox& boundingBox = mWorldBounds[renderElem->id].getBox();

					if (cameraProxy.worldFrustum.intersects(boundingBox))
					{
						float distanceToCamera = (cameraProxy.worldPosition - boundingBox.getCenter()).length();

						renderQueue->add(renderElem, distanceToCamera);
					}
				}
			}
		}

		renderQueue->sort();
		const Vector<RenderQueueElement>& sortedRenderElements = renderQueue->getSortedElements();

		for(auto iter = sortedRenderElements.begin(); iter != sortedRenderElements.end(); ++iter)
		{
			MaterialProxyPtr materialProxy = iter->material;

			setPass(*materialProxy, iter->passIdx);
			draw(iter->mesh, iter->subMesh);
		}

		renderQueue->clear();

		onCorePostRenderViewport(cameraProxy);
	}
}