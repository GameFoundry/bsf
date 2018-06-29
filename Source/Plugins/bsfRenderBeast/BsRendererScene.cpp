//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsRendererScene.h"
#include "Renderer/BsCamera.h"
#include "Renderer/BsLight.h"
#include "Renderer/BsSkybox.h"
#include "Renderer/BsReflectionProbe.h"
#include "Renderer/BsRenderer.h"
#include "Particles/BsParticleManager.h"
#include "Mesh/BsMesh.h"
#include "Material/BsPass.h"
#include "Material/BsGpuParamsSet.h"
#include "Utility/BsSamplerOverrides.h"
#include "BsRenderBeastOptions.h"
#include "BsRenderBeast.h"
#include "Image/BsSpriteTexture.h"

namespace bs {	namespace ct
{
	PerFrameParamDef gPerFrameParamDef;

	RendererScene::RendererScene(const SPtr<RenderBeastOptions>& options)
		:mOptions(options)
	{
		mPerFrameParamBuffer = gPerFrameParamDef.createBuffer();
	}

	RendererScene::~RendererScene()
	{
		for (auto& entry : mInfo.renderables)
			bs_delete(entry);

		for (auto& entry : mInfo.views)
			bs_delete(entry);

		assert(mSamplerOverrides.empty());
	}

	void RendererScene::registerCamera(Camera* camera)
	{
		RENDERER_VIEW_DESC viewDesc = createViewDesc(camera);

		RendererView* view = bs_new<RendererView>(viewDesc);
		view->setRenderSettings(camera->getRenderSettings());
		view->updatePerViewBuffer();

		UINT32 viewIdx = (UINT32)mInfo.views.size();
		mInfo.views.push_back(view);

		mInfo.cameraToView[camera] = viewIdx;
		camera->setRendererId(viewIdx);

		updateCameraRenderTargets(camera);
	}

	void RendererScene::updateCamera(Camera* camera, UINT32 updateFlag)
	{
		UINT32 cameraId = camera->getRendererId();
		RendererView* view = mInfo.views[cameraId];

		UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything 
			| (UINT32)ActorDirtyFlag::Active 
			| (UINT32)CameraDirtyFlag::Viewport;

		if((updateFlag & updateEverythingFlag) != 0)
		{
			RENDERER_VIEW_DESC viewDesc = createViewDesc(camera);

			view->setView(viewDesc);
			view->setRenderSettings(camera->getRenderSettings());

			updateCameraRenderTargets(camera);
		}
		else if((updateFlag & (UINT32)CameraDirtyFlag::RenderSettings) != 0)
		{
			view->setRenderSettings(camera->getRenderSettings());
		}
		else // Transform
		{
			view = mInfo.views[cameraId];

			const Transform& tfrm = camera->getTransform();
			view->setTransform(
				tfrm.getPosition(),
				tfrm.getForward(),
				camera->getViewMatrix(),
				camera->getProjectionMatrixRS(),
				camera->getWorldFrustum());
		}

		view->updatePerViewBuffer();
	}

	void RendererScene::unregisterCamera(Camera* camera)
	{
		UINT32 cameraId = camera->getRendererId();

		Camera* lastCamera = mInfo.views.back()->getSceneCamera();
		UINT32 lastCameraId = lastCamera->getRendererId();
		
		if (cameraId != lastCameraId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mInfo.views[cameraId], mInfo.views[lastCameraId]);
			lastCamera->setRendererId(cameraId);

			mInfo.cameraToView[lastCamera] = cameraId;
		}
		
		// Last element is the one we want to erase
		RendererView* view = mInfo.views[mInfo.views.size() - 1];
		bs_delete(view);

		mInfo.views.erase(mInfo.views.end() - 1);

		auto iterFind = mInfo.cameraToView.find(camera);
		if(iterFind != mInfo.cameraToView.end())
			mInfo.cameraToView.erase(iterFind);

		updateCameraRenderTargets(camera, true);
	}

	void RendererScene::registerLight(Light* light)
	{
		if (light->getType() == LightType::Directional)
		{
			UINT32 lightId = (UINT32)mInfo.directionalLights.size();
			light->setRendererId(lightId);

			mInfo.directionalLights.push_back(RendererLight(light));
		}
		else
		{
			if (light->getType() == LightType::Radial)
			{
				UINT32 lightId = (UINT32)mInfo.radialLights.size();
				light->setRendererId(lightId);

				mInfo.radialLights.push_back(RendererLight(light));
				mInfo.radialLightWorldBounds.push_back(light->getBounds());
			}
			else // Spot
			{
				UINT32 lightId = (UINT32)mInfo.spotLights.size();
				light->setRendererId(lightId);

				mInfo.spotLights.push_back(RendererLight(light));
				mInfo.spotLightWorldBounds.push_back(light->getBounds());
			}
		}
	}

