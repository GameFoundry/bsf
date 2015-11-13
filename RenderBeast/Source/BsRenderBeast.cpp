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
		:mOptions(bs_shared_ptr_new<RenderBeastOptions>()), mOptionsDirty(true), mStaticHandler(nullptr),
		mDefaultMaterial(nullptr), mDefaultNoNormalMaterial(nullptr), mPointLightMat(nullptr), mDirLightMat(nullptr)
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

		CoreThread::instance().queueCommand(std::bind(&RenderBeast::initializeCore, this));
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

		mDefaultMaterial = bs_new<DefaultMaterial>();
		mDefaultNoNormalMaterial = bs_new<DefaultMaterialNoNormal>();
		mPointLightMat = bs_new<PointLightMat>();
		mDirLightMat = bs_new<DirectionalLightMat>();

		RenderTexturePool::startUp();
	}

	void RenderBeast::destroyCore()
	{
		if (mStaticHandler != nullptr)
			bs_delete(mStaticHandler);

		mRenderTargets.clear();
		mCameraData.clear();
		mRenderables.clear();

		RenderTexturePool::shutDown();

		bs_delete(mDefaultMaterial);
		bs_delete(mDefaultNoNormalMaterial);
		bs_delete(mPointLightMat);
		bs_delete(mDirLightMat);

		assert(mSamplerOverrides.empty());
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
				{
					SPtr<VertexData> vertexData = mesh->getVertexData();
					const VertexDeclarationProperties& vertexProps = vertexData->vertexDeclaration->getProperties();

					if (vertexProps.findElementBySemantic(VES_NORMAL))
						renElement.material = mDefaultMaterial->getMaterial();
					else
						renElement.material = mDefaultNoNormalMaterial->getMaterial();
				}

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
		if (light->getType() == LightType::Directional)
		{
			UINT32 lightId = (UINT32)mDirectionalLights.size();

			light->setRendererId(lightId);

			mDirectionalLights.push_back(LightData());

			LightData& lightData = mDirectionalLights.back();
			lightData.internal = light;
		}
		else
		{
			UINT32 lightId = (UINT32)mPointLights.size();

			light->setRendererId(lightId);

			mPointLights.push_back(LightData());
			mLightWorldBounds.push_back(light->getBounds());

			LightData& lightData = mPointLights.back();
			lightData.internal = light;
		}
	}

	void RenderBeast::_notifyLightUpdated(LightCore* light)
	{
		UINT32 lightId = light->getRendererId();

		if (light->getType() != LightType::Directional)
			mLightWorldBounds[lightId] = light->getBounds();
	}

	void RenderBeast::_notifyLightRemoved(LightCore* light)
	{
		UINT32 lightId = light->getRendererId();
		if (light->getType() == LightType::Directional)
		{
			LightCore* lastLight = mDirectionalLights.back().internal;
			UINT32 lastLightId = lastLight->getRendererId();

			if (lightId != lastLightId)
			{
				// Swap current last element with the one we want to erase
				std::swap(mDirectionalLights[lightId], mDirectionalLights[lastLightId]);
				lastLight->setRendererId(lightId);
			}

			// Last element is the one we want to erase
			mDirectionalLights.erase(mDirectionalLights.end() - 1);
		}
		else
		{
			LightCore* lastLight = mPointLights.back().internal;
			UINT32 lastLightId = lastLight->getRendererId();

			if (lightId != lastLightId)
			{
				// Swap current last element with the one we want to erase
				std::swap(mPointLights[lightId], mPointLights[lastLightId]);
				std::swap(mLightWorldBounds[lightId], mLightWorldBounds[lastLightId]);

				lastLight->setRendererId(lightId);
			}

			// Last element is the one we want to erase
			mPointLights.erase(mPointLights.end() - 1);
			mLightWorldBounds.erase(mLightWorldBounds.end() - 1);
		}
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

		// Render scene objects to g-buffer if there are any
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

		// Render lights and resolve gbuffer if there is one
		if (hasGBuffer)
		{
			SPtr<MaterialCore> dirMaterial = mDirLightMat->getMaterial();
			SPtr<PassCore> dirPass = dirMaterial->getPass(0);

			setPass(dirPass);

			for (auto& light : mDirectionalLights)
			{
				if (!light.internal->getIsActive())
					continue;

				mDirLightMat->setParameters(light.internal);

				SPtr<MeshCore> mesh = nullptr; // TODO - Get full screen quad
				draw(mesh, mesh->getProperties().getSubMesh(0));
			}

			// TODO - Cull lights based on visibility, right now I just iterate over all of them. 
			for (auto& light : mPointLights)
			{
				if (!light.internal->getIsActive())
					continue;

				mPointLightMat->setParameters(light.internal);

				SPtr<MeshCore> mesh = light.internal->getMesh();
				draw(mesh, mesh->getProperties().getSubMesh(0));
			}

			// TODO - Resolve to render target (Later: Manual resolve during deferred light pass?)
			
			camData.gbuffer->unbind();
		}

		// Render transparent objects (TODO - No lighting yet)
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
}