//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsObjectRendering.h"
#include "Material/BsShader.h"
#include "RenderAPI/BsGpuParams.h"
#include "BsRenderBeast.h"
#include "Material/BsMaterial.h"
#include "Mesh/BsMesh.h"
#include "Animation/BsSkeleton.h"
#include "RenderAPI/BsGpuBuffer.h"
#include "Material/BsGpuParamsSet.h"
#include "Animation/BsMorphShapes.h"
#include "Animation/BsAnimationManager.h"

namespace bs { namespace ct
{
	PerFrameParamDef gPerFrameParamDef;

	ObjectRenderer::ObjectRenderer()
	{
		mPerFrameParamBuffer = gPerFrameParamDef.createBuffer();
	}

	void ObjectRenderer::initElement(RendererObject& owner, BeastRenderableElement& element)
	{
		SPtr<Shader> shader = element.material->getShader();
		if (shader == nullptr)
		{
			LOGWRN("Missing shader on material.");
			return;
		}

		SPtr<GpuParams> gpuParams = element.params->getGpuParams();

		// Note: Perhaps perform buffer validation to ensure expected buffer has the same size and layout as the provided
		// buffer, and show a warning otherwise. But this is perhaps better handled on a higher level.
		gpuParams->setParamBlockBuffer("PerFrame", mPerFrameParamBuffer);
		gpuParams->setParamBlockBuffer("PerObject", owner.perObjectParamBuffer);
		gpuParams->setParamBlockBuffer("PerCall", owner.perCallParamBuffer);

		gpuParams->getParamInfo()->getBindings(
			GpuPipelineParamInfoBase::ParamType::ParamBlock, 
			"PerCamera",
			element.perCameraBindings
		);

		if (gpuParams->hasBuffer(GPT_VERTEX_PROGRAM, "boneMatrices"))
			gpuParams->setBuffer(GPT_VERTEX_PROGRAM, "boneMatrices", element.boneMatrixBuffer);

		const bool isTransparent = (shader->getFlags() & (UINT32)ShaderFlags::Transparent) != 0;
		const bool usesForwardRendering = isTransparent;

		if(usesForwardRendering)
		{
			const bool supportsClusteredForward = gRenderBeast()->getFeatureSet() == RenderBeastFeatureSet::Desktop;
			if(supportsClusteredForward)
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

	void ObjectRenderer::setParamFrameParams(float time)
	{
		gPerFrameParamDef.gTime.set(mPerFrameParamBuffer, time);
	}
}}