	void RendererScene::updateLight(Light* light)
	{
		UINT32 lightId = light->getRendererId();

		if (light->getType() == LightType::Radial)
			mInfo.radialLightWorldBounds[lightId] = light->getBounds();
		else if(light->getType() == LightType::Spot)
			mInfo.spotLightWorldBounds[lightId] = light->getBounds();
	}

	void RendererScene::unregisterLight(Light* light)
	{
		UINT32 lightId = light->getRendererId();
		if (light->getType() == LightType::Directional)
		{
			Light* lastLight = mInfo.directionalLights.back().internal;
			UINT32 lastLightId = lastLight->getRendererId();

			if (lightId != lastLightId)
			{
				// Swap current last element with the one we want to erase
				std::swap(mInfo.directionalLights[lightId], mInfo.directionalLights[lastLightId]);
				lastLight->setRendererId(lightId);
			}

			// Last element is the one we want to erase
			mInfo.directionalLights.erase(mInfo.directionalLights.end() - 1);
		}
		else
		{
			if (light->getType() == LightType::Radial)
			{
				Light* lastLight = mInfo.radialLights.back().internal;
				UINT32 lastLightId = lastLight->getRendererId();

				if (lightId != lastLightId)
				{
					// Swap current last element with the one we want to erase
					std::swap(mInfo.radialLights[lightId], mInfo.radialLights[lastLightId]);
					std::swap(mInfo.radialLightWorldBounds[lightId], mInfo.radialLightWorldBounds[lastLightId]);

					lastLight->setRendererId(lightId);
				}

				// Last element is the one we want to erase
				mInfo.radialLights.erase(mInfo.radialLights.end() - 1);
				mInfo.radialLightWorldBounds.erase(mInfo.radialLightWorldBounds.end() - 1);
			}
			else // Spot
			{
				Light* lastLight = mInfo.spotLights.back().internal;
				UINT32 lastLightId = lastLight->getRendererId();

				if (lightId != lastLightId)
				{
					// Swap current last element with the one we want to erase
					std::swap(mInfo.spotLights[lightId], mInfo.spotLights[lastLightId]);
					std::swap(mInfo.spotLightWorldBounds[lightId], mInfo.spotLightWorldBounds[lastLightId]);

					lastLight->setRendererId(lightId);
				}

				// Last element is the one we want to erase
				mInfo.spotLights.erase(mInfo.spotLights.end() - 1);
				mInfo.spotLightWorldBounds.erase(mInfo.spotLightWorldBounds.end() - 1);
			}
		}
	}

