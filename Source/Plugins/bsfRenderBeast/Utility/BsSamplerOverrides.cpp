//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsSamplerOverrides.h"
#include "BsRenderBeastOptions.h"
#include "Material/BsMaterial.h"
#include "RenderAPI/BsGpuParams.h"
#include "Material/BsGpuParamsSet.h"
#include "RenderAPI/BsGpuParamDesc.h"
#include "Material/BsMaterialParams.h"
#include "RenderAPI/BsSamplerState.h"
#include "Managers/BsRenderStateManager.h"

namespace bs { namespace ct
{
	MaterialSamplerOverrides* SamplerOverrideUtility::generateSamplerOverrides(const SPtr<Shader>& shader,
		const SPtr<MaterialParams>& params, const SPtr<GpuParamsSet>& paramsSet,
		const SPtr<RenderBeastOptions>& options)
	{
		MaterialSamplerOverrides* output = nullptr;

		if (shader == nullptr)
			return nullptr;

		bs_frame_mark();
		{
			// Generate a list of all sampler state overrides
			FrameUnorderedMap<String, UINT32> overrideLookup;
			Vector<SamplerOverride> overrides;

			auto& samplerParams = shader->getSamplerParams();
			for(auto& samplerParam : samplerParams)
			{
				UINT32 paramIdx;
				auto result = params->getParamIndex(samplerParam.first, MaterialParams::ParamType::Sampler, GPDT_UNKNOWN,
					0, paramIdx);

				// Parameter shouldn't be in the valid parameter list if it cannot be found
				assert(result == MaterialParams::GetParamResult::Success);
				const MaterialParamsBase::ParamData* materialParamData = params->getParamData(paramIdx);

				UINT32 overrideIdx = (UINT32)overrides.size();
				overrides.push_back(SamplerOverride());
				SamplerOverride& override = overrides.back();

				SPtr<SamplerState> samplerState;
				params->getSamplerState(*materialParamData, samplerState);

				if (samplerState == nullptr)
					samplerState = SamplerState::getDefault();

				override.paramIdx = paramIdx;
				
				if (checkNeedsOverride(samplerState, options))
					override.state = generateSamplerOverride(samplerState, options);
				else
					override.state = samplerState;

				override.originalStateHash = override.state->getProperties().getHash();

				for (auto& entry : samplerParam.second.gpuVariableNames)
					overrideLookup[entry] = overrideIdx;
			}

			UINT32 numPasses = paramsSet->getNumPasses();

			// First pass just determine if we even need to override and count the number of sampler states
			UINT32* numSetsPerPass = (UINT32*)bs_stack_alloc<UINT32>(numPasses);
			memset(numSetsPerPass, 0, sizeof(UINT32) * numPasses);

			UINT32 totalNumSets = 0;
			for (UINT32 i = 0; i < numPasses; i++)
			{
				UINT32 maxSamplerSet = 0;

				SPtr<GpuParams> paramsPtr = paramsSet->getGpuParams(i);
				for (UINT32 j = 0; j < GpuParamsSet::NUM_STAGES; j++)
				{
					GpuProgramType progType = (GpuProgramType)j;
					SPtr<GpuParamDesc> paramDesc = paramsPtr->getParamDesc(progType);
					if (paramDesc == nullptr)
						continue;

					for (auto iter = paramDesc->samplers.begin(); iter != paramDesc->samplers.end(); ++iter)
					{
						UINT32 set = iter->second.set;
						maxSamplerSet = std::max(maxSamplerSet, set + 1);
					}
				}

				numSetsPerPass[i] = maxSamplerSet;
				totalNumSets += maxSamplerSet;
			}
			
			UINT32 totalNumSamplerStates = 0;
			UINT32* slotsPerSet = (UINT32*)bs_stack_alloc<UINT32>(totalNumSets);
			memset(slotsPerSet, 0, sizeof(UINT32) * totalNumSets);

			UINT32* slotsPerSetIter = slotsPerSet;
			for (UINT32 i = 0; i < numPasses; i++)
			{
				SPtr<GpuParams> paramsPtr = paramsSet->getGpuParams(i);
				for (UINT32 j = 0; j < GpuParamsSet::NUM_STAGES; j++)
				{
					GpuProgramType progType = (GpuProgramType)j;
					SPtr<GpuParamDesc> paramDesc = paramsPtr->getParamDesc(progType);
					if (paramDesc == nullptr)
						continue;

					for (auto iter = paramDesc->samplers.begin(); iter != paramDesc->samplers.end(); ++iter)
					{
						UINT32 set = iter->second.set;
						UINT32 slot = iter->second.slot;
						slotsPerSetIter[set] = std::max(slotsPerSetIter[set], slot + 1);
					}
				}

				for(UINT32 j = 0; j < numSetsPerPass[i]; j++)
					totalNumSamplerStates += slotsPerSetIter[j];

				slotsPerSetIter += numSetsPerPass[i];
			}

			UINT32 outputSize = sizeof(MaterialSamplerOverrides) +
				numPasses * sizeof(PassSamplerOverrides) +
				totalNumSets * sizeof(UINT32*) +
				totalNumSamplerStates * sizeof(UINT32) +
				(UINT32)overrides.size() * sizeof(SamplerOverride);

			UINT8* outputData = (UINT8*)bs_alloc(outputSize);
			output = (MaterialSamplerOverrides*)outputData;
			outputData += sizeof(MaterialSamplerOverrides);

			output->refCount = 0;
			output->numPasses = numPasses;
			output->passes = (PassSamplerOverrides*)outputData;
			output->isDirty = true;
			outputData += sizeof(PassSamplerOverrides) * numPasses;

			slotsPerSetIter = slotsPerSet;
			for (UINT32 i = 0; i < numPasses; i++)
			{
				SPtr<GpuParams> paramsPtr = paramsSet->getGpuParams(i);

				PassSamplerOverrides& passOverrides = output->passes[i];
				passOverrides.numSets = numSetsPerPass[i];
				passOverrides.stateOverrides = (UINT32**)outputData;
				outputData += sizeof(UINT32*) * passOverrides.numSets;

				for (UINT32 j = 0; j < passOverrides.numSets; j++)
				{
					passOverrides.stateOverrides[j] = (UINT32*)outputData;
					outputData += sizeof(UINT32) * slotsPerSetIter[j];
				}

				for (UINT32 j = 0; j < GpuParamsSet::NUM_STAGES; j++)
				{
					GpuProgramType progType = (GpuProgramType)j;
					SPtr<GpuParamDesc> paramDesc = paramsPtr->getParamDesc(progType);
					if (paramDesc == nullptr)
						continue;

					UINT32 numStates = 0;
					for (auto iter = paramDesc->samplers.begin(); iter != paramDesc->samplers.end(); ++iter)
					{
						UINT32 set = iter->second.set;
						UINT32 slot = iter->second.slot;
						while (slot > numStates)
						{
							passOverrides.stateOverrides[set][numStates] = (UINT32)-1;
							numStates++;
						}

						numStates = std::max(numStates, slot + 1);

						auto iterFind = overrideLookup.find(iter->first);
						if (iterFind != overrideLookup.end())
							passOverrides.stateOverrides[set][slot] = iterFind->second;
						else
							passOverrides.stateOverrides[set][slot] = (UINT32)-1;
					}
				}

				slotsPerSetIter += passOverrides.numSets;
			}

			output->numOverrides = (UINT32)overrides.size();
			output->overrides = (SamplerOverride*)outputData;

			for(UINT32 i = 0; i < output->numOverrides; i++)
			{
				new (&output->overrides[i].state) SPtr<SamplerState>();
				output->overrides[i] = overrides[i];
			}

			bs_stack_free(slotsPerSet);
			bs_stack_free(numSetsPerPass);
		}
		bs_frame_clear();

		return output;
	}

