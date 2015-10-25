#include "BsRenderBeast.h"
#include "BsCCamera.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsCRenderable.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsSamplerState.h"
#include "BsCoreApplication.h"
#include "BsViewport.h"
#include "BsRenderTarget.h"
#include "BsRenderQueue.h"
#include "BsGUIManager.h"
#include "BsCoreThread.h"
#include "BsGpuParams.h"
#include "BsProfilerCPU.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsShader.h"
#include "BsStaticRenderableHandler.h"
#include "BsTime.h"
#include "BsRenderableElement.h"
#include "BsFrameAlloc.h"
#include "BsCoreObjectManager.h"
#include "BsRenderBeastOptions.h"
#include "BsSamplerOverrides.h"
#include "BsLight.h"
#include "BsRenderTexturePool.h"
#include "BsRenderTargets.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	RenderBeast::RenderBeast()
		:mOptions(bs_shared_ptr_new<RenderBeastOptions>()), mOptionsDirty(true), mStaticHandler(nullptr)
	{

	}

	const StringID& RenderBeast::getName() const
	{
		static StringID name = "RenderBeast";
		return name;
	}

	void RenderBeast::_onActivated()
	{
		CoreRenderer::_onActivated();

		gCoreAccessor().queueCommand(std::bind(&RenderBeast::initializeCore, this));
	}

	void RenderBeast::_onDeactivated()
	{
		CoreRenderer::_onDeactivated();

		gCoreAccessor().queueCommand(std::bind(&RenderBeast::destroyCore, this));
		gCoreAccessor().submitToCoreThread(true);
	}

	void RenderBeast::initializeCore()
	{
		mCoreOptions = bs_shared_ptr_new<RenderBeastOptions>();
		mStaticHandler = bs_new<StaticRenderableHandler>();

		RenderTexturePool::startUp();

		SPtr<ShaderCore> shader = createDefaultShader();
		mDummyMaterial = MaterialCore::create(shader);
	}

	void RenderBeast::destroyCore()
	{
		if (mStaticHandler != nullptr)
			bs_delete(mStaticHandler);

		mRenderTargets.clear();
		mCameraData.clear();
		mRenderables.clear();

		RenderTexturePool::shutDown();

		assert(mSamplerOverrides.empty());

		mDummyMaterial = nullptr;
	}

	void RenderBeast::_notifyRenderableAdded(RenderableCore* renderable)
	{
		UINT32 renderableId = (UINT32)mRenderables.size();

		renderable->setRendererId(renderableId);

		mRenderables.push_back(RenderableData());
		mRenderableShaderData.push_back(RenderableShaderData());
		mWorldBounds.push_back(renderable->getBounds());

		RenderableData& renderableData = mRenderables.back();
		renderableData.renderable = renderable;

		RenderableShaderData& shaderData = mRenderableShaderData.back();
		shaderData.worldTransform = renderable->getTransform();
		shaderData.invWorldTransform = shaderData.worldTransform.inverseAffine();
		shaderData.worldNoScaleTransform = renderable->getTransformNoScale();
		shaderData.invWorldNoScaleTransform = shaderData.worldNoScaleTransform.inverseAffine();
		shaderData.worldDeterminantSign = shaderData.worldTransform.determinant3x3() >= 0.0f ? 1.0f : -1.0f;

		if (renderable->getRenderableType() == RenType_LitTextured)
			renderableData.controller = mStaticHandler;
		else
			renderableData.controller = nullptr;

		SPtr<MeshCore> mesh = renderable->getMesh();
		if (mesh != nullptr)
		{
			const MeshProperties& meshProps = mesh->getProperties();
			for (UINT32 i = 0; i < meshProps.getNumSubMeshes(); i++)
			{
				renderableData.elements.push_back(BeastRenderableElement());
				BeastRenderableElement& renElement = renderableData.elements.back();

				renElement.mesh = mesh;
				renElement.subMesh = meshProps.getSubMesh(i);
				renElement.renderableId = renderableId;

				renElement.material = renderable->getMaterial(i);
				if (renElement.material == nullptr)
					renElement.material = renderable->getMaterial(0);

				if (renElement.material == nullptr)
					renElement.material = mDummyMaterial;

				auto iterFind = mSamplerOverrides.find(renElement.material);
				if (iterFind != mSamplerOverrides.end())
				{
					renElement.samplerOverrides = iterFind->second;
					iterFind->second->refCount++;
				}
				else
				{
					MaterialSamplerOverrides* samplerOverrides = SamplerOverrideUtility::generateSamplerOverrides(renElement.material, mCoreOptions);
					mSamplerOverrides[renElement.material] = samplerOverrides;

					renElement.samplerOverrides = samplerOverrides;
					samplerOverrides->refCount++;
				}

				if (renderableData.controller != nullptr)
					renderableData.controller->initializeRenderElem(renElement);
			}
		}
	}

	void RenderBeast::_notifyRenderableRemoved(RenderableCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();
		RenderableCore* lastRenerable = mRenderables.back().renderable;
		UINT32 lastRenderableId = lastRenerable->getRendererId();

		Vector<BeastRenderableElement>& elements = mRenderables[renderableId].elements;
		for (auto& element : elements)
		{
			auto iterFind = mSamplerOverrides.find(element.material);
			assert(iterFind != mSamplerOverrides.end());

			MaterialSamplerOverrides* samplerOverrides = iterFind->second;
			samplerOverrides->refCount--;
			if (samplerOverrides->refCount == 0)
			{
				SamplerOverrideUtility::destroySamplerOverrides(samplerOverrides);
				mSamplerOverrides.erase(iterFind);
			}

			element.samplerOverrides = nullptr;
		}

		if (renderableId != lastRenderableId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mRenderables[renderableId], mRenderables[lastRenderableId]);
			std::swap(mWorldBounds[renderableId], mWorldBounds[lastRenderableId]);
			std::swap(mRenderableShaderData[renderableId], mRenderableShaderData[lastRenderableId]);

			lastRenerable->setRendererId(renderableId);

			Vector<BeastRenderableElement>& lastRenderableElements = mRenderables[renderableId].elements;
			for (auto& element : elements)
				element.renderableId = renderableId;
		}

		// Last element is the one we want to erase
		mRenderables.erase(mRenderables.end() - 1);
		mWorldBounds.erase(mWorldBounds.end() - 1);
		mRenderableShaderData.erase(mRenderableShaderData.end() - 1);
	}

	void RenderBeast::_notifyRenderableUpdated(RenderableCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();

		RenderableShaderData& shaderData = mRenderableShaderData[renderableId];
		shaderData.worldTransform = renderable->getTransform();
		shaderData.invWorldTransform = shaderData.worldTransform.inverseAffine();
		shaderData.worldNoScaleTransform = renderable->getTransformNoScale();
		shaderData.invWorldNoScaleTransform = shaderData.worldNoScaleTransform.inverseAffine();
		shaderData.worldDeterminantSign = shaderData.worldTransform.determinant3x3() >= 0.0f ? 1.0f : -1.0f;

		mWorldBounds[renderableId] = renderable->getBounds();
	}

	void RenderBeast::_notifyLightAdded(LightCore* light)
	{
		UINT32 lightId = (UINT32)mLights.size();

		light->setRendererId(lightId);

		mLights.push_back(LightData());
		mLightWorldBounds.push_back(light->getBounds());

		LightData& lightData = mLights.back();
		lightData.internal = light;
	}

	void RenderBeast::_notifyLightUpdated(LightCore* light)
	{
		UINT32 lightId = light->getRendererId();

		mLightWorldBounds[lightId] = light->getBounds();
	}

	void RenderBeast::_notifyLightRemoved(LightCore* light)
	{
		UINT32 lightId = light->getRendererId();
		LightCore* lastLight = mLights.back().internal;
		UINT32 lastLightId = lastLight->getRendererId();

		if (lightId != lastLightId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mLights[lightId], mLights[lastLightId]);
			std::swap(mLightWorldBounds[lightId], mLightWorldBounds[lastLightId]);

			lastLight->setRendererId(lightId);
		}

		// Last element is the one we want to erase
		mLights.erase(mLights.end() - 1);
		mLightWorldBounds.erase(mLightWorldBounds.end() - 1);
	}

	void RenderBeast::_notifyCameraAdded(const CameraCore* camera)
	{
		CameraData& camData = mCameraData[camera];
		camData.opaqueQueue = bs_shared_ptr_new<RenderQueue>(mCoreOptions->stateReductionMode);

		StateReduction transparentStateReduction = mCoreOptions->stateReductionMode;
		if (transparentStateReduction == StateReduction::Material)
			transparentStateReduction = StateReduction::Distance; // Transparent object MUST be sorted by distance

		camData.transparentQueue = bs_shared_ptr_new<RenderQueue>(transparentStateReduction);

		// Register in render target list
		SPtr<RenderTargetCore> renderTarget = camera->getViewport()->getTarget();
		if (renderTarget == nullptr)
			return;

		auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), 
			[&](const RenderTargetData& x) { return x.target == renderTarget; });

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

		// Sort render targets based on priority
		auto cameraComparer = [&](const CameraCore* a, const CameraCore* b) { return a->getPriority() > b->getPriority(); };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b)
		{ return a.target->getProperties().getPriority() > b.target->getProperties().getPriority(); };
		std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : mRenderTargets)
		{
			Vector<const CameraCore*>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}
	}

	void RenderBeast::_notifyCameraRemoved(const CameraCore* camera)
	{
		mCameraData.erase(camera);

		// Remove from render target list
		for (auto iterTarget = mRenderTargets.begin(); iterTarget != mRenderTargets.end(); ++iterTarget)
		{
			RenderTargetData& target = *iterTarget;
			for (auto iterCam = target.cameras.begin(); iterCam != target.cameras.end(); ++iterCam)
			{
				if (camera == *iterCam)
				{
					target.cameras.erase(iterCam);
					break;
				}
			}

			if (target.cameras.empty())
			{
				mRenderTargets.erase(iterTarget);
				break;
			}
		}
	}

	void RenderBeast::setOptions(const SPtr<CoreRendererOptions>& options)
	{
		mOptions = std::static_pointer_cast<RenderBeastOptions>(options);
		mOptionsDirty = true;
	}

	SPtr<CoreRendererOptions> RenderBeast::getOptions() const
	{
		return mOptions;
	}

	void RenderBeast::renderAll() 
	{
		// Sync all dirty sim thread CoreObject data to core thread
		CoreObjectManager::instance().syncToCore(gCoreAccessor());

		if (mOptionsDirty)
		{
			gCoreAccessor().queueCommand(std::bind(&RenderBeast::syncRenderOptions, this, *mOptions));
			mOptionsDirty = false;
		}

		gCoreAccessor().queueCommand(std::bind(&RenderBeast::renderAllCore, this, gTime().getTime()));
	}

	void RenderBeast::syncRenderOptions(const RenderBeastOptions& options)
	{
		bool filteringChanged = mCoreOptions->filtering != options.filtering;
		if (options.filtering == RenderBeastFiltering::Anisotropic)
			filteringChanged |= mCoreOptions->anisotropyMax != options.anisotropyMax;

		if (filteringChanged)
			refreshSamplerOverrides(true);

		*mCoreOptions = options;

		for (auto& cameraData : mCameraData)
		{
			cameraData.second.opaqueQueue->setStateReduction(mCoreOptions->stateReductionMode);

			StateReduction transparentStateReduction = mCoreOptions->stateReductionMode;
			if (transparentStateReduction == StateReduction::Material)
				transparentStateReduction = StateReduction::Distance; // Transparent object MUST be sorted by distance

			cameraData.second.transparentQueue->setStateReduction(transparentStateReduction);
		}
	}

	void RenderBeast::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Note: I'm iterating over all sampler states every frame. If this ends up being a performance
		// issue consider handling this internally in MaterialCore which can only do it when sampler states
		// are actually modified after sync
		refreshSamplerOverrides();

		// Update global per-frame hardware buffers
		mStaticHandler->updatePerFrameBuffers(time);

		// Generate render queues per camera
		for (auto& cameraData : mCameraData)
		{
			const CameraCore* camera = cameraData.first;
			determineVisible(*camera);
		}

		// Render everything, target by target
		for (auto& renderTargetData : mRenderTargets)
		{
			SPtr<RenderTargetCore> target = renderTargetData.target;
			Vector<const CameraCore*>& cameras = renderTargetData.cameras;

			RenderAPICore::instance().beginFrame();

			//UINT32 numCameras = (UINT32)cameras.size();
			//for (UINT32 i = 0; i < numCameras; i++)
			//	render(renderTargetData, i);

			// BEGIN OLD STUFF
			RenderAPICore::instance().setRenderTarget(target);
			for(auto& camera : cameras)
			{
				SPtr<ViewportCore> viewport = camera->getViewport();
				RenderAPICore::instance().setViewport(viewport->getNormArea());

				UINT32 clearBuffers = 0;
				if(viewport->getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport->getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport->getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					RenderAPICore::instance().clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());

				renderOLD(*camera);
			}
			// END OLD STUFF

			RenderAPICore::instance().endFrame();
			RenderAPICore::instance().swapBuffers(target);
		}
	}

	void RenderBeast::render(RenderTargetData& rtData, UINT32 camIdx)
	{
		const CameraCore* camera = rtData.cameras[camIdx];
		CameraData& camData = mCameraData[camera];
		SPtr<ViewportCore> viewport = camera->getViewport();

		Matrix4 projMatrix = camera->getProjectionMatrixRS();
		Matrix4 viewMatrix = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrix * viewMatrix;

		mStaticHandler->updatePerCameraBuffers(viewProjMatrix, viewMatrix, projMatrix, camera->getForward());

		// Render scene object to g-buffer if there are any
		const Vector<RenderQueueElement>& opaqueElements = camData.opaqueQueue->getSortedElements();
		bool hasGBuffer = opaqueElements.size() > 0;

		if (hasGBuffer)
		{
			bool createGBuffer = camData.gbuffer == nullptr ||
				camData.gbuffer->getHDR() != mCoreOptions->hdr ||
				camData.gbuffer->getNumSamples() != mCoreOptions->msaa;

			if (createGBuffer)
				camData.gbuffer = RenderTargets::create(*viewport, mCoreOptions->hdr, mCoreOptions->msaa);

			camData.gbuffer->bind();

			UINT32 clearBuffers = 0;
			if (viewport->getRequiresColorClear())
				clearBuffers |= FBT_COLOR;

			if (viewport->getRequiresDepthClear())
				clearBuffers |= FBT_DEPTH;

			if (viewport->getRequiresStencilClear())
				clearBuffers |= FBT_STENCIL;

			if (clearBuffers != 0)
				RenderAPICore::instance().clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());

			for (auto iter = opaqueElements.begin(); iter != opaqueElements.end(); ++iter)
			{
				BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
				SPtr<MaterialCore> material = renderElem->material;

				UINT32 rendererId = renderElem->renderableId;
				Matrix4 worldViewProjMatrix = viewProjMatrix * mRenderableShaderData[rendererId].worldTransform;

				mStaticHandler->updatePerObjectBuffers(*renderElem, mRenderableShaderData[rendererId], worldViewProjMatrix);
				mStaticHandler->bindGlobalBuffers(*renderElem); // Note: If I can keep global buffer slot indexes the same between shaders I could only bind these once
				mStaticHandler->bindPerObjectBuffers(*renderElem);

				if (iter->applyPass)
				{
					SPtr<PassCore> pass = material->getPass(iter->passIdx);
					setPass(pass);
				}

				SPtr<PassParametersCore> passParams = material->getPassParameters(iter->passIdx);

				if (renderElem->samplerOverrides != nullptr)
					setPassParams(passParams, &renderElem->samplerOverrides->passes[iter->passIdx]);
				else
					setPassParams(passParams, nullptr);

				draw(iter->renderElem->mesh, iter->renderElem->subMesh);
			}
		}
		else
			camData.gbuffer = nullptr;

		// Prepare final render target
		SPtr<RenderTargetCore> target = rtData.target;

		// If first camera in render target, prepare the RT
		if (camIdx == 0)
		{
			RenderAPICore::instance().setRenderTarget(target);
			RenderAPICore::instance().setViewport(viewport->getNormArea());

			UINT32 clearBuffers = 0;
			if (viewport->getRequiresColorClear())
				clearBuffers |= FBT_COLOR;

			if (viewport->getRequiresDepthClear())
				clearBuffers |= FBT_DEPTH;

			if (viewport->getRequiresStencilClear())
				clearBuffers |= FBT_STENCIL;

			if (clearBuffers != 0)
				RenderAPICore::instance().clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());
		}

		// Trigger pre-scene callbacks
		auto iterCameraCallbacks = mRenderCallbacks.find(camera);
		if (iterCameraCallbacks != mRenderCallbacks.end())
		{
			for (auto& callbackPair : iterCameraCallbacks->second)
			{
				if (callbackPair.first >= 0)
					break;

				callbackPair.second();
			}
		}

		// Resolve gbuffer if there is one
		if (hasGBuffer)
		{
			// TODO - Render lights
			// TODO - Resolve to render target
			
			camData.gbuffer->unbind();
		}

		// Render transparent objects
		const Vector<RenderQueueElement>& transparentElements = camData.transparentQueue->getSortedElements();
		for (auto iter = transparentElements.begin(); iter != transparentElements.end(); ++iter)
		{
			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			SPtr<MaterialCore> material = renderElem->material;

			UINT32 rendererId = renderElem->renderableId;
			Matrix4 worldViewProjMatrix = viewProjMatrix * mRenderableShaderData[rendererId].worldTransform;

			mStaticHandler->updatePerObjectBuffers(*renderElem, mRenderableShaderData[rendererId], worldViewProjMatrix);
			mStaticHandler->bindGlobalBuffers(*renderElem); // Note: If I can keep global buffer slot indexes the same between shaders I could only bind these once
			mStaticHandler->bindPerObjectBuffers(*renderElem);

			if (iter->applyPass)
			{
				SPtr<PassCore> pass = material->getPass(iter->passIdx);
				setPass(pass);
			}

			SPtr<PassParametersCore> passParams = material->getPassParameters(iter->passIdx);

			if (renderElem->samplerOverrides != nullptr)
				setPassParams(passParams, &renderElem->samplerOverrides->passes[iter->passIdx]);
			else
				setPassParams(passParams, nullptr);

			draw(iter->renderElem->mesh, iter->renderElem->subMesh);
		}

		camData.opaqueQueue->clear();
		camData.transparentQueue->clear();

		// Render post-scene callbacks
		if (iterCameraCallbacks != mRenderCallbacks.end())
		{
			for (auto& callbackPair : iterCameraCallbacks->second)
			{
				if (callbackPair.first < 0)
					continue;

				callbackPair.second();
			}
		}
	}

	void RenderBeast::renderOLD(const CameraCore& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();
		CameraData& cameraData = mCameraData[&camera];

		Matrix4 projMatrix = camera.getProjectionMatrixRS();
		Matrix4 viewMatrix = camera.getViewMatrix();

		Matrix4 viewProjMatrix = projMatrix * viewMatrix;

		// Trigger pre-render callbacks
		auto iterCameraCallbacks = mRenderCallbacks.find(&camera);
		if (iterCameraCallbacks != mRenderCallbacks.end())
		{
			for (auto& callbackPair : iterCameraCallbacks->second)
			{
				if (callbackPair.first >= 0)
					break;

				callbackPair.second();
			}
		}

		// Render opaque

		//// Update global per-frame hardware buffers
		mStaticHandler->updatePerCameraBuffers(viewProjMatrix, viewMatrix, projMatrix, camera.getForward());

		// TODO - This bit can be removed once I fully switch to deferred
		const Vector<RenderQueueElement>& opaqueElements = cameraData.opaqueQueue->getSortedElements();
		for(auto iter = opaqueElements.begin(); iter != opaqueElements.end(); ++iter)
		{
			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			SPtr<MaterialCore> material = renderElem->material;

			UINT32 rendererId = renderElem->renderableId;
			Matrix4 worldViewProjMatrix = viewProjMatrix * mRenderableShaderData[rendererId].worldTransform;

			mStaticHandler->updatePerObjectBuffers(*renderElem, mRenderableShaderData[rendererId], worldViewProjMatrix);
			mStaticHandler->bindGlobalBuffers(*renderElem); // Note: If I can keep global buffer slot indexes the same between shaders I could only bind these once
			mStaticHandler->bindPerObjectBuffers(*renderElem);

			if (iter->applyPass)
			{
				SPtr<PassCore> pass = material->getPass(iter->passIdx);
				setPass(pass);
			}

			{
				SPtr<PassParametersCore> passParams = material->getPassParameters(iter->passIdx);

				if (renderElem->samplerOverrides != nullptr)
					setPassParams(passParams, &renderElem->samplerOverrides->passes[iter->passIdx]);
				else
					setPassParams(passParams, nullptr);
			}

			draw(iter->renderElem->mesh, iter->renderElem->subMesh);
		}

		// Render transparent
		const Vector<RenderQueueElement>& transparentElements = cameraData.transparentQueue->getSortedElements();
		for (auto iter = transparentElements.begin(); iter != transparentElements.end(); ++iter)
		{
			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			SPtr<MaterialCore> material = renderElem->material;

			UINT32 rendererId = renderElem->renderableId;
			Matrix4 worldViewProjMatrix = viewProjMatrix * mRenderableShaderData[rendererId].worldTransform;

			mStaticHandler->updatePerObjectBuffers(*renderElem, mRenderableShaderData[rendererId], worldViewProjMatrix);
			mStaticHandler->bindGlobalBuffers(*renderElem); // Note: If I can keep global buffer slot indexes the same between shaders I could only bind these once
			mStaticHandler->bindPerObjectBuffers(*renderElem);

			if (iter->applyPass)
			{
				SPtr<PassCore> pass = material->getPass(iter->passIdx);
				setPass(pass);
			}

			SPtr<PassParametersCore> passParams = material->getPassParameters(iter->passIdx);

			if (renderElem->samplerOverrides != nullptr)
				setPassParams(passParams, &renderElem->samplerOverrides->passes[iter->passIdx]);
			else
				setPassParams(passParams, nullptr);

			draw(iter->renderElem->mesh, iter->renderElem->subMesh);
		}

		cameraData.opaqueQueue->clear();
		cameraData.transparentQueue->clear();

		// Trigger post-render callbacks
		if (iterCameraCallbacks != mRenderCallbacks.end())
		{
			for (auto& callbackPair : iterCameraCallbacks->second)
			{
				if (callbackPair.first < 0)
					continue;

				callbackPair.second();
			}
		}
	}

	void RenderBeast::determineVisible(const CameraCore& camera)
	{
		CameraData& cameraData = mCameraData[&camera];

		UINT64 cameraLayers = camera.getLayers();
		ConvexVolume worldFrustum = camera.getWorldFrustum();

		// Update per-object param buffers and queue render elements
		for (auto& renderableData : mRenderables)
		{
			RenderableCore* renderable = renderableData.renderable;
			RenderableHandler* controller = renderableData.controller;
			UINT32 renderableType = renderable->getRenderableType();
			UINT32 rendererId = renderable->getRendererId();

			if ((renderable->getLayer() & cameraLayers) == 0)
				continue;

			// Do frustum culling
			// TODO - This is bound to be a bottleneck at some point. When it is ensure that intersect
			// methods use vector operations, as it is trivial to update them.
			const Sphere& boundingSphere = mWorldBounds[rendererId].getSphere();
			if (worldFrustum.intersects(boundingSphere))
			{
				// More precise with the box
				const AABox& boundingBox = mWorldBounds[rendererId].getBox();

				if (worldFrustum.intersects(boundingBox))
				{
					float distanceToCamera = (camera.getPosition() - boundingBox.getCenter()).length();

					for (auto& renderElem : renderableData.elements)
					{
						bool isTransparent = (renderElem.material->getShader()->getFlags() & (UINT32)ShaderFlags::Transparent) != 0;

						if (isTransparent)
							cameraData.transparentQueue->add(&renderElem, distanceToCamera);
						else
							cameraData.opaqueQueue->add(&renderElem, distanceToCamera);
					}

				}
			}
		}

		cameraData.opaqueQueue->sort();
		cameraData.transparentQueue->sort();
	}

	Vector2 RenderBeast::getDeviceZTransform(const Matrix4& projMatrix)
	{
		Vector2 output;
		output.x = 1.0f / projMatrix[2][2];
		output.y = projMatrix[2][3] / projMatrix[2][2];

		return output;
	}

	void RenderBeast::refreshSamplerOverrides(bool force)
	{
		for (auto& entry : mSamplerOverrides)
		{
			SPtr<MaterialCore> material = entry.first;

			if (force)
			{
				SamplerOverrideUtility::destroySamplerOverrides(entry.second);
				entry.second = SamplerOverrideUtility::generateSamplerOverrides(material, mCoreOptions);
			}
			else
			{
				MaterialSamplerOverrides* materialOverrides = entry.second;
				UINT32 numPasses = material->getNumPasses();

				assert(numPasses == materialOverrides->numPasses);
				for (UINT32 i = 0; i < numPasses; i++)
				{
					SPtr<PassParametersCore> passParams = material->getPassParameters(i);
					PassSamplerOverrides& passOverrides = materialOverrides->passes[i];

					for (UINT32 j = 0; j < PassParametersCore::NUM_PARAMS; j++)
					{
						StageSamplerOverrides& stageOverrides = passOverrides.stages[j];

						SPtr<GpuParamsCore> params = passParams->getParamByIdx(j);
						if (params == nullptr)
							continue;

						const GpuParamDesc& paramDesc = params->getParamDesc();

						for (auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
						{
							UINT32 slot = iter->second.slot;
							SPtr<SamplerStateCore> samplerState = params->getSamplerState(slot);

							assert(stageOverrides.numStates > slot);

							if (samplerState != stageOverrides.stateOverrides[slot])
							{
								if (samplerState != nullptr)
									stageOverrides.stateOverrides[slot] = SamplerOverrideUtility::generateSamplerOverride(samplerState, mCoreOptions);
								else
									stageOverrides.stateOverrides[slot] = SamplerOverrideUtility::generateSamplerOverride(SamplerStateCore::getDefault(), mCoreOptions);;
							}	
						}
					}
				}
			}
		}
	}

	void RenderBeast::setPass(const SPtr<PassCore>& pass)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		struct StageData
		{
			GpuProgramType type;
			bool enable;
			SPtr<GpuProgramCore> program;
		};

		const UINT32 numStages = 6;
		StageData stages[numStages] =
		{
			{ GPT_VERTEX_PROGRAM, pass->hasVertexProgram(), pass->getVertexProgram() },
			{ GPT_FRAGMENT_PROGRAM, pass->hasFragmentProgram(), pass->getFragmentProgram() },
			{ GPT_GEOMETRY_PROGRAM, pass->hasGeometryProgram(), pass->getGeometryProgram() },
			{ GPT_HULL_PROGRAM, pass->hasHullProgram(), pass->getHullProgram() },
			{ GPT_DOMAIN_PROGRAM, pass->hasDomainProgram(), pass->getDomainProgram() },
			{ GPT_COMPUTE_PROGRAM, pass->hasComputeProgram(), pass->getComputeProgram() }
		};

		for (UINT32 i = 0; i < numStages; i++)
		{
			const StageData& stage = stages[i];

			if (stage.enable)
				rs.bindGpuProgram(stage.program);
			else
				rs.unbindGpuProgram(stage.type);
		}

		// Set up non-texture related pass settings
		if (pass->getBlendState() != nullptr)
			rs.setBlendState(pass->getBlendState());
		else
			rs.setBlendState(BlendStateCore::getDefault());

		if (pass->getDepthStencilState() != nullptr)
			rs.setDepthStencilState(pass->getDepthStencilState(), pass->getStencilRefValue());
		else
			rs.setDepthStencilState(DepthStencilStateCore::getDefault(), pass->getStencilRefValue());

		if (pass->getRasterizerState() != nullptr)
			rs.setRasterizerState(pass->getRasterizerState());
		else
			rs.setRasterizerState(RasterizerStateCore::getDefault());
	}

	void RenderBeast::setPassParams(const SPtr<PassParametersCore>& passParams, const PassSamplerOverrides* samplerOverrides)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		struct StageData
		{
			GpuProgramType type;
			SPtr<GpuParamsCore> params;
		};

		const UINT32 numStages = 6;
		StageData stages[numStages] =
		{
			{ GPT_VERTEX_PROGRAM, passParams->mVertParams },
			{ GPT_FRAGMENT_PROGRAM, passParams->mFragParams },
			{ GPT_GEOMETRY_PROGRAM, passParams->mGeomParams },
			{ GPT_HULL_PROGRAM, passParams->mHullParams },
			{ GPT_DOMAIN_PROGRAM, passParams->mDomainParams },
			{ GPT_COMPUTE_PROGRAM, passParams->mComputeParams }
		};

		for (UINT32 i = 0; i < numStages; i++)
		{
			const StageData& stage = stages[i];

			SPtr<GpuParamsCore> params = stage.params;
			if (params == nullptr)
				continue;

			const GpuParamDesc& paramDesc = params->getParamDesc();

			for (auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
			{
				SPtr<SamplerStateCore> samplerState;

				if (samplerOverrides != nullptr)
					samplerState = samplerOverrides->stages[i].stateOverrides[iter->second.slot];
				else
					samplerState = params->getSamplerState(iter->second.slot);

				if (samplerState == nullptr)
					rs.setSamplerState(stage.type, iter->second.slot, SamplerStateCore::getDefault());
				else
					rs.setSamplerState(stage.type, iter->second.slot, samplerState);
			}

			for (auto iter = paramDesc.textures.begin(); iter != paramDesc.textures.end(); ++iter)
			{
				SPtr<TextureCore> texture = params->getTexture(iter->second.slot);

				if (!params->isLoadStoreTexture(iter->second.slot))
				{
					if (texture == nullptr)
						rs.setTexture(stage.type, iter->second.slot, false, nullptr);
					else
						rs.setTexture(stage.type, iter->second.slot, true, texture);
				}
				else
				{
					const TextureSurface& surface = params->getLoadStoreSurface(iter->second.slot);

					if (texture == nullptr)
						rs.setLoadStoreTexture(stage.type, iter->second.slot, false, nullptr, surface);
					else
						rs.setLoadStoreTexture(stage.type, iter->second.slot, true, texture, surface);
				}
			}

			rs.setConstantBuffers(stage.type, params);
		}
	}

	SPtr<ShaderCore> RenderBeast::createDefaultShader()
	{
		StringID rsName = RenderAPICore::instance().getName();

		SPtr<GpuProgramCore> vsProgram;
		SPtr<GpuProgramCore> psProgram;

		if (rsName == RenderAPIDX11)
		{
			String vsCode = R"(
				cbuffer PerObject
				{
					float4x4 gMatWorldViewProj;
					float4x4 gMatWorld;
					float4x4 gMatInvWorld;
					float4x4 gMatWorldNoScale;
					float4x4 gMatInvWorldNoScale;
					float gWorldDeterminantSign;
				}

				void vs_main(
					in float3 inPos : POSITION,
					in float3 inNormal : NORMAL,
					out float4 oPosition : SV_Position,
					out float3 oNormal : NORMAL)
				{
					oPosition = mul(gMatWorldViewProj, float4(inPos.xyz, 1));
					oNormal = inNormal;
				})";

			String psCode = R"(
				cbuffer PerCamera
				{
					float3 gViewDir;
					float4x4 gMatViewProj;
					float4x4 gMatView;
					float4x4 gMatProj;
				}

				float4 ps_main(
					in float4 inPos : SV_Position,
					in float3 normal : NORMAL) : SV_Target
				{
					float4 outColor = float4(0.3f, 0.3f, 0.3f, 1.0f) * clamp(dot(normalize(normal), -gViewDir), 0.5f, 1.0);
					outColor.a = 1.0f;
				
					return outColor;
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}
		else if (rsName == RenderAPIDX9)
		{
			String vsCode = R"(
				BS_PARAM_BLOCK PerObject { gMatWorldViewProj, gMatWorld, gMatInvWorld, gMatWorldNoScale, gMatInvWorldNoScale, gMatWorldDeterminantSign }

				float4x4 gMatWorldViewProj;
				float4x4 gMatWorld;
				float4x4 gMatInvWorld;
				float4x4 gMatWorldNoScale;
				float4x4 gMatInvWorldNoScale;
				float gWorldDeterminantSign;

				 void vs_main(
					in float3 inPos : POSITION,
					in float3 inNormal : NORMAL,
					out float4 oPosition : POSITION,
					out float3 oNormal : TEXCOORD0)
				 {
					 oPosition = mul(gMatWorldViewProj, float4(inPos.xyz, 1));
					 oNormal = inNormal;
				 })";

			String psCode = R"(
				 BS_PARAM_BLOCK PerCamera { gViewDir, gMatViewProj, gMatView, gMatProj }
				 float3 gViewDir;
				 float4x4 gMatViewProj;
				 float4x4 gMatView;
				 float4x4 gMatProj;

				float4 ps_main(
					in float3 inPos : POSITION,
					in float3 inNormal : TEXCOORD0) : COLOR0
				{
					float4 outColor = float4(0.3f, 0.3f, 0.3f, 1.0f) * clamp(dot(normalize(inNormal), -gViewDir), 0.5f, 1.0);
					outColor.a = 1.0f;
				
					return outColor;
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl9", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl9", GPT_FRAGMENT_PROGRAM, GPP_FS_2_0);
		}
		else if (rsName == RenderAPIOpenGL)
		{
			String vsCode = R"(
				uniform PerObject
				{
					mat4 gMatWorldViewProj;
					mat4 gMatWorld;
					mat4 gMatInvWorld;
					mat4 gMatWorldNoScale;
					mat4 gMatInvWorldNoScale;
					float gWorldDeterminantSign;
				};

				in vec3 bs_position;
				in vec3 bs_normal;
				out vec3 normal;

				out gl_PerVertex
				{
					vec4 gl_Position;
				};

				void main()
				{
					gl_Position = gMatWorldViewProj * vec4(bs_position.xyz, 1);
					normal = bs_normal;
				})";

			String psCode = R"(
				uniform PerCamera
				{
					vec3 gViewDir;
					mat4 gMatViewProj;
					mat4 gMatView;
					mat4 gMatProj;
				};

				in vec3 normal;
				out vec4 fragColor;

				void main()
				{
					vec4 outColor = vec4(0.3f, 0.3f, 0.3f, 1.0f) * clamp(dot(normalize(normal), -gViewDir), 0.5f, 1.0);
					outColor.a = 1.0f;
				
					fragColor = outColor;
				})";

			vsProgram = GpuProgramCore::create(vsCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}

		PASS_DESC_CORE passDesc;
		passDesc.vertexProgram = vsProgram;
		passDesc.fragmentProgram = psProgram;

		SPtr<PassCore> newPass = PassCore::create(passDesc);
		SPtr<TechniqueCore> newTechnique = TechniqueCore::create(rsName, RendererDefault, { newPass });

		SHADER_DESC_CORE shaderDesc;
		shaderDesc.setParamBlockAttribs("PerObject", true, GPBU_DYNAMIC, RBS_PerObject);
		shaderDesc.setParamBlockAttribs("PerCamera", true, GPBU_DYNAMIC, RBS_PerCamera);
		shaderDesc.addParameter("gMatWorldViewProj", "gMatWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);
		shaderDesc.addParameter("gViewDir", "gViewDir", GPDT_FLOAT3, RPS_ViewDir);

		SPtr<ShaderCore> defaultShader = ShaderCore::create("DummyShader", shaderDesc, { newTechnique });

		return defaultShader;
	}
}