	void RendererScene::registerRenderable(Renderable* renderable)
	{
		UINT32 renderableId = (UINT32)mInfo.renderables.size();

		renderable->setRendererId(renderableId);

		mInfo.renderables.push_back(bs_new<RendererRenderable>());
		mInfo.renderableCullInfos.push_back(CullInfo(renderable->getBounds(), renderable->getLayer()));

		RendererRenderable* rendererRenderable = mInfo.renderables.back();
		rendererRenderable->renderable = renderable;
		rendererRenderable->updatePerObjectBuffer();

		SPtr<Mesh> mesh = renderable->getMesh();
		if (mesh != nullptr)
		{
			const MeshProperties& meshProps = mesh->getProperties();
			SPtr<VertexDeclaration> vertexDecl = mesh->getVertexData()->vertexDeclaration;

			for (UINT32 i = 0; i < meshProps.getNumSubMeshes(); i++)
			{
				rendererRenderable->elements.push_back(RenderableElement());
				RenderableElement& renElement = rendererRenderable->elements.back();

				renElement.type = (UINT32)RenderElementType::Renderable;
				renElement.mesh = mesh;
				renElement.subMesh = meshProps.getSubMesh(i);
				renElement.animType = renderable->getAnimType();
				renElement.animationId = renderable->getAnimationId();
				renElement.morphShapeVersion = 0;
				renElement.morphShapeBuffer = renderable->getMorphShapeBuffer();
				renElement.boneMatrixBuffer = renderable->getBoneMatrixBuffer();
				renElement.morphVertexDeclaration = renderable->getMorphVertexDeclaration();

				renElement.material = renderable->getMaterial(i);
				if (renElement.material == nullptr)
					renElement.material = renderable->getMaterial(0);

				if (renElement.material != nullptr && renElement.material->getShader() == nullptr)
					renElement.material = nullptr;

				// If no material use the default material
				if (renElement.material == nullptr)
					renElement.material = Material::create(DefaultMaterial::get()->getShader());

				// Determine which technique to use
				static_assert((UINT32)RenderableAnimType::Count == 4, "RenderableAnimType is expected to have four sequential entries.");

				ShaderFlags shaderFlags = renElement.material->getShader()->getFlags();
				bool useForwardRendering = shaderFlags.isSet(ShaderFlag::Forward) || shaderFlags.isSet(ShaderFlag::Transparent);
				
				RenderableAnimType animType = renderable->getAnimType();

				static const ShaderVariation* VAR_LOOKUP[4];
				if(useForwardRendering)
				{
					bool supportsClusteredForward = gRenderBeast()->getFeatureSet() == RenderBeastFeatureSet::Desktop;

					if(supportsClusteredForward)
					{
						VAR_LOOKUP[0] = &getForwardRenderingVariation<false, false, true>();
						VAR_LOOKUP[1] = &getForwardRenderingVariation<true, false, true>();
						VAR_LOOKUP[2] = &getForwardRenderingVariation<false, true, true>();
						VAR_LOOKUP[3] = &getForwardRenderingVariation<true, true, true>();
					}
					else
					{
						VAR_LOOKUP[0] = &getForwardRenderingVariation<false, false, false>();
						VAR_LOOKUP[1] = &getForwardRenderingVariation<true, false, false>();
						VAR_LOOKUP[2] = &getForwardRenderingVariation<false, true, false>();
						VAR_LOOKUP[3] = &getForwardRenderingVariation<true, true, false>();
					}
				}
				else
				{
					VAR_LOOKUP[0] = &getVertexInputVariation<false, false>();
					VAR_LOOKUP[1] = &getVertexInputVariation<true, false>();
					VAR_LOOKUP[2] = &getVertexInputVariation<false, true>();
					VAR_LOOKUP[3] = &getVertexInputVariation<true, true>();
				}

				const ShaderVariation* variation = VAR_LOOKUP[(int)animType];

				FIND_TECHNIQUE_DESC findDesc;
				findDesc.variation = variation;

				UINT32 techniqueIdx = renElement.material->findTechnique(findDesc);

				if (techniqueIdx == (UINT32)-1)
					techniqueIdx = renElement.material->getDefaultTechnique();

				renElement.techniqueIdx = techniqueIdx;

#if BS_DEBUG_MODE
				// Validate mesh <-> shader vertex bindings
				if (renElement.material != nullptr)
				{
					UINT32 numPasses = renElement.material->getNumPasses(techniqueIdx);
					for (UINT32 j = 0; j < numPasses; j++)
					{
						SPtr<Pass> pass = renElement.material->getPass(j, techniqueIdx);
						SPtr<GraphicsPipelineState> graphicsPipeline = pass->getGraphicsPipelineState();

						SPtr<VertexDeclaration> shaderDecl = graphicsPipeline->getVertexProgram()->getInputDeclaration();
						if (!vertexDecl->isCompatible(shaderDecl))
						{
							Vector<VertexElement> missingElements = vertexDecl->getMissingElements(shaderDecl);

							// If using morph shapes ignore POSITION1 and NORMAL1 missing since we assign them from within the renderer
							if (animType == RenderableAnimType::Morph || animType == RenderableAnimType::SkinnedMorph)
							{
								auto removeIter = std::remove_if(missingElements.begin(), missingElements.end(), [](const VertexElement& x)
								{
									return (x.getSemantic() == VES_POSITION && x.getSemanticIdx() == 1) ||
										(x.getSemantic() == VES_NORMAL && x.getSemanticIdx() == 1);
								});

								missingElements.erase(removeIter, missingElements.end());
							}

							if (!missingElements.empty())
							{
								StringStream wrnStream;
								wrnStream << "Provided mesh is missing required vertex attributes to render with the \
									provided shader. Missing elements: " << std::endl;

								for (auto& entry : missingElements)
									wrnStream << "\t" << toString(entry.getSemantic()) << entry.getSemanticIdx() << std::endl;

								LOGWRN(wrnStream.str());
								break;
							}
						}
					}
				}
#endif

				// Generate or assigned renderer specific data for the material
				renElement.params = renElement.material->createParamsSet(techniqueIdx);
				renElement.material->updateParamsSet(renElement.params, 0.0f, true);

				// Generate or assign sampler state overrides
				SamplerOverrideKey samplerKey(renElement.material, techniqueIdx);
				auto iterFind = mSamplerOverrides.find(samplerKey);
				if (iterFind != mSamplerOverrides.end())
				{
					renElement.samplerOverrides = iterFind->second;
					iterFind->second->refCount++;
				}
				else
				{
					SPtr<Shader> shader = renElement.material->getShader();
					MaterialSamplerOverrides* samplerOverrides = SamplerOverrideUtility::generateSamplerOverrides(shader,
						renElement.material->_getInternalParams(), renElement.params, mOptions);

					mSamplerOverrides[samplerKey] = samplerOverrides;

					renElement.samplerOverrides = samplerOverrides;
					samplerOverrides->refCount++;
				}
			}
		}

		// Prepare all parameter bindings
		for(auto& element : rendererRenderable->elements)
		{
			SPtr<Shader> shader = element.material->getShader();
			if (shader == nullptr)
			{
				LOGWRN("Missing shader on material.");
				continue;
			}

			SPtr<GpuParams> gpuParams = element.params->getGpuParams();

			// Note: Perhaps perform buffer validation to ensure expected buffer has the same size and layout as the 
			// provided buffer, and show a warning otherwise. But this is perhaps better handled on a higher level.
			gpuParams->setParamBlockBuffer("PerFrame", mPerFrameParamBuffer);
			gpuParams->setParamBlockBuffer("PerObject", rendererRenderable->perObjectParamBuffer);
			gpuParams->setParamBlockBuffer("PerCall", rendererRenderable->perCallParamBuffer);

			gpuParams->getParamInfo()->getBindings(
				GpuPipelineParamInfoBase::ParamType::ParamBlock,
				"PerCamera",
				element.perCameraBindings
			);

			if (gpuParams->hasBuffer(GPT_VERTEX_PROGRAM, "boneMatrices"))
				gpuParams->setBuffer(GPT_VERTEX_PROGRAM, "boneMatrices", element.boneMatrixBuffer);

			ShaderFlags shaderFlags = shader->getFlags();
			bool useForwardRendering = shaderFlags.isSet(ShaderFlag::Forward) || shaderFlags.isSet(ShaderFlag::Transparent);

			if (useForwardRendering)
			{
				const bool supportsClusteredForward = gRenderBeast()->getFeatureSet() == RenderBeastFeatureSet::Desktop;
				if (supportsClusteredForward)
				{
					gpuParams->getParamInfo()->getBindings(
						GpuPipelineParamInfoBase::ParamType::ParamBlock,
						"GridParams",
						element.gridParamsBindings
					);

					if (gpuParams->hasBuffer(GPT_FRAGMENT_PROGRAM, "gLights"))
						gpuParams->getBufferParam(GPT_FRAGMENT_PROGRAM, "gLights", element.lightsBufferParam);

					if (gpuParams->hasBuffer(GPT_FRAGMENT_PROGRAM, "gGridLightOffsetsAndSize"))
						gpuParams->getBufferParam(GPT_FRAGMENT_PROGRAM, "gGridLightOffsetsAndSize",
							element.gridLightOffsetsAndSizeParam);

					if (gpuParams->hasBuffer(GPT_FRAGMENT_PROGRAM, "gLightIndices"))
						gpuParams->getBufferParam(GPT_FRAGMENT_PROGRAM, "gLightIndices", element.gridLightIndicesParam);

					if (gpuParams->hasBuffer(GPT_FRAGMENT_PROGRAM, "gGridProbeOffsetsAndSize"))
						gpuParams->getBufferParam(GPT_FRAGMENT_PROGRAM, "gGridProbeOffsetsAndSize",
							element.gridProbeOffsetsAndSizeParam);
				}
				else
				{
					gpuParams->getParamInfo()->getBinding(
						GPT_FRAGMENT_PROGRAM,
						GpuPipelineParamInfoBase::ParamType::ParamBlock,
						"Lights",
						element.lightsParamBlockBinding
					);

					gpuParams->getParamInfo()->getBinding(
						GPT_FRAGMENT_PROGRAM,
						GpuPipelineParamInfoBase::ParamType::ParamBlock,
						"LightAndReflProbeParams",
						element.lightAndReflProbeParamsParamBlockBinding
					);
				}

				element.imageBasedParams.populate(gpuParams, GPT_FRAGMENT_PROGRAM, true, supportsClusteredForward,
					supportsClusteredForward);
			}
		}
	}

