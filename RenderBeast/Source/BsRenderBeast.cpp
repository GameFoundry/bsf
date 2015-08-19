#include "BsRenderBeast.h"
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
#include "BsSamplerState.h"
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
#include "BsLitTexRenderableController.h"
#include "BsTime.h"
#include "BsRenderableElement.h"
#include "BsFrameAlloc.h"
#include "BsCoreObjectManager.h"
#include "BsRenderBeastOptions.h"
#include "BsSamplerOverrides.h"
#include "BsLightInternal.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	RenderBeast::RenderBeast()
		:mOptions(bs_shared_ptr_new<RenderBeastOptions>()), mOptionsDirty(true)
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
		mLitTexHandler = bs_new<LitTexRenderableController>();

		SPtr<ShaderCore> shader = createDefaultShader();
		mDummyMaterial = MaterialCore::create(shader);
	}

	void RenderBeast::destroyCore()
	{
		if (mLitTexHandler != nullptr)
			bs_delete(mLitTexHandler);

		mRenderTargets.clear();
		mCameraData.clear();
		mRenderables.clear();

		assert(mSamplerOverrides.empty());

		mDummyMaterial = nullptr;
	}

	void RenderBeast::_notifyRenderableAdded(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = (UINT32)mRenderables.size();

		renderable->setRendererId(renderableId);

		mRenderables.push_back(RenderableData());
		mWorldTransforms.push_back(renderable->getTransform());
		mWorldBounds.push_back(renderable->getBounds());

		RenderableData& renderableData = mRenderables.back();
		renderableData.renderable = renderable;

		if (renderable->getRenderableType() == RenType_LitTextured)
			renderableData.controller = mLitTexHandler;
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

	void RenderBeast::_notifyRenderableRemoved(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();
		RenderableHandlerCore* lastRenerable = mRenderables.back().renderable;
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
			std::swap(mWorldTransforms[renderableId], mWorldTransforms[lastRenderableId]);

			lastRenerable->setRendererId(renderableId);

			Vector<BeastRenderableElement>& lastRenderableElements = mRenderables[renderableId].elements;
			for (auto& element : elements)
				element.renderableId = renderableId;
		}

		// Last element is the one we want to erase
		mRenderables.erase(mRenderables.end() - 1);
		mWorldBounds.erase(mWorldBounds.end() - 1);
		mWorldTransforms.erase(mWorldTransforms.end() - 1);
	}

	void RenderBeast::_notifyRenderableUpdated(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();

		mWorldTransforms[renderableId] = renderable->getTransform();
		mWorldBounds[renderableId] = renderable->getBounds();
	}

	void RenderBeast::_notifyLightAdded(LightInternalCore* light)
	{
		UINT32 lightId = (UINT32)mLights.size();

		light->setRendererId(lightId);

		mLights.push_back(LightData());
		mLightWorldBounds.push_back(light->getBounds());

		LightData& lightData = mLights.back();
		lightData.internal = light;
	}

	void RenderBeast::_notifyLightUpdated(LightInternalCore* light)
	{
		UINT32 lightId = light->getRendererId();

		mLightWorldBounds[lightId] = light->getBounds();
	}

	void RenderBeast::_notifyLightRemoved(LightInternalCore* light)
	{
		UINT32 lightId = light->getRendererId();
		LightInternalCore* lastLight = mLights.back().internal;
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

	void RenderBeast::_notifyCameraAdded(const CameraHandlerCore* camera)
	{
		CameraData& camData = mCameraData[camera];
		camData.renderQueue = bs_shared_ptr_new<RenderQueue>();
	}

	void RenderBeast::_notifyCameraRemoved(const CameraHandlerCore* camera)
	{
		mCameraData.erase(camera);
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
		// Populate direct draw lists
		const Map<CameraHandler*, SceneCameraData>& allCameras = gSceneManager().getAllCameras();
		for (auto& cameraData : allCameras)
		{
			CameraHandlerPtr camera = cameraData.second.camera;
			HSceneObject cameraSO = cameraData.second.sceneObject;

			DrawListPtr drawList = bs_shared_ptr_new<DrawList>();

			// Get GUI render operations
			GUIManager::instance().render(camera->getViewport(), *drawList);

			// Get overlay render operations
			OverlayManager::instance().render(camera->getViewport(), *drawList);

			// Get any operations from hooked up callbacks
			const Viewport* viewportRawPtr = camera->getViewport().get();
			onRenderViewport(viewportRawPtr, *drawList);

			RenderQueuePtr renderQueue = bs_shared_ptr_new<RenderQueue>();
			const Vector<DrawOperation>& drawOps = drawList->getDrawOperations();

			for (auto& drawOp : drawOps)
			{
				SPtr<MaterialCore> materialCore = drawOp.material->getCore();
				SPtr<MeshCoreBase> meshCore = drawOp.mesh->getCore();
				SubMesh subMesh = meshCore->getProperties().getSubMesh(drawOp.submeshIdx);

				float distanceToCamera = (cameraSO->getPosition() - drawOp.worldPosition).length();
				renderQueue->add(materialCore, meshCore, subMesh, distanceToCamera);
			}

			gCoreAccessor().queueCommand(std::bind(&RenderBeast::addToRenderQueue, this, camera->getCore(), renderQueue));
		}

		// Sync all dirty sim thread CoreObject data to core thread
		CoreObjectManager::instance().syncToCore(gCoreAccessor());

		if (mOptionsDirty)
		{
			gCoreAccessor().queueCommand(std::bind(&RenderBeast::syncRenderOptions, this, *mOptions));
			mOptionsDirty = false;
		}

		gCoreAccessor().queueCommand(std::bind(&RenderBeast::renderAllCore, this, gTime().getTime()));
	}

	void RenderBeast::addToRenderQueue(const SPtr<CameraHandlerCore>& camera, RenderQueuePtr renderQueue)
	{
		RenderQueuePtr cameraRenderQueue = mCameraData[camera.get()].renderQueue;
		cameraRenderQueue->add(*renderQueue);
	}

	void RenderBeast::syncRenderOptions(const RenderBeastOptions& options)
	{
		bool filteringChanged = mCoreOptions->filtering != options.filtering;
		if (options.filtering == RenderBeastFiltering::Anisotropic)
			filteringChanged |= mCoreOptions->anisotropyMax != options.anisotropyMax;

		if (filteringChanged)
			refreshSamplerOverrides(true);

		*mCoreOptions = options;
	}

	void RenderBeast::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Note: I'm iterating over all sampler states every frame. If this ends up being a performance
		// issue consider handling this internally in MaterialCore which can only do it when sampler states
		// are actually modified after sync
		refreshSamplerOverrides();

		// Update global per-frame hardware buffers
		mLitTexHandler->updatePerFrameBuffers(time);

		// Sort cameras by render target
		for (auto& cameraData : mCameraData)
		{
			const CameraHandlerCore* camera = cameraData.first;
			SPtr<RenderTargetCore> renderTarget = camera->getViewport()->getTarget();

			if (renderTarget == nullptr)
				continue;

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

			RenderAPICore::instance().beginFrame();
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

				render(*camera, mCameraData[camera].renderQueue);
			}

			RenderAPICore::instance().endFrame();
			RenderAPICore::instance().swapBuffers(target);
		}

		mRenderTargets.clear();
	}

	void RenderBeast::render(const CameraHandlerCore& camera, RenderQueuePtr& renderQueue)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		// Update global per-frame hardware buffers
		mLitTexHandler->updatePerCameraBuffers(camera.getForward());

		Matrix4 projMatrixCstm = camera.getProjectionMatrixRS();
		Matrix4 viewMatrixCstm = camera.getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

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

		UINT64 cameraLayers = camera.getLayers();
		ConvexVolume worldFrustum = camera.getWorldFrustum();

		// Update per-object param buffers and queue render elements
		for (auto& renderableData : mRenderables)
		{
			RenderableHandlerCore* renderable = renderableData.renderable;
			RenderableController* controller = renderableData.controller;
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
						renderQueue->add(&renderElem, distanceToCamera);
				}
			}
		}

		renderQueue->sort();
		const Vector<RenderQueueElement>& sortedRenderElements = renderQueue->getSortedElements();

		for(auto iter = sortedRenderElements.begin(); iter != sortedRenderElements.end(); ++iter)
		{
			SPtr<MaterialCore> material = iter->material;

			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			if (renderElem != nullptr)
			{
				UINT32 rendererId = renderElem->renderableId;
				const RenderableData& renderableData = mRenderables[rendererId];

				RenderableHandlerCore* renderable = renderableData.renderable;
				RenderableController* controller = renderableData.controller;
				UINT32 renderableType = renderable->getRenderableType();
				
				if (controller != nullptr)
					controller->bindPerObjectBuffers(*renderElem);

				if (renderableType == RenType_LitTextured)
				{
					Matrix4 worldViewProjMatrix = viewProjMatrix * mWorldTransforms[rendererId];
					mLitTexHandler->updatePerObjectBuffers(*renderElem, worldViewProjMatrix);
				}

				UINT32 numPasses = renderElem->material->getNumPasses();
				for (UINT32 i = 0; i < numPasses; i++)
				{
					SPtr<PassParametersCore> passParams = renderElem->material->getPassParameters(i);

					for (UINT32 j = 0; j < PassParametersCore::NUM_PARAMS; j++)
					{
						SPtr<GpuParamsCore> params = passParams->getParamByIdx(j);
						if (params != nullptr)
							params->updateHardwareBuffers();
					}
				}
				
				if (renderElem != nullptr && renderElem->samplerOverrides != nullptr)
					setPass(material, iter->passIdx, &renderElem->samplerOverrides->passes[iter->passIdx]);
				else
					setPass(material, iter->passIdx, nullptr);

			}
			else
				setPass(material, iter->passIdx, nullptr);

			draw(iter->mesh, iter->subMesh);
		}

		renderQueue->clear();

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

	void RenderBeast::setPass(const SPtr<MaterialCore>& material, UINT32 passIdx, PassSamplerOverrides* samplerOverrides)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		SPtr<PassCore> pass = material->getPass(passIdx);
		SPtr<PassParametersCore> passParams = material->getPassParameters(passIdx);

		struct StageData
		{
			GpuProgramType type;
			bool enable;
			SPtr<GpuParamsCore> params;
			SPtr<GpuProgramCore> program;
		};

		const UINT32 numStages = 6;
		StageData stages[numStages] =
		{
			{
				GPT_VERTEX_PROGRAM, pass->hasVertexProgram(),
				passParams->mVertParams, pass->getVertexProgram()
			},
			{
				GPT_FRAGMENT_PROGRAM, pass->hasFragmentProgram(),
				passParams->mFragParams, pass->getFragmentProgram()
			},
			{
				GPT_GEOMETRY_PROGRAM, pass->hasGeometryProgram(),
				passParams->mGeomParams, pass->getGeometryProgram()
			},
			{
				GPT_HULL_PROGRAM, pass->hasHullProgram(),
				passParams->mHullParams, pass->getHullProgram()
			},
			{
				GPT_DOMAIN_PROGRAM, pass->hasDomainProgram(),
				passParams->mDomainParams, pass->getDomainProgram()
			},
			{
				GPT_COMPUTE_PROGRAM, pass->hasComputeProgram(),
				passParams->mComputeParams, pass->getComputeProgram()
			}
		};

		for (UINT32 i = 0; i < numStages; i++)
		{
			const StageData& stage = stages[i];

			if (stage.enable)
			{
				rs.bindGpuProgram(stage.program);

				SPtr<GpuParamsCore> params = stage.params;
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
			else
				rs.unbindGpuProgram(stage.type);
		}

		// TODO - Try to limit amount of state changes, if previous state is already the same

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
					float4x4 matWorldViewProj;
				}

				void vs_main(
				in float3 inPos : POSITION,
				out float4 oPosition : SV_Position)
				{
					oPosition = mul(matWorldViewProj, float4(inPos.xyz, 1));
				})";

			String psCode = R"(
				float4 ps_main() : SV_Target
				{
					return float4(0.3f, 0.9f, 0.3f, 1.0f);
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}
		else if (rsName == RenderAPIDX9)
		{
			String vsCode = R"(
				 BS_PARAM_BLOCK PerObject { matWorldViewProj }
				 float4x4 matWorldViewProj;

				 void vs_main(
				 in float3 inPos : POSITION,
				 out float4 oPosition : POSITION)
				 {
					 oPosition = mul(matWorldViewProj, float4(inPos.xyz, 1));
				 })";

			String psCode = R"(
				float4 ps_main() : COLOR0
				{
					return float4(0.3f, 0.9f, 0.3f, 1.0f);
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl9", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl9", GPT_FRAGMENT_PROGRAM, GPP_FS_2_0);
		}
		else if (rsName == RenderAPIOpenGL)
		{
			String vsCode = R"(
				uniform PerObject
				{
					mat4 matWorldViewProj;
				};

				in vec3 bs_position;

				out gl_PerVertex
				{
					vec4 gl_Position;
				};

				void main()
				{
					gl_Position = matWorldViewProj * vec4(bs_position.xyz, 1);
				})";

			String psCode = R"(
				out vec4 fragColor;

				void main()
				{
					fragColor = vec4(0.3f, 0.9f, 0.3f, 1.0f);
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
		shaderDesc.addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);

		SPtr<ShaderCore> defaultShader = ShaderCore::create("DummyShader", shaderDesc, { newTechnique });

		return defaultShader;
	}
}