	void SamplerOverrideUtility::destroySamplerOverrides(MaterialSamplerOverrides* overrides)
	{
		if (overrides != nullptr)
		{
			for (UINT32 i = 0; i < overrides->numOverrides; i++)
				overrides->overrides[i].state.~SPtr<SamplerState>();

			bs_free(overrides);
			overrides = nullptr;
		}
	}

	bool SamplerOverrideUtility::checkNeedsOverride(const SPtr<SamplerState>& samplerState, const SPtr<RenderBeastOptions>& options)
	{
		const SamplerProperties& props = samplerState->getProperties();

		switch (options->filtering)
		{
		case RenderBeastFiltering::Bilinear:
		{
			if (props.getTextureFiltering(FT_MIN) != FO_LINEAR)
				return true;

			if (props.getTextureFiltering(FT_MAG) != FO_LINEAR)
				return true;

			if (props.getTextureFiltering(FT_MIP) != FO_POINT)
				return true;
		}
			break;
		case RenderBeastFiltering::Trilinear:
		{
			if (props.getTextureFiltering(FT_MIN) != FO_LINEAR)
				return true;

			if (props.getTextureFiltering(FT_MAG) != FO_LINEAR)
				return true;

			if (props.getTextureFiltering(FT_MIP) != FO_LINEAR)
				return true;
		}
			break;
		case RenderBeastFiltering::Anisotropic:
		{
			if (props.getTextureFiltering(FT_MIN) != FO_ANISOTROPIC)
				return true;

			if (props.getTextureFiltering(FT_MAG) != FO_ANISOTROPIC)
				return true;

			if (props.getTextureFiltering(FT_MIP) != FO_ANISOTROPIC)
				return true;

			if (props.getTextureAnisotropy() != options->anisotropyMax)
				return true;
		}
			break;
		}

		return false;
	}

	SPtr<SamplerState> SamplerOverrideUtility::generateSamplerOverride(const SPtr<SamplerState>& samplerState, const SPtr<RenderBeastOptions>& options)
	{
		const SamplerProperties& props = samplerState->getProperties();
		SAMPLER_STATE_DESC desc = props.getDesc();

		switch (options->filtering)
		{
		case RenderBeastFiltering::Bilinear:
			desc.minFilter = FO_LINEAR;
			desc.magFilter = FO_LINEAR;
			desc.mipFilter = FO_POINT;
			break;
		case RenderBeastFiltering::Trilinear:
			desc.minFilter = FO_LINEAR;
			desc.magFilter = FO_LINEAR;
			desc.mipFilter = FO_LINEAR;
			break;
		case RenderBeastFiltering::Anisotropic:
			desc.minFilter = FO_ANISOTROPIC;
			desc.magFilter = FO_ANISOTROPIC;
			desc.mipFilter = FO_ANISOTROPIC;
			break;
		}

		desc.maxAniso = options->anisotropyMax;

		return RenderStateManager::instance().createSamplerState(desc);
	}
}}