	void RendererScene::updateRenderable(Renderable* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();

		mInfo.renderables[renderableId]->updatePerObjectBuffer();
		mInfo.renderableCullInfos[renderableId].bounds = renderable->getBounds();
	}

	void RendererScene::unregisterRenderable(Renderable* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();
		Renderable* lastRenerable = mInfo.renderables.back()->renderable;
		UINT32 lastRenderableId = lastRenerable->getRendererId();

		RendererRenderable* rendererRenderable = mInfo.renderables[renderableId];
		Vector<RenderableElement>& elements = rendererRenderable->elements;
		for (auto& element : elements)
		{
			SamplerOverrideKey samplerKey(element.material, element.techniqueIdx);

			auto iterFind = mSamplerOverrides.find(samplerKey);
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
			std::swap(mInfo.renderables[renderableId], mInfo.renderables[lastRenderableId]);
			std::swap(mInfo.renderableCullInfos[renderableId], mInfo.renderableCullInfos[lastRenderableId]);

			lastRenerable->setRendererId(renderableId);
		}

		// Last element is the one we want to erase
		mInfo.renderables.erase(mInfo.renderables.end() - 1);
		mInfo.renderableCullInfos.erase(mInfo.renderableCullInfos.end() - 1);

		bs_delete(rendererRenderable);
	}

