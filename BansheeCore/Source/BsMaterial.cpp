#include "BsMaterial.h"
#include "BsException.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsRenderSystem.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuProgram.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsGpuParamDesc.h"
#include "BsMaterialRTTI.h"
#include "BsMaterialManager.h"
#include "BsDebug.h"
#include "BsResources.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	struct ShaderBlockDesc
	{
		String name;
		GpuParamBlockUsage usage;
		int size;
		bool create;
	};

	SPtr<PassParametersCore> convertParamsToCore(const SPtr<PassParameters>& passParams)
	{
		SPtr<PassParametersCore> passParameters = bs_shared_ptr<PassParametersCore>();

		if (passParams->mVertParams != nullptr)
			passParameters->mVertParams = passParams->mVertParams->getCore();
		else
			passParameters->mVertParams = nullptr;

		if (passParams->mFragParams != nullptr)
			passParameters->mFragParams = passParams->mFragParams->getCore();
		else
			passParameters->mFragParams = nullptr;

		if (passParams->mGeomParams != nullptr)
			passParameters->mGeomParams = passParams->mGeomParams->getCore();
		else
			passParameters->mGeomParams = nullptr;

		if (passParams->mHullParams != nullptr)
			passParameters->mHullParams = passParams->mHullParams->getCore();
		else
			passParameters->mHullParams = nullptr;

		if (passParams->mDomainParams != nullptr)
			passParameters->mDomainParams = passParams->mDomainParams->getCore();
		else
			passParameters->mDomainParams = nullptr;

		if (passParams->mComputeParams != nullptr)
			passParameters->mComputeParams = passParams->mComputeParams->getCore();
		else
			passParameters->mComputeParams = nullptr;

		return passParameters;
	}

	bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets)
	{
		bool equal = paramA.arraySize == paramB.arraySize && paramA.elementSize == paramB.elementSize
			&& paramA.type == paramB.type && paramA.arrayElementStride == paramB.arrayElementStride;

		if (!ignoreBufferOffsets)
			equal &= paramA.cpuMemOffset == paramB.cpuMemOffset && paramA.gpuMemOffset == paramB.gpuMemOffset;

		return equal;
	}

	Map<String, const GpuParamDataDesc*> determineValidDataParameters(const Vector<GpuParamDescPtr>& paramDescs)
	{
		Map<String, const GpuParamDataDesc*> foundDataParams;
		Map<String, bool> validParams;

		for (auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;

			// Check regular data params
			for (auto iter2 = curDesc.params.begin(); iter2 != curDesc.params.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamDataDesc& curParam = iter2->second;

				auto dataFindIter = validParams.find(iter2->first);
				if (dataFindIter == validParams.end())
				{
					validParams[iter2->first] = true;
					foundDataParams[iter2->first] = &curParam;
				}
				else
				{
					if (validParams[iter2->first])
					{
						auto dataFindIter2 = foundDataParams.find(iter2->first);

						const GpuParamDataDesc* otherParam = dataFindIter2->second;
						if (!areParamsEqual(curParam, *otherParam, true))
						{
							validParams[iter2->first] = false;
							foundDataParams.erase(dataFindIter2);
						}
					}
				}
			}
		}

		return foundDataParams;
	}

	Vector<const GpuParamObjectDesc*> determineValidObjectParameters(const Vector<GpuParamDescPtr>& paramDescs)
	{
		Vector<const GpuParamObjectDesc*> validParams;

		for (auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;

			// Check sampler params
			for (auto iter2 = curDesc.samplers.begin(); iter2 != curDesc.samplers.end(); ++iter2)
			{
				validParams.push_back(&iter2->second);
			}

			// Check texture params
			for (auto iter2 = curDesc.textures.begin(); iter2 != curDesc.textures.end(); ++iter2)
			{
				validParams.push_back(&iter2->second);
			}

			// Check buffer params
			for (auto iter2 = curDesc.buffers.begin(); iter2 != curDesc.buffers.end(); ++iter2)
			{
				validParams.push_back(&iter2->second);
			}
		}

		return validParams;
	}

	Set<String> determineValidShareableParamBlocks(const Vector<GpuParamDescPtr>& paramDescs)
	{
		// Make sure param blocks with the same name actually are the same
		Map<String, std::pair<String, GpuParamDescPtr>> uniqueParamBlocks;
		Map<String, bool> validParamBlocks;

		for (auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;
			for (auto blockIter = curDesc.paramBlocks.begin(); blockIter != curDesc.paramBlocks.end(); ++blockIter)
			{
				bool isBlockValid = true;
				const GpuParamBlockDesc& curBlock = blockIter->second;

				if (!curBlock.isShareable) // Non-shareable buffers are handled differently, they're allowed same names
					continue;

				auto iterFind = uniqueParamBlocks.find(blockIter->first);
				if (iterFind == uniqueParamBlocks.end())
				{
					uniqueParamBlocks[blockIter->first] = std::make_pair(blockIter->first, *iter);
					validParamBlocks[blockIter->first] = true;
					continue;
				}

				String otherBlockName = iterFind->second.first;
				GpuParamDescPtr otherDesc = iterFind->second.second;

				for (auto myParamIter = curDesc.params.begin(); myParamIter != curDesc.params.end(); ++myParamIter)
				{
					const GpuParamDataDesc& myParam = myParamIter->second;

					if (myParam.paramBlockSlot != curBlock.slot)
						continue; // Param is in another block, so we will check it when its time for that block

					auto otherParamFind = otherDesc->params.find(myParamIter->first);

					// Cannot find other param, blocks aren't equal
					if (otherParamFind == otherDesc->params.end())
					{
						isBlockValid = false;
						break;
					}

					const GpuParamDataDesc& otherParam = otherParamFind->second;

					if (!areParamsEqual(myParam, otherParam, false) || curBlock.name != otherBlockName)
					{
						isBlockValid = false;
						break;
					}
				}

				if (!isBlockValid)
				{
					if (validParamBlocks[blockIter->first])
					{
						LOGWRN("Found two param blocks with the same name but different contents: " + blockIter->first);
						validParamBlocks[blockIter->first] = false;
					}
				}
			}
		}

		Set<String> validParamBlocksReturn;
		for (auto iter = validParamBlocks.begin(); iter != validParamBlocks.end(); ++iter)
		{
			if (iter->second)
				validParamBlocksReturn.insert(iter->first);
		}

		return validParamBlocksReturn;
	}

	Map<String, String> determineParameterToBlockMapping(const Vector<GpuParamDescPtr>& paramDescs)
	{
		Map<String, String> paramToParamBlock;

		for (auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;
			for (auto iter2 = curDesc.params.begin(); iter2 != curDesc.params.end(); ++iter2)
			{
				const GpuParamDataDesc& curParam = iter2->second;

				auto iterFind = paramToParamBlock.find(curParam.name);
				if (iterFind != paramToParamBlock.end())
					continue;

				for (auto iterBlock = curDesc.paramBlocks.begin(); iterBlock != curDesc.paramBlocks.end(); ++iterBlock)
				{
					if (iterBlock->second.slot == curParam.paramBlockSlot)
					{
						paramToParamBlock[curParam.name] = iterBlock->second.name;
						break;
					}
				}
			}
		}

		return paramToParamBlock;
	}

	Map<String, String> determineParamMappings(const Vector<GpuParamDescPtr>& paramDescs, const Map<String, SHADER_DATA_PARAM_DESC>& dataParams,
		const Map<String, SHADER_OBJECT_PARAM_DESC>& objectParam)
	{
		Map<String, String> validParams;

		Map<String, const GpuParamDataDesc*> validDataParameters = determineValidDataParameters(paramDescs);
		Vector<const GpuParamObjectDesc*> validObjectParameters = determineValidObjectParameters(paramDescs);
		Map<String, String> paramToParamBlockMap = determineParameterToBlockMapping(paramDescs);

		// Create data param mappings
		for (auto iter = dataParams.begin(); iter != dataParams.end(); ++iter)
		{
			auto findIter = validDataParameters.find(iter->second.gpuVariableName);

			// Not valid so we skip it
			if (findIter == validDataParameters.end())
				continue;

			if (findIter->second->type != iter->second.type)
			{
				LOGWRN("Ignoring shader parameter \"" + iter->first + "\". Type doesn't match the one defined in the gpu program. "
					+ "Shader defined type: " + toString(iter->second.type) + " - Gpu program defined type: " + toString(findIter->second->type));
				continue;
			}

			if (findIter->second->arraySize != iter->second.arraySize)
			{
				LOGWRN("Ignoring shader parameter \"" + iter->first + "\". Array size doesn't match the one defined in the gpu program."
					+ "Shader defined array size: " + toString(iter->second.arraySize) + " - Gpu program defined array size: " + toString(findIter->second->arraySize));
				continue;
			}

			auto findBlockIter = paramToParamBlockMap.find(iter->second.gpuVariableName);

			if (findBlockIter == paramToParamBlockMap.end())
				BS_EXCEPT(InternalErrorException, "Parameter doesn't exist in param to param block map but exists in valid param map.");

			String& paramBlockName = findBlockIter->second;
			validParams[iter->first] = iter->second.gpuVariableName;
		}

		// Create object param mappings
		for (auto iter = objectParam.begin(); iter != objectParam.end(); ++iter)
		{
			const Vector<String>& gpuVariableNames = iter->second.gpuVariableNames;
			for (auto iter2 = gpuVariableNames.begin(); iter2 != gpuVariableNames.end(); ++iter2)
			{
				for (auto iter3 = validObjectParameters.begin(); iter3 != validObjectParameters.end(); ++iter3)
				{
					if ((*iter3)->name == (*iter2) && (*iter3)->type == iter->second.type)
					{
						validParams[iter->first] = *iter2;
						break;
					}
				}
			}
		}

		return validParams;
	}

	Vector<ShaderBlockDesc> determineShaderBlockData(const Set<String>& paramBlocks, const Vector<GpuParamDescPtr>& paramDescs,
		const Map<String, SHADER_PARAM_BLOCK_DESC>& shaderDesc)
	{
		Vector<ShaderBlockDesc> output;
		for (auto iter = paramBlocks.begin(); iter != paramBlocks.end(); ++iter)
		{
			ShaderBlockDesc shaderBlockDesc;
			shaderBlockDesc.create = true;
			shaderBlockDesc.usage = GPBU_STATIC;
			shaderBlockDesc.size = 0;
			shaderBlockDesc.name = *iter;

			auto iterFind = shaderDesc.find(*iter);
			if (iterFind != shaderDesc.end())
			{
				shaderBlockDesc.create = !iterFind->second.shared && iterFind->second.rendererSemantic == 0;
				shaderBlockDesc.usage = iterFind->second.usage;
			}

			for (auto iter2 = paramDescs.begin(); iter2 != paramDescs.end(); ++iter2)
			{
				auto findParamBlockDesc = (*iter2)->paramBlocks.find(*iter);

				if (findParamBlockDesc != (*iter2)->paramBlocks.end())
				{
					shaderBlockDesc.size = findParamBlockDesc->second.blockSize * sizeof(UINT32);
					break;
				}
			}

			output.push_back(shaderBlockDesc);
		}

		return output;
	}

	Vector<GpuParamDescPtr> MaterialBase::getAllParamDescs(const SPtr<Technique>& technique)
	{
		Vector<GpuParamDescPtr> allParamDescs;

		// Make sure all gpu programs are fully loaded
		for (UINT32 i = 0; i < technique->getNumPasses(); i++)
		{
			PassPtr curPass = technique->getPass(i);

			HGpuProgram vertProgram = curPass->getVertexProgram();
			if (vertProgram)
			{
				vertProgram.synchronize();
				allParamDescs.push_back(vertProgram->getParamDesc());
			}

			HGpuProgram fragProgram = curPass->getFragmentProgram();
			if (fragProgram)
			{
				fragProgram.synchronize();
				allParamDescs.push_back(fragProgram->getParamDesc());
			}

			HGpuProgram geomProgram = curPass->getGeometryProgram();
			if (geomProgram)
			{
				geomProgram.synchronize();
				allParamDescs.push_back(geomProgram->getParamDesc());
			}

			HGpuProgram hullProgram = curPass->getHullProgram();
			if (hullProgram)
			{
				hullProgram.synchronize();
				allParamDescs.push_back(hullProgram->getParamDesc());
			}

			HGpuProgram domainProgram = curPass->getDomainProgram();
			if (domainProgram)
			{
				domainProgram.synchronize();
				allParamDescs.push_back(domainProgram->getParamDesc());
			}

			HGpuProgram computeProgram = curPass->getComputeProgram();
			if (computeProgram)
			{
				computeProgram.synchronize();
				allParamDescs.push_back(computeProgram->getParamDesc());
			}
		}

		return allParamDescs;
	}

	Vector<GpuParamDescPtr> MaterialBase::getAllParamDescs(const SPtr<TechniqueCore>& technique)
	{
		Vector<GpuParamDescPtr> allParamDescs;

		// Make sure all gpu programs are fully loaded
		for (UINT32 i = 0; i < technique->getNumPasses(); i++)
		{
			SPtr<PassCore> curPass = technique->getPass(i);

			SPtr<GpuProgramCore> vertProgram = curPass->getVertexProgram();
			if (vertProgram)
				allParamDescs.push_back(vertProgram->getParamDesc());

			SPtr<GpuProgramCore> fragProgram = curPass->getFragmentProgram();
			if (fragProgram)
				allParamDescs.push_back(fragProgram->getParamDesc());

			SPtr<GpuProgramCore> geomProgram = curPass->getGeometryProgram();
			if (geomProgram)
				allParamDescs.push_back(geomProgram->getParamDesc());

			SPtr<GpuProgramCore> hullProgram = curPass->getHullProgram();
			if (hullProgram)
				allParamDescs.push_back(hullProgram->getParamDesc());

			SPtr<GpuProgramCore> domainProgram = curPass->getDomainProgram();
			if (domainProgram)
				allParamDescs.push_back(domainProgram->getParamDesc());

			SPtr<GpuProgramCore> computeProgram = curPass->getComputeProgram();
			if (computeProgram)
				allParamDescs.push_back(computeProgram->getParamDesc());
		}

		return allParamDescs;
	}

	template<bool Core>
	void TMaterial<Core>::setShader(ShaderType shader)
	{
		mShader = shader;

		initBestTechnique();
		_markCoreDirty();
	}

	template<bool Core>
	UINT32 TMaterial<Core>::getNumPasses() const
	{
		throwIfNotInitialized();

		return mShader->getBestTechnique()->getNumPasses();
	}

	template<bool Core>
	typename TMaterial<Core>::PassType TMaterial<Core>::getPass(UINT32 passIdx) const
	{
		if (passIdx < 0 || passIdx >= mShader->getBestTechnique()->getNumPasses())
			BS_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mShader->getBestTechnique()->getPass(passIdx);
	}

	template<bool Core>
	TMaterialParamStruct<Core> TMaterial<Core>::getParamStruct(const String& name) const
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if (iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return TMaterialParamStruct<Core>();
		}

		const String& gpuVarName = iterFind->second;
		Vector<TGpuParamStruct<Core>> gpuParams;

		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasParam(gpuVarName))
					{
						gpuParams.push_back(TGpuParamStruct<Core>());
						paramPtr->getStructParam(gpuVarName, gpuParams.back());
					}
				}
			}
		}

		return TMaterialParamStruct<Core>(gpuParams);
	}

	template<bool Core>
	TMaterialParamTexture<Core> TMaterial<Core>::getParamTexture(const String& name) const
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if (iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return TMaterialParamTexture<Core>();
		}

		const String& gpuVarName = iterFind->second;
		Vector<TGpuParamTexture<Core>> gpuParams;

		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasTexture(gpuVarName))
					{
						gpuParams.push_back(TGpuParamTexture<Core>());
						paramPtr->getTextureParam(gpuVarName, gpuParams.back());
					}
				}
			}
		}

		return TMaterialParamTexture<Core>(gpuParams);
	}

	template<bool Core>
	TMaterialParamLoadStoreTexture<Core> TMaterial<Core>::getParamLoadStoreTexture(const String& name) const
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if (iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return TMaterialParamLoadStoreTexture<Core>();
		}

		const String& gpuVarName = iterFind->second;
		Vector<TGpuParamLoadStoreTexture<Core>> gpuParams;

		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasTexture(gpuVarName))
					{
						gpuParams.push_back(TGpuParamLoadStoreTexture<Core>());
						paramPtr->getLoadStoreTextureParam(gpuVarName, gpuParams.back());
					}
				}
			}
		}

		return TMaterialParamLoadStoreTexture<Core>(gpuParams);
	}

	template<bool Core>
	TMaterialParamSampState<Core> TMaterial<Core>::getParamSamplerState(const String& name) const
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if (iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return TMaterialParamSampState<Core>();
		}

		const String& gpuVarName = iterFind->second;
		Vector<TGpuParamSampState<Core>> gpuParams;
		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasSamplerState(gpuVarName))
					{
						gpuParams.push_back(TGpuParamSampState<Core>());
						paramPtr->getSamplerStateParam(gpuVarName, gpuParams.back());
					}
				}
			}
		}

		return TMaterialParamSampState<Core>(gpuParams);
	}

	template<bool Core>
	void TMaterial<Core>::setParamBlockBuffer(const String& name, const ParamBlockPtrType& paramBlock)
	{
		auto iterFind = mValidShareableParamBlocks.find(name);
		if (iterFind == mValidShareableParamBlocks.end())
		{
			LOGWRN("Material doesn't have a parameter block named " + name);
			return;
		}

		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasParamBlock(name))
						paramPtr->setParamBlockBuffer(name, paramBlock);
				}
			}
		}
	}

	template<bool Core>
	void TMaterial<Core>::initBestTechnique()
	{
		mBestTechnique = nullptr;
		mParametersPerPass.clear();

		if (mShader)
		{
			mBestTechnique = mShader->getBestTechnique();

			if (mBestTechnique == nullptr)
				return;

			mValidShareableParamBlocks.clear();
			Vector<GpuParamDescPtr> allParamDescs = getAllParamDescs(mBestTechnique);

			mValidParams = determineParamMappings(allParamDescs, mShader->getDataParams(), mShader->getObjectParams());

			// Fill out various helper structures
			Set<String> validShareableParamBlocks = determineValidShareableParamBlocks(allParamDescs);
			Vector<ShaderBlockDesc> paramBlockData = determineShaderBlockData(validShareableParamBlocks, allParamDescs, mShader->getParamBlocks());

			Map<String, ParamBlockPtrType> paramBlockBuffers;

			// Create param blocks
			for (auto& paramBlock : paramBlockData)
			{
				ParamBlockPtrType newParamBlockBuffer;
				if (paramBlock.create)
				{
					newParamBlockBuffer = ParamBlockType::create(paramBlock.size, paramBlock.usage);
				}

				paramBlockBuffers[paramBlock.name] = newParamBlockBuffer;
				mValidShareableParamBlocks.insert(paramBlock.name);
			}

			for (UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
			{
				PassType curPass = mBestTechnique->getPass(i);
				SPtr<TPassParameters<Core>> params = SPtr<TPassParameters<Core>>(new TPassParameters<Core>());

				GpuProgramType vertProgram = curPass->getVertexProgram();
				if (vertProgram)
					params->mVertParams = vertProgram->createParameters();

				GpuProgramType fragProgram = curPass->getFragmentProgram();
				if (fragProgram)
					params->mFragParams = fragProgram->createParameters();

				GpuProgramType geomProgram = curPass->getGeometryProgram();
				if (geomProgram)
					params->mGeomParams = geomProgram->createParameters();

				GpuProgramType hullProgram = curPass->getHullProgram();
				if (hullProgram)
					params->mHullParams = hullProgram->createParameters();

				GpuProgramType domainProgram = curPass->getDomainProgram();
				if (domainProgram)
					params->mDomainParams = domainProgram->createParameters();

				GpuProgramType computeProgram = curPass->getComputeProgram();
				if (computeProgram)
					params->mComputeParams = computeProgram->createParameters();

				mParametersPerPass.push_back(params);
			}

			// Assign param block buffers
			for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
			{
				SPtr<TPassParameters<Core>> params = *iter;

				for (UINT32 i = 0; i < params->getNumParams(); i++)
				{
					GpuParamsType& paramPtr = params->getParamByIdx(i);
					if (paramPtr)
					{
						// Assign shareable buffers
						for (auto iterBlock = mValidShareableParamBlocks.begin(); iterBlock != mValidShareableParamBlocks.end(); ++iterBlock)
						{
							const String& paramBlockName = *iterBlock;
							if (paramPtr->hasParamBlock(paramBlockName))
							{
								ParamBlockPtrType blockBuffer = paramBlockBuffers[paramBlockName];

								paramPtr->setParamBlockBuffer(paramBlockName, blockBuffer);
							}
						}

						// Create non-shareable ones
						const GpuParamDesc& desc = paramPtr->getParamDesc();
						for (auto iterBlockDesc = desc.paramBlocks.begin(); iterBlockDesc != desc.paramBlocks.end(); ++iterBlockDesc)
						{
							if (!iterBlockDesc->second.isShareable)
							{
								ParamBlockPtrType newParamBlockBuffer = ParamBlockType::create(iterBlockDesc->second.blockSize * sizeof(UINT32));

								paramPtr->setParamBlockBuffer(iterBlockDesc->first, newParamBlockBuffer);
							}
						}
					}
				}
			}
		}
	}

	template <bool Core>
	template <typename T>
	void TMaterial<Core>::getParam(const String& name, TMaterialDataParam<T, Core>& output) const
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if (iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		const String& gpuVarName = iterFind->second;
		Vector<TGpuDataParam<T, Core>> gpuParams;

		for (auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			SPtr<TPassParameters<Core>> params = *iter;

			for (UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsType& paramPtr = params->getParamByIdx(i);
				if (paramPtr)
				{
					if (paramPtr->hasParam(gpuVarName))
					{
						gpuParams.push_back(TGpuDataParam<T, Core>());
						paramPtr->getParam<T>(gpuVarName, gpuParams.back());
					}
				}
			}
		}

		output = TMaterialDataParam<T, Core>(gpuParams);
	}

	template<bool Core>
	void TMaterial<Core>::throwIfNotInitialized() const
	{
		if (mShader == nullptr)
		{
			BS_EXCEPT(InternalErrorException, "Material does not have shader set.");
		}

		if (mBestTechnique == nullptr)
		{
			BS_EXCEPT(InternalErrorException, "Shader does not contain a supported technique.");
		}
	}

	template class TMaterial < false > ;
	template class TMaterial < true > ;

	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<float, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Color, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Vector2, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Vector3, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Vector4, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Matrix3, false>&) const;
	template BS_CORE_EXPORT void TMaterial<false>::getParam(const String&, TMaterialDataParam<Matrix4, false>&) const;

	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<float, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Color, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Vector2, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Vector3, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Vector4, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Matrix3, true>&) const;
	template BS_CORE_EXPORT void TMaterial<true>::getParam(const String&, TMaterialDataParam<Matrix4, true>&) const;

	MaterialCore::MaterialCore(const SPtr<ShaderCore>& shader)
	{
		setShader(shader);
	}

	MaterialCore::MaterialCore(const SPtr<ShaderCore>& shader, const SPtr<TechniqueCore>& bestTechnique,
		const Set<String>& validShareableParamBlocks, const Map<String, String>& validParams,
		const Vector<SPtr<PassParametersCore>>& passParams)
	{
		mShader = shader;
		mBestTechnique = bestTechnique;
		mValidShareableParamBlocks = validShareableParamBlocks;
		mValidParams = validParams;
		mParametersPerPass = passParams;
	}

	void MaterialCore::syncToCore(const CoreSyncData& data)
	{
		char* dataPtr = (char*)data.getBuffer();

		mValidShareableParamBlocks.clear();
		mValidParams.clear();
		mParametersPerPass.clear();

		UINT32 numPasses = 0;

		dataPtr = rttiReadElem(mValidShareableParamBlocks, dataPtr);
		dataPtr = rttiReadElem(mValidParams, dataPtr);
		dataPtr = rttiReadElem(numPasses, dataPtr);

		for (UINT32 i = 0; i < numPasses; i++)
		{
			SPtr<PassParametersCore>* passParameters = (SPtr<PassParametersCore>*)dataPtr;

			mParametersPerPass.push_back(*passParameters);

			passParameters->~SPtr<PassParametersCore>();
			dataPtr += sizeof(SPtr<PassParametersCore>);
		}

		SPtr<ShaderCore>* shader = (SPtr<ShaderCore>*)dataPtr;
		mShader = *shader;
		shader->~SPtr<ShaderCore>();
		dataPtr += sizeof(SPtr<ShaderCore>);

		SPtr<TechniqueCore>* technique = (SPtr<TechniqueCore>*)dataPtr;
		mBestTechnique = *technique;
		technique->~SPtr<TechniqueCore>();
		dataPtr += sizeof(SPtr<TechniqueCore>);
	}

	Material::Material(const ShaderPtr& shader)
	{
		setShader(shader);
	}

	void Material::_markCoreDirty()
	{
		markCoreDirty();
	}

	SPtr<MaterialCore> Material::getCore() const
	{
		return std::static_pointer_cast<MaterialCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Material::createCore() const
	{
		MaterialCore* material = nullptr;

		SPtr<ShaderCore> shader;
		if (mShader != nullptr)
		{
			shader = mShader->getCore();

			if (mBestTechnique != nullptr)
			{
				SPtr<TechniqueCore> technique = mBestTechnique->getCore();

				Vector<SPtr<PassParametersCore>> passParams;
				for (auto& passParam : mParametersPerPass)
					passParams.push_back(convertParamsToCore(passParam));

				material = new (bs_alloc<MaterialCore>()) MaterialCore(shader, technique, mValidShareableParamBlocks, mValidParams, passParams);
			}
		}
		
		if (material == nullptr)
			material = new (bs_alloc<MaterialCore>()) MaterialCore(shader);

		SPtr<MaterialCore> materialPtr = bs_shared_ptr<MaterialCore, GenAlloc>(material);
		materialPtr->_setThisPtr(materialPtr);

		return materialPtr;
	}

	CoreSyncData Material::syncToCore(FrameAlloc* allocator)
	{
		UINT32 numPasses = (UINT32)mParametersPerPass.size();

		UINT32 size = sizeof(UINT32) + numPasses * sizeof(SPtr<PassParametersCore>)
			+ sizeof(SPtr<ShaderCore>) + sizeof(SPtr<TechniqueCore>) + rttiGetElemSize(mValidShareableParamBlocks)
			+ rttiGetElemSize(mValidParams);

		UINT8* buffer = allocator->alloc(size);
		char* dataPtr = (char*)buffer;
		dataPtr = rttiWriteElem(mValidShareableParamBlocks, dataPtr);
		dataPtr = rttiWriteElem(mValidParams, dataPtr);
		dataPtr = rttiWriteElem(numPasses, dataPtr);

		for (UINT32 i = 0; i < numPasses; i++)
		{
			SPtr<PassParametersCore>* passParameters = new (dataPtr) SPtr<PassParametersCore>();
			*passParameters = convertParamsToCore(mParametersPerPass[i]);

			dataPtr += sizeof(SPtr<PassParametersCore>);
		}

		SPtr<ShaderCore>* shader = new (dataPtr)SPtr<ShaderCore>();
		if (mShader != nullptr)
			*shader = mShader->getCore();
		else
			*shader = nullptr;

		dataPtr += sizeof(SPtr<ShaderCore>);

		SPtr<TechniqueCore>* technique = new (dataPtr)SPtr<TechniqueCore>();
		if (mBestTechnique != nullptr)
			*technique = mBestTechnique->getCore();
		else
			*technique = nullptr;

		dataPtr += sizeof(SPtr<TechniqueCore>);

		return CoreSyncData(buffer, size);
	}

	HMaterial Material::create()
	{
		MaterialPtr materialPtr = MaterialManager::instance().create();

		return static_resource_cast<Material>(gResources()._createResourceHandle(materialPtr));
	}

	HMaterial Material::create(ShaderPtr shader)
	{
		MaterialPtr materialPtr = MaterialManager::instance().create(shader);

		return static_resource_cast<Material>(gResources()._createResourceHandle(materialPtr));
	}

	RTTITypeBase* Material::getRTTIStatic()
	{
		return MaterialRTTI::instance();
	}

	RTTITypeBase* Material::getRTTI() const
	{
		return Material::getRTTIStatic();
	}
}