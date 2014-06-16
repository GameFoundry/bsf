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

namespace BansheeEngine
{
	Material::Material()
		:Resource(false), mCoreDirtyFlags(0xFFFFFFFF)
	{

	}

	Material::~Material()
	{
		
	}

	void Material::setShader(ShaderPtr shader)
	{
		mShader = shader;

		initBestTechnique();
	}

	void Material::initBestTechnique()
	{
		mBestTechnique = nullptr;
		mParametersPerPass.clear();
		freeParamBuffers();

		if(mShader)
		{
			mBestTechnique = mShader->getBestTechnique();

			if(mBestTechnique == nullptr)
				return;

			mValidShareableParamBlocks.clear();
			mValidParams.clear();

			Vector<const GpuParamDesc*> allParamDescs;

			// Make sure all gpu programs are fully loaded
			for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
			{
				PassPtr curPass = mBestTechnique->getPass(i);

				HGpuProgram vertProgram = curPass->getVertexProgram();
				if(vertProgram)
				{
					vertProgram.synchronize();
					allParamDescs.push_back(&vertProgram->getParamDesc());
				}

				HGpuProgram fragProgram = curPass->getFragmentProgram();
				if(fragProgram)
				{
					fragProgram.synchronize();
					allParamDescs.push_back(&fragProgram->getParamDesc());
				}

				HGpuProgram geomProgram = curPass->getGeometryProgram();
				if(geomProgram)
				{
					geomProgram.synchronize();
					allParamDescs.push_back(&geomProgram->getParamDesc());
				}

				HGpuProgram hullProgram = curPass->getHullProgram();
				if(hullProgram)
				{
					hullProgram.synchronize();
					allParamDescs.push_back(&hullProgram->getParamDesc());
				}

				HGpuProgram domainProgram = curPass->getDomainProgram();
				if(domainProgram)
				{
					domainProgram.synchronize();
					allParamDescs.push_back(&domainProgram->getParamDesc());
				}

				HGpuProgram computeProgram = curPass->getComputeProgram();
				if(computeProgram)
				{
					computeProgram.synchronize();
					allParamDescs.push_back(&computeProgram->getParamDesc());
				}
			}

			// Fill out various helper structures
			Map<String, const GpuParamDataDesc*> validDataParameters = determineValidDataParameters(allParamDescs);
			Set<String> validObjectParameters = determineValidObjectParameters(allParamDescs);

			Set<String> validShareableParamBlocks = determineValidShareableParamBlocks(allParamDescs);
			Map<String, String> paramToParamBlockMap = determineParameterToBlockMapping(allParamDescs);
			Map<String, GpuParamBlockBufferPtr> paramBlockBuffers;

			// Create param blocks
			const Map<String, SHADER_PARAM_BLOCK_DESC>& shaderDesc = mShader->_getParamBlocks();
			for(auto iter = validShareableParamBlocks.begin(); iter != validShareableParamBlocks.end(); ++iter)
			{
				bool createBlockBuffer = true;
				GpuParamBlockUsage usage = GPBU_STATIC;

				auto iterFind = shaderDesc.find(*iter);
				if(iterFind != shaderDesc.end())
				{
					createBlockBuffer = !iterFind->second.shared && iterFind->second.rendererSemantic == 0;
					usage = iterFind->second.usage;
				}

				GpuParamBlockDesc blockDesc;
				for(auto iter2 = allParamDescs.begin(); iter2 != allParamDescs.end(); ++iter2)
				{
					auto findParamBlockDesc = (*iter2)->paramBlocks.find(*iter);

					if(findParamBlockDesc != (*iter2)->paramBlocks.end())
					{
						blockDesc = findParamBlockDesc->second;
						break;
					}
				}

				GpuParamBlockBufferPtr newParamBlockBuffer;
				if(createBlockBuffer)
				{
					newParamBlockBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(blockDesc.blockSize * sizeof(UINT32), usage);
					mParamBuffers.push_back(newParamBlockBuffer);
				}

				paramBlockBuffers[*iter] = newParamBlockBuffer;
				mValidShareableParamBlocks.insert(*iter);
			}

			// Create data param mappings
			const Map<String, SHADER_DATA_PARAM_DESC>& dataParamDesc = mShader->_getDataParams();
			for(auto iter = dataParamDesc.begin(); iter != dataParamDesc.end(); ++iter)
			{
				auto findIter = validDataParameters.find(iter->second.gpuVariableName);

				// Not valid so we skip it
				if(findIter == validDataParameters.end())
					continue;

				if(findIter->second->type != iter->second.type)
				{
					LOGWRN("Ignoring shader parameter \"" + iter->first  +"\". Type doesn't match the one defined in the gpu program. "
						+ "Shader defined type: " + toString(iter->second.type) + " - Gpu program defined type: " + toString(findIter->second->type));
					continue;
				}

				if(findIter->second->arraySize != iter->second.arraySize)
				{
					LOGWRN("Ignoring shader parameter \"" + iter->first  +"\". Array size doesn't match the one defined in the gpu program."
						+ "Shader defined array size: " + toString(iter->second.arraySize) + " - Gpu program defined array size: " + toString(findIter->second->arraySize));
					continue;
				}

				auto findBlockIter = paramToParamBlockMap.find(iter->second.gpuVariableName);

				if(findBlockIter == paramToParamBlockMap.end())
					BS_EXCEPT(InternalErrorException, "Parameter doesn't exist in param to param block map but exists in valid param map.");

				String& paramBlockName = findBlockIter->second;
				mValidParams[iter->first] = iter->second.gpuVariableName;
			}

			// Create object param mappings
			const Map<String, SHADER_OBJECT_PARAM_DESC>& objectParamDesc = mShader->_getObjectParams();
			for(auto iter = objectParamDesc.begin(); iter != objectParamDesc.end(); ++iter)
			{
				auto findIter = validObjectParameters.find(iter->second.gpuVariableName);

				// Not valid so we skip it
				if(findIter == validObjectParameters.end())
					continue;

				mValidParams[iter->first] = iter->second.gpuVariableName;
			}

			for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
			{
				PassPtr curPass = mBestTechnique->getPass(i);
				PassParametersPtr params = PassParametersPtr(new PassParameters());

				HGpuProgram vertProgram = curPass->getVertexProgram();
				if(vertProgram)
					params->mVertParams = vertProgram->createParameters();

				HGpuProgram fragProgram = curPass->getFragmentProgram();
				if(fragProgram)
					params->mFragParams = fragProgram->createParameters();

				HGpuProgram geomProgram = curPass->getGeometryProgram();
				if(geomProgram)
					params->mGeomParams = geomProgram->createParameters();	

				HGpuProgram hullProgram = curPass->getHullProgram();
				if(hullProgram)
					params->mHullParams = hullProgram->createParameters();

				HGpuProgram domainProgram = curPass->getDomainProgram();
				if(domainProgram)
					params->mDomainParams = domainProgram->createParameters();

				HGpuProgram computeProgram = curPass->getComputeProgram();
				if(computeProgram)
					params->mComputeParams = computeProgram->createParameters();	

				mParametersPerPass.push_back(params);
			}

			// Assign param block buffers
			for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
			{
				PassParametersPtr params = *iter;

				for(UINT32 i = 0; i < params->getNumParams(); i++)
				{
					GpuParamsPtr& paramPtr = params->getParamByIdx(i);
					if(paramPtr)
					{
						// Assign shareable buffers
						for(auto iterBlock = mValidShareableParamBlocks.begin(); iterBlock != mValidShareableParamBlocks.end(); ++iterBlock)
						{
							const String& paramBlockName = *iterBlock;
							if(paramPtr->hasParamBlock(paramBlockName))
							{
								GpuParamBlockBufferPtr blockBuffer = paramBlockBuffers[paramBlockName];

								paramPtr->setParamBlockBuffer(paramBlockName, blockBuffer);
							}
						}

						// Create non-shareable ones
						const GpuParamDesc& desc = paramPtr->getParamDesc();
						for(auto iterBlockDesc = desc.paramBlocks.begin(); iterBlockDesc != desc.paramBlocks.end(); ++iterBlockDesc)
						{
							if(!iterBlockDesc->second.isShareable)
							{
								GpuParamBlockBufferPtr newParamBlockBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(iterBlockDesc->second.blockSize * sizeof(UINT32));
								mParamBuffers.push_back(newParamBlockBuffer);

								paramPtr->setParamBlockBuffer(iterBlockDesc->first, newParamBlockBuffer);
							}
						}
					}
				}
			}
		}

		mCoreDirtyFlags = 0xFFFFFFFF;
	}