	void RendererScene::registerReflectionProbe(ReflectionProbe* probe)
	{
		UINT32 probeId = (UINT32)mInfo.reflProbes.size();
		probe->setRendererId(probeId);

		mInfo.reflProbes.push_back(RendererReflectionProbe(probe));
		RendererReflectionProbe& probeInfo = mInfo.reflProbes.back();

		mInfo.reflProbeWorldBounds.push_back(probe->getBounds());

		// Find a spot in cubemap array
		UINT32 numArrayEntries = (UINT32)mInfo.reflProbeCubemapArrayUsedSlots.size();
		for(UINT32 i = 0; i < numArrayEntries; i++)
		{
			if(!mInfo.reflProbeCubemapArrayUsedSlots[i])
			{
				setReflectionProbeArrayIndex(probeId, i, false);
				mInfo.reflProbeCubemapArrayUsedSlots[i] = true;
				break;
			}
		}

		// No empty slot was found
		if (probeInfo.arrayIdx == (UINT32)-1)
		{
			setReflectionProbeArrayIndex(probeId, numArrayEntries, false);
			mInfo.reflProbeCubemapArrayUsedSlots.push_back(true);
		}

		if(probeInfo.arrayIdx > MaxReflectionCubemaps)
		{
			LOGERR("Reached the maximum number of allowed reflection probe cubemaps at once. "
				"Ignoring reflection probe data.");
		}
	}

	void RendererScene::updateReflectionProbe(ReflectionProbe* probe, bool texture)
	{
		// Should only get called if transform changes, any other major changes and ReflProbeInfo entry gets rebuild
		UINT32 probeId = probe->getRendererId();
		mInfo.reflProbeWorldBounds[probeId] = probe->getBounds();

		if (texture)
		{
			RendererReflectionProbe& probeInfo = mInfo.reflProbes[probeId];
			probeInfo.arrayDirty = true;
		}
	}

	void RendererScene::unregisterReflectionProbe(ReflectionProbe* probe)
	{
		UINT32 probeId = probe->getRendererId();
		UINT32 arrayIdx = mInfo.reflProbes[probeId].arrayIdx;

		if (arrayIdx != (UINT32)-1)
			mInfo.reflProbeCubemapArrayUsedSlots[arrayIdx] = false;

		ReflectionProbe* lastProbe = mInfo.reflProbes.back().probe;
		UINT32 lastProbeId = lastProbe->getRendererId();

		if (probeId != lastProbeId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mInfo.reflProbes[probeId], mInfo.reflProbes[lastProbeId]);
			std::swap(mInfo.reflProbeWorldBounds[probeId], mInfo.reflProbeWorldBounds[lastProbeId]);

			lastProbe->setRendererId(probeId);
		}