	Map<String, const GpuParamDataDesc*> Material::determineValidDataParameters(const Vector<const GpuParamDesc*>& paramDescs) const
	{
		Map<String, const GpuParamDataDesc*> foundDataParams;
		Map<String, bool> validParams;

		for(auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;

			// Check regular data params
			for(auto iter2 = curDesc.params.begin(); iter2 != curDesc.params.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamDataDesc& curParam = iter2->second;

				auto dataFindIter = validParams.find(iter2->first);
				if(dataFindIter == validParams.end())
				{
					validParams[iter2->first] = true;
					foundDataParams[iter2->first] = &curParam;
				}
				else
				{
					if(validParams[iter2->first])
					{
						auto dataFindIter2 = foundDataParams.find(iter2->first);

						const GpuParamDataDesc* otherParam = dataFindIter2->second;
						if(!areParamsEqual(curParam, *otherParam, true))
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

	
	Set<String> Material::determineValidObjectParameters(const Vector<const GpuParamDesc*>& paramDescs) const
	{
		Set<String> validParams;

		for(auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;

			// Check sampler params
			for(auto iter2 = curDesc.samplers.begin(); iter2 != curDesc.samplers.end(); ++iter2)
			{
				if(validParams.find(iter2->first) == validParams.end())
					validParams.insert(iter2->first);
			}

			// Check texture params
			for(auto iter2 = curDesc.textures.begin(); iter2 != curDesc.textures.end(); ++iter2)
			{
				if(validParams.find(iter2->first) == validParams.end())
					validParams.insert(iter2->first);
			}

			// Check buffer params
			for(auto iter2 = curDesc.buffers.begin(); iter2 != curDesc.buffers.end(); ++iter2)
			{
				if(validParams.find(iter2->first) == validParams.end())
					validParams.insert(iter2->first);
			}
		}

		return validParams;
	}

	Set<String> Material::determineValidShareableParamBlocks(const Vector<const GpuParamDesc*>& paramDescs) const
	{
		// Make sure param blocks with the same name actually are the same
		Map<String, std::pair<String, const GpuParamDesc*>> uniqueParamBlocks;
		Map<String, bool> validParamBlocks;

		for(auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;
			for(auto blockIter = curDesc.paramBlocks.begin(); blockIter != curDesc.paramBlocks.end(); ++blockIter)
			{
				bool isBlockValid = true;
				const GpuParamBlockDesc& curBlock = blockIter->second;

				if(!curBlock.isShareable) // Non-shareable buffers are handled differently, they're allowed same names
					continue;

				auto iterFind = uniqueParamBlocks.find(blockIter->first);
				if(iterFind == uniqueParamBlocks.end())
				{
					uniqueParamBlocks[blockIter->first] = std::make_pair(blockIter->first, *iter);
					validParamBlocks[blockIter->first] = true;
					continue;
				}

				String otherBlockName = iterFind->second.first;
				const GpuParamDesc* otherDesc = iterFind->second.second;

				for(auto myParamIter = curDesc.params.begin(); myParamIter != curDesc.params.end(); ++myParamIter)
				{
					const GpuParamDataDesc& myParam = myParamIter->second;

					if(myParam.paramBlockSlot != curBlock.slot)
						continue; // Param is in another block, so we will check it when its time for that block

					auto otherParamFind = otherDesc->params.find(myParamIter->first);

					// Cannot find other param, blocks aren't equal
					if(otherParamFind == otherDesc->params.end())
					{
						isBlockValid = false;
						break;
					}

					const GpuParamDataDesc& otherParam = otherParamFind->second;

					if(!areParamsEqual(myParam, otherParam) || curBlock.name != otherBlockName)
					{
						isBlockValid = false;
						break;
					}
				}

				if(!isBlockValid)
				{
					if(validParamBlocks[blockIter->first])
					{
						LOGWRN("Found two param blocks with the same name but different contents: " + blockIter->first);
						validParamBlocks[blockIter->first] = false;
					}
				}
			}
		}

		Set<String> validParamBlocksReturn;
		for(auto iter = validParamBlocks.begin(); iter != validParamBlocks.end(); ++iter)
		{
			if(iter->second)
				validParamBlocksReturn.insert(iter->first);
		}

		return validParamBlocksReturn;
	}

	Map<String, String> Material::determineParameterToBlockMapping(const Vector<const GpuParamDesc*>& paramDescs)
	{
		Map<String, String> paramToParamBlock;

		for(auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;
			for(auto iter2 = curDesc.params.begin(); iter2 != curDesc.params.end(); ++iter2)
			{
				const GpuParamDataDesc& curParam = iter2->second;
				
				auto iterFind = paramToParamBlock.find(curParam.name);
				if(iterFind != paramToParamBlock.end())
					continue;

				for(auto iterBlock = curDesc.paramBlocks.begin(); iterBlock != curDesc.paramBlocks.end(); ++iterBlock)
				{
					if(iterBlock->second.slot == curParam.paramBlockSlot)
					{
						paramToParamBlock[curParam.name] = iterBlock->second.name;
						break;
					}
				}
			}
		}

		return paramToParamBlock;
	}

	bool Material::areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets) const
	{
		bool equal = paramA.arraySize == paramB.arraySize && paramA.elementSize == paramB.elementSize 
			&& paramA.type == paramB.type && paramA.arrayElementStride == paramB.arrayElementStride;

		if(!ignoreBufferOffsets)
			equal &= paramA.cpuMemOffset == paramB.cpuMemOffset && paramA.gpuMemOffset == paramB.gpuMemOffset;

		return equal;
	}

	void Material::throwIfNotInitialized() const
	{
		if(mShader == nullptr)
		{
			BS_EXCEPT(InternalErrorException, "Material does not have shader set.");
		}

		if(mBestTechnique == nullptr)
		{
			BS_EXCEPT(InternalErrorException, "Shader does not contain a supported technique.");
		}
	}

	void Material::setColor(const String& name, const Color& value, UINT32 arrayIdx)				
	{ 
		return getParamVec4(name).set(Vector4(value.r, value.g, value.b, value.a), arrayIdx); 
	}

	void Material::setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		auto iterFind = mValidShareableParamBlocks.find(name);
		if(iterFind == mValidShareableParamBlocks.end())
		{
			LOGWRN("Material doesn't have a parameter block named " + name);
			return;
		}

		for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			for(UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsPtr& paramPtr = params->getParamByIdx(i);
				if(paramPtr)
				{
					if(paramPtr->hasParamBlock(name))
						paramPtr->setParamBlockBuffer(name, paramBlockBuffer);
				}
			}
		}
	}

	UINT32 Material::getNumPasses() const
	{
		throwIfNotInitialized();

		return mShader->getBestTechnique()->getNumPasses();
	}

	PassPtr Material::getPass(UINT32 passIdx) const
	{
		if(passIdx < 0 || passIdx >= mShader->getBestTechnique()->getNumPasses())
			BS_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mShader->getBestTechnique()->getPass(passIdx);
	}

	PassParametersPtr Material::getPassParameters(UINT32 passIdx) const
	{
		if(passIdx < 0 || passIdx >= mParametersPerPass.size())
			BS_EXCEPT(InvalidParametersException, "Invalid pass index.");

		PassParametersPtr params = mParametersPerPass[passIdx];

		return params;
	}

	Material::StructData Material::getStructData(const String& name, UINT32 arrayIdx) const
	{
		GpuParamStruct structParam = getParamStruct(name);

		StructData data(structParam.getElementSize());
		structParam.get(data.data.get(), structParam.getElementSize(), arrayIdx);

		return data;
	}

	GpuParamFloat Material::getParamFloat(const String& name) const
	{
		TGpuDataParam<float> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamVec2 Material::getParamVec2(const String& name) const
	{
		TGpuDataParam<Vector2> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamVec3 Material::getParamVec3(const String& name) const
	{
		TGpuDataParam<Vector3> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamVec4 Material::getParamVec4(const String& name) const
	{
		TGpuDataParam<Vector4> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamMat3 Material::getParamMat3(const String& name) const
	{
		TGpuDataParam<Matrix3> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamMat4 Material::getParamMat4(const String& name) const
	{
		TGpuDataParam<Matrix4> gpuParam;
		getParam(name, gpuParam);

		return gpuParam;
	}

	GpuParamStruct Material::getParamStruct(const String& name) const
	{
		throwIfNotInitialized();

		GpuParamStruct gpuParam;

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return gpuParam;
		}

		const String& gpuVarName = iterFind->second;
		GpuParamsPtr params = findParamsWithName(gpuVarName);

		params->getStructParam(gpuVarName, gpuParam);
		return gpuParam;
	}

	GpuParamTexture Material::getParamTexture(const String& name) const
	{
		throwIfNotInitialized();

		GpuParamTexture gpuParam;

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return gpuParam;
		}

		const String& gpuVarName = iterFind->second;
		GpuParamsPtr params = findTexWithName(gpuVarName);

		params->getTextureParam(gpuVarName, gpuParam);
		return gpuParam;
	}

	GpuParamSampState Material::getParamSamplerState(const String& name) const
	{
		throwIfNotInitialized();

		GpuParamSampState gpuParam;

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return gpuParam;
		}

		const String& gpuVarName = iterFind->second;
		GpuParamsPtr params = findSamplerStateWithName(gpuVarName);

		params->getSamplerStateParam(gpuVarName, gpuParam);
		return gpuParam;
	}

	GpuParamsPtr Material::findParamsWithName(const String& name) const
	{
		for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			for(UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsPtr& paramPtr = params->getParamByIdx(i);
				if(paramPtr)
				{
					if(paramPtr->hasParam(name))
						return paramPtr;
				}
			}
		}

		BS_EXCEPT(InternalErrorException, "Shader has no parameter with the name: " + name);
	}

	GpuParamsPtr Material::findTexWithName(const String& name) const
	{
		for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			for(UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsPtr& paramPtr = params->getParamByIdx(i);
				if(paramPtr)
				{
					if(paramPtr->hasTexture(name))
						return paramPtr;
				}
			}
		}

		BS_EXCEPT(InternalErrorException, "Shader has no parameter with the name: " + name);
	}

	GpuParamsPtr Material::findSamplerStateWithName(const String& name) const
	{
		for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			for(UINT32 i = 0; i < params->getNumParams(); i++)
			{
				GpuParamsPtr& paramPtr = params->getParamByIdx(i);
				if(paramPtr)
				{
					if(paramPtr->hasSamplerState(name))
						return paramPtr;
				}
			}
		}

		BS_EXCEPT(InternalErrorException, "Shader has no parameter with the name: " + name);
	}

	bool Material::_isCoreDirty(MaterialDirtyFlag flag) const
	{ 
		if (flag == MaterialDirtyFlag::Params)
		{
			for (auto& paramsPerPass : mParametersPerPass)
			{
				for (UINT32 i = 0; i < paramsPerPass->getNumParams(); i++)
				{
					GpuParamsPtr params = paramsPerPass->getParamByIdx(i);
					if (params != nullptr && params->_isCoreDirty())
						return true;
				}
			}

			return false;
		}
		else
			return (mCoreDirtyFlags & (UINT32)flag) != 0 || (mShader != nullptr && mShader->_isCoreDirty(ShaderDirtyFlag::Shader));
	}

	void Material::_markCoreClean(MaterialDirtyFlag flag)
	{ 
		mCoreDirtyFlags &= ~(UINT32)flag;

		if (flag == MaterialDirtyFlag::Material)
		{
			if (mShader != nullptr)
				mShader->_markCoreClean(ShaderDirtyFlag::Shader);
		}
		
		if (flag == MaterialDirtyFlag::Material || flag == MaterialDirtyFlag::Params)
		{
			for (auto& paramsPerPass : mParametersPerPass)
			{
				for (UINT32 i = 0; i < paramsPerPass->getNumParams(); i++)
				{
					GpuParamsPtr params = paramsPerPass->getParamByIdx(i);

					if (params != nullptr)
						params->_markCoreClean();
				}
			}
		}
	}

	Vector<MaterialProxy::ParamsBindInfo> Material::_getDirtyProxyParams()
	{
		Vector<MaterialProxy::ParamsBindInfo> dirtyParams;
		UINT32 idx = 0;

		UINT32 numPasses = mShader->getBestTechnique()->getNumPasses();
		for (UINT32 i = 0; i < numPasses; i++)
		{
			PassParametersPtr params = mParametersPerPass[i];
			PassPtr pass = mShader->getBestTechnique()->getPass(i);

			if (pass->hasVertexProgram())
			{
				if (params->mVertParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mVertParams));

				idx++;
			}

			if (pass->hasFragmentProgram())
			{
				if (params->mFragParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mFragParams));

				idx++;
			}

			if (pass->hasGeometryProgram())
			{
				if (params->mGeomParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mGeomParams));

				idx++;
			}

			if (pass->hasHullProgram())
			{
				if (params->mHullParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mHullParams));

				idx++;
			}

			if (pass->hasDomainProgram())
			{
				if (params->mDomainParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mDomainParams));

				idx++;
			}

			if (pass->hasComputeProgram())
			{
				if (params->mComputeParams->_isCoreDirty())
					dirtyParams.push_back(MaterialProxy::ParamsBindInfo(idx, params->mComputeParams));

				idx++;
			}

			return dirtyParams;
		}
	}

	MaterialProxyPtr Material::_createProxy()
	{
		throwIfNotInitialized();

		MaterialProxyPtr proxy = bs_shared_ptr<MaterialProxy>();

		UINT32 numPasses = mShader->getBestTechnique()->getNumPasses();
		for (UINT32 i = 0; i < numPasses; i++)
		{
			PassParametersPtr params = mParametersPerPass[i];
			PassPtr pass = mShader->getBestTechnique()->getPass(i);

			proxy->passes.push_back(MaterialProxyPass());
			MaterialProxyPass& passData = proxy->passes.back();

			if (pass->hasVertexProgram())
			{
				passData.vertexProg = pass->getVertexProgram();
				passData.vertexProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mVertParams->_cloneForCore());
			}
			else
				passData.vertexProgParamsIdx = 0;

			if (pass->hasFragmentProgram())
			{
				passData.fragmentProg = pass->getFragmentProgram();
				passData.fragmentProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mFragParams->_cloneForCore());
			}
			else
				passData.fragmentProgParamsIdx = 0;

			if (pass->hasGeometryProgram())
			{
				passData.geometryProg = pass->getGeometryProgram();
				passData.geometryProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mGeomParams->_cloneForCore());
			}
			else
				passData.geometryProgParamsIdx = 0;

			if (pass->hasHullProgram())
			{
				passData.hullProg = pass->getHullProgram();
				passData.hullProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mHullParams->_cloneForCore());
			}
			else
				passData.hullProgParamsIdx = 0;

			if (pass->hasDomainProgram())
			{
				passData.domainProg = pass->getDomainProgram();
				passData.domainProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mDomainParams->_cloneForCore());
			}
			else
				passData.domainProgParamsIdx = 0;

			if (pass->hasComputeProgram())
			{
				passData.computeProg = pass->getComputeProgram();
				passData.computeProgParamsIdx = proxy->params.size();
				proxy->params.push_back(params->mComputeParams->_cloneForCore());
			}
			else
				passData.computeProgParamsIdx = 0;

			passData.blendState = pass->getBlendState();
			passData.rasterizerState = pass->getRasterizerState();
			passData.depthStencilState = pass->getDepthStencilState();
			passData.stencilRefValue = pass->getStencilRefValue();
		}

		if (mShader->_isCoreDirty(ShaderDirtyFlag::Proxy))
		{
			mShader->_setActiveProxy(mShader->_createProxy());
			mShader->_markCoreClean(ShaderDirtyFlag::Proxy);
		}

		proxy->shader = mShader->_getActiveProxy();

		return proxy;
	}

	void Material::destroy_internal()
	{
		freeParamBuffers();

		Resource::destroy_internal();
	}

	void Material::freeParamBuffers()
	{
		mParamBuffers.clear();
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