		// Last element is the one we want to erase
		mInfo.reflProbes.erase(mInfo.reflProbes.end() - 1);
		mInfo.reflProbeWorldBounds.erase(mInfo.reflProbeWorldBounds.end() - 1);
	}

	void RendererScene::setReflectionProbeArrayIndex(UINT32 probeIdx, UINT32 arrayIdx, bool markAsClean)
	{
		RendererReflectionProbe* probe = &mInfo.reflProbes[probeIdx];
		probe->arrayIdx = arrayIdx;

		if (markAsClean)
			probe->arrayDirty = false;
	}

	void RendererScene::registerLightProbeVolume(LightProbeVolume* volume)
	{
		mInfo.lightProbes.notifyAdded(volume);
	}

	void RendererScene::updateLightProbeVolume(LightProbeVolume* volume)
	{
		mInfo.lightProbes.notifyDirty(volume);
	}

	void RendererScene::unregisterLightProbeVolume(LightProbeVolume* volume)
	{
		mInfo.lightProbes.notifyRemoved(volume);
	}

	void RendererScene::updateLightProbes()
	{
		mInfo.lightProbes.updateProbes();
	}

	void RendererScene::registerSkybox(Skybox* skybox)
	{
		mInfo.skybox = skybox;
	}

	void RendererScene::unregisterSkybox(Skybox* skybox)
	{
		if (mInfo.skybox == skybox)
			mInfo.skybox = nullptr;
	}

	void RendererScene::registerParticleSystem(ParticleSystem* particleSystem)
	{
		const auto rendererId = (UINT32)mInfo.particleSystems.size();
		particleSystem->setRendererId(rendererId);

		mInfo.particleSystems.push_back(RendererParticles());
		mInfo.particleSystemBounds.push_back(AABox());

		RendererParticles& rendererParticles = mInfo.particleSystems.back();
		rendererParticles.particleSystem = particleSystem;

		Matrix4 transform;
		if(particleSystem->getSimulationSpace() == ParticleSimulationSpace::Local)
			transform = particleSystem->getTransform().getMatrix();
		else
			transform = Matrix4::IDENTITY;

		SPtr<GpuParamBlockBuffer> paramBuffer = gParticlesParamDef.createBuffer();
		gParticlesParamDef.gWorldTfrm.set(paramBuffer, transform);

		Vector3 axisForward = particleSystem->getOrientationPlane().normal;

		Vector3 axisUp = Vector3::UNIT_Y;
		if(axisForward.dot(axisUp) > 0.9998f)
			axisUp = Vector3::UNIT_Z;

		Vector3 axisRight = axisUp.cross(axisForward);
		Vector3::orthonormalize(axisRight, axisUp, axisForward);

		gParticlesParamDef.gAxisUp.set(paramBuffer, axisUp);
		gParticlesParamDef.gAxisRight.set(paramBuffer, axisRight);

		rendererParticles.particlesParamBuffer = paramBuffer;

		ParticlesRenderElement& renElement = rendererParticles.renderElement;
		renElement.type = (UINT32)RenderElementType::Particle;

		renElement.material = particleSystem->getMaterial();

		if (renElement.material != nullptr && renElement.material->getShader() == nullptr)
			renElement.material = nullptr;

		// If no material use the default material
		if (renElement.material == nullptr)
			renElement.material = Material::create(DefaultParticlesMat::get()->getShader());

		SpriteTexture* spriteTexture = nullptr;
		if(renElement.material->getShader()->hasTextureParam("gTexture"))
			spriteTexture = renElement.material->getSpriteTexture("gTexture").get();

		if(spriteTexture)
		{
			gParticlesParamDef.gUVOffset.set(paramBuffer, spriteTexture->getOffset());
			gParticlesParamDef.gUVScale.set(paramBuffer, spriteTexture->getScale());

			const SpriteSheetGridAnimation& anim = spriteTexture->getAnimation();
			gParticlesParamDef.gSubImageSize.set(paramBuffer, 
				Vector4((float)anim.numColumns, (float)anim.numRows, 1.0f / anim.numColumns, 1.0f / anim.numRows));
		}
		else
		{
			gParticlesParamDef.gUVOffset.set(paramBuffer, Vector2::ZERO);
			gParticlesParamDef.gUVScale.set(paramBuffer, Vector2::ONE);
			gParticlesParamDef.gSubImageSize.set(paramBuffer, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		const ParticleOrientation orientation = particleSystem->getOrientation();
		const bool lockY = particleSystem->getOrientationLockY();
		const ShaderVariation* variation = &getParticleShaderVariation(orientation, lockY);

		FIND_TECHNIQUE_DESC findDesc;
		findDesc.variation = variation;

		UINT32 techniqueIdx = renElement.material->findTechnique(findDesc);

		if (techniqueIdx == (UINT32)-1)
			techniqueIdx = renElement.material->getDefaultTechnique();

		renElement.techniqueIdx = techniqueIdx;

		// Generate or assigned renderer specific data for the material
		renElement.params = renElement.material->createParamsSet(techniqueIdx);
		renElement.material->updateParamsSet(renElement.params, 0.0f, true);

		SPtr<GpuParams> gpuParams = renElement.params->getGpuParams();

		// Note: Perhaps perform buffer validation to ensure expected buffer has the same size and layout as the 
		// provided buffer, and show a warning otherwise. But this is perhaps better handled on a higher level.
		gpuParams->setParamBlockBuffer("ParticleParams", rendererParticles.particlesParamBuffer);

		gpuParams->getTextureParam(GPT_VERTEX_PROGRAM, "gPositionAndRotTex", renElement.positionAndRotTexture);
		gpuParams->getTextureParam(GPT_VERTEX_PROGRAM, "gColorTex", renElement.colorTexture);
		gpuParams->getTextureParam(GPT_VERTEX_PROGRAM, "gSizeAndFrameIdxTex", renElement.sizeAndFrameIdxTexture);
		gpuParams->getBufferParam(GPT_VERTEX_PROGRAM, "gIndices", renElement.indicesBuffer);

		gpuParams->getParamInfo()->getBindings(
			GpuPipelineParamInfoBase::ParamType::ParamBlock,
			"PerCamera",
			renElement.perCameraBindings
		);

		// TODO - Set up buffers/bindings required for advanced lighting
	}

	void RendererScene::updateParticleSystem(ParticleSystem* particleSystem)
	{
		const UINT32 rendererId = particleSystem->getRendererId();

		SPtr<GpuParamBlockBuffer>& paramBuffer = mInfo.particleSystems[rendererId].particlesParamBuffer;
		gParticlesParamDef.gWorldTfrm.set(paramBuffer, particleSystem->getTransform().getMatrix());
	}

	void RendererScene::unregisterParticleSystem(ParticleSystem* particleSystem)
	{
		const UINT32 rendererId = particleSystem->getRendererId();

		const RendererParticles& lastSystem = mInfo.particleSystems.back();
		const UINT32 lastRendererId = lastSystem.particleSystem->getRendererId();

		if (rendererId != lastRendererId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mInfo.particleSystems[rendererId], mInfo.particleSystems[lastRendererId]);
			std::swap(mInfo.particleSystemBounds[rendererId], mInfo.particleSystemBounds[lastRendererId]);

			particleSystem->setRendererId(rendererId);
		}

		// Last element is the one we want to erase
		mInfo.particleSystems.erase(mInfo.particleSystems.end() - 1);
		mInfo.particleSystemBounds.erase(mInfo.particleSystemBounds.end() - 1);
	}

	void RendererScene::setOptions(const SPtr<RenderBeastOptions>& options)
	{
		mOptions = options;

		for (auto& entry : mInfo.views)
			entry->setStateReductionMode(mOptions->stateReductionMode);
	}

	RENDERER_VIEW_DESC RendererScene::createViewDesc(Camera* camera) const
	{
		SPtr<Viewport> viewport = camera->getViewport();
		ClearFlags clearFlags = viewport->getClearFlags();
		RENDERER_VIEW_DESC viewDesc;

		viewDesc.target.clearFlags = 0;
		if (clearFlags.isSet(ClearFlagBits::Color))
			viewDesc.target.clearFlags |= FBT_COLOR;

		if (clearFlags.isSet(ClearFlagBits::Depth))
			viewDesc.target.clearFlags |= FBT_DEPTH;

		if (clearFlags.isSet(ClearFlagBits::Stencil))
			viewDesc.target.clearFlags |= FBT_STENCIL;

		viewDesc.target.clearColor = viewport->getClearColorValue();
		viewDesc.target.clearDepthValue = viewport->getClearDepthValue();
		viewDesc.target.clearStencilValue = viewport->getClearStencilValue();

		viewDesc.target.target = viewport->getTarget();
		viewDesc.target.nrmViewRect = viewport->getArea();
		viewDesc.target.viewRect = viewport->getPixelArea();

		if (viewDesc.target.target != nullptr)
		{
			viewDesc.target.targetWidth = viewDesc.target.target->getProperties().width;
			viewDesc.target.targetHeight = viewDesc.target.target->getProperties().height;
		}
		else
		{
			viewDesc.target.targetWidth = 0;
			viewDesc.target.targetHeight = 0;
		}

		viewDesc.target.numSamples = camera->getMSAACount();

		viewDesc.triggerCallbacks = true;
		viewDesc.runPostProcessing = true;
		viewDesc.capturingReflections = false;

		viewDesc.cullFrustum = camera->getWorldFrustum();
		viewDesc.visibleLayers = camera->getLayers();
		viewDesc.nearPlane = camera->getNearClipDistance();
		viewDesc.farPlane = camera->getFarClipDistance();
		viewDesc.flipView = false;

		const Transform& tfrm = camera->getTransform();
		viewDesc.viewOrigin = tfrm.getPosition();
		viewDesc.viewDirection = tfrm.getForward();
		viewDesc.projTransform = camera->getProjectionMatrixRS();
		viewDesc.viewTransform = camera->getViewMatrix();
		viewDesc.projType = camera->getProjectionType();

		viewDesc.stateReduction = mOptions->stateReductionMode;
		viewDesc.sceneCamera = camera;

		return viewDesc;
	}

	void RendererScene::updateCameraRenderTargets(Camera* camera, bool remove)
	{
		SPtr<RenderTarget> renderTarget = camera->getViewport()->getTarget();

		// Remove from render target list
		int rtChanged = 0; // 0 - No RT, 1 - RT found, 2 - RT changed
		for (auto iterTarget = mInfo.renderTargets.begin(); iterTarget != mInfo.renderTargets.end(); ++iterTarget)
		{
			RendererRenderTarget& target = *iterTarget;
			for (auto iterCam = target.cameras.begin(); iterCam != target.cameras.end(); ++iterCam)
			{
				if (camera == *iterCam)
				{
					if(remove)
					{
						target.cameras.erase(iterCam);
						rtChanged = 1;
						
					}
					else
					{
						if (renderTarget != target.target)
						{
							target.cameras.erase(iterCam);
							rtChanged = 2;

						}
						else
							rtChanged = 1;
					}

					break;
				}
			}

			if (target.cameras.empty())
			{
				mInfo.renderTargets.erase(iterTarget);
				break;
			}
		}

		// Register in render target list
		if (renderTarget != nullptr && !remove && (rtChanged == 0 || rtChanged == 2))
		{
			auto findIter = std::find_if(mInfo.renderTargets.begin(), mInfo.renderTargets.end(),
				[&](const RendererRenderTarget& x) { return x.target == renderTarget; });

			if (findIter != mInfo.renderTargets.end())
			{
				findIter->cameras.push_back(camera);
			}
			else
			{
				mInfo.renderTargets.push_back(RendererRenderTarget());
				RendererRenderTarget& renderTargetData = mInfo.renderTargets.back();

				renderTargetData.target = renderTarget;
				renderTargetData.cameras.push_back(camera);
			}

			// Sort render targets based on priority
			auto cameraComparer = [&](const Camera* a, const Camera* b) { return a->getPriority() > b->getPriority(); };
			auto renderTargetInfoComparer = [&](const RendererRenderTarget& a, const RendererRenderTarget& b)
			{ return a.target->getProperties().priority > b.target->getProperties().priority; };
			std::sort(begin(mInfo.renderTargets), end(mInfo.renderTargets), renderTargetInfoComparer);

			for (auto& camerasPerTarget : mInfo.renderTargets)
			{
				Vector<Camera*>& cameras = camerasPerTarget.cameras;

				std::sort(begin(cameras), end(cameras), cameraComparer);
			}
		}
	}

	void RendererScene::refreshSamplerOverrides(bool force)
	{
		bool anyDirty = false;
		for (auto& entry : mSamplerOverrides)
		{
			SPtr<MaterialParams> materialParams = entry.first.material->_getInternalParams();

			MaterialSamplerOverrides* materialOverrides = entry.second;
			for(UINT32 i = 0; i < materialOverrides->numOverrides; i++)
			{
				SamplerOverride& override = materialOverrides->overrides[i];
				const MaterialParamsBase::ParamData* materialParamData = materialParams->getParamData(override.paramIdx);

				SPtr<SamplerState> samplerState;
				materialParams->getSamplerState(*materialParamData, samplerState);

				UINT64 hash = 0;
				if (samplerState != nullptr)
					hash = samplerState->getProperties().getHash();

				if (hash != override.originalStateHash || force)
				{
					if (samplerState != nullptr)
						override.state = SamplerOverrideUtility::generateSamplerOverride(samplerState, mOptions);
					else
						override.state = SamplerOverrideUtility::generateSamplerOverride(SamplerState::getDefault(), mOptions);

					override.originalStateHash = override.state->getProperties().getHash();
					materialOverrides->isDirty = true;
				}

				// Dirty flag can also be set externally, so check here even though we assign it above
				if (materialOverrides->isDirty)
					anyDirty = true;
			}
		}

		// Early exit if possible
		if (!anyDirty)
			return;

		UINT32 numRenderables = (UINT32)mInfo.renderables.size();
		for (UINT32 i = 0; i < numRenderables; i++)
		{
			for(auto& element : mInfo.renderables[i]->elements)
			{
				MaterialSamplerOverrides* overrides = element.samplerOverrides;
				if(overrides != nullptr && overrides->isDirty)
				{
					UINT32 numPasses = element.material->getNumPasses();
					for(UINT32 j = 0; j < numPasses; j++)
					{
						SPtr<GpuParams> params = element.params->getGpuParams(j);

						const UINT32 numStages = 6;
						for (UINT32 k = 0; k < numStages; k++)
						{
							GpuProgramType type = (GpuProgramType)k;

							SPtr<GpuParamDesc> paramDesc = params->getParamDesc(type);
							if (paramDesc == nullptr)
								continue;

							for (auto& samplerDesc : paramDesc->samplers)
							{
								UINT32 set = samplerDesc.second.set;
								UINT32 slot = samplerDesc.second.slot;

								UINT32 overrideIndex = overrides->passes[j].stateOverrides[set][slot];
								if (overrideIndex == (UINT32)-1)
									continue;

								params->setSamplerState(set, slot, overrides->overrides[overrideIndex].state);
							}
						}
					}
				}
			}
		}

		for (auto& entry : mSamplerOverrides)
			entry.second->isDirty = false;
	}

	void RendererScene::setParamFrameParams(float time)
	{
		gPerFrameParamDef.gTime.set(mPerFrameParamBuffer, time);
	}

	void RendererScene::prepareRenderable(UINT32 idx, const FrameInfo& frameInfo)
	{
		if (mInfo.renderableReady[idx])
			return;
		
		// Note: Before uploading bone matrices perhaps check if they has actually been changed since last frame
		if(frameInfo.perFrameData.animation != nullptr)
			mInfo.renderables[idx]->renderable->updateAnimationBuffers(*frameInfo.perFrameData.animation);
		
		// Note: Could this step be moved in notifyRenderableUpdated, so it only triggers when material actually gets
		// changed? Although it shouldn't matter much because if the internal versions keeping track of dirty params.
		for (auto& element : mInfo.renderables[idx]->elements)
			element.material->updateParamsSet(element.params, element.materialAnimationTime);
		
		mInfo.renderables[idx]->perObjectParamBuffer->flushToGPU();
		mInfo.renderableReady[idx] = true;
	}

	void RendererScene::updateParticleSystemBounds(const ParticleRenderDataGroup* particleRenderData)
	{
		// Note: Avoid updating bounds for deterministic particle systems every frame. Also see if this can be copied
		// over in a faster way (or ideally just assigned)

		for(auto& entry : mInfo.particleSystems)
		{
			const auto iterFind = particleRenderData->data.find(entry.particleSystem->getId());
			if(iterFind == particleRenderData->data.end())
				continue;

			const UINT32 rendererId = entry.particleSystem->getRendererId();

			AABox worldBounds = iterFind->second->bounds;
			worldBounds.transformAffine(entry.particleSystem->getTransform().getMatrix());

			mInfo.particleSystemBounds[rendererId] = worldBounds;
		}
	}
}}
