#include "CmMaterial.h"
#include "CmException.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramParams.h"
#include "CmHardwareBufferManager.h"
#include "CmGpuProgram.h"
#include "CmGpuParamDesc.h"
#include "CmMaterialRTTI.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	Material::Material()
	{
		// Material doesn't do anything render thread specific, so we can just initialize right away
		initialize_internal();
	}

	void Material::initialize_internal()
	{
		Resource::initialize_internal();
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

		if(mShader)
		{
			mBestTechnique = mShader->getBestTechnique();

			if(mBestTechnique == nullptr)
				return;

			mValidShareableParamBlocks.clear();
			mValidParams.clear();

			vector<const GpuParamDesc*>::type allParamDescs;

			// Make sure all gpu programs are fully loaded
			for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
			{
				PassPtr curPass = mBestTechnique->getPass(i);

				GpuProgramHandle vertProgram = curPass->getVertexProgram();
				if(vertProgram)
				{
					vertProgram.waitUntilLoaded();
					allParamDescs.push_back(&vertProgram->getParamDesc());
				}

				GpuProgramHandle fragProgram = curPass->getFragmentProgram();
				if(fragProgram)
				{
					fragProgram.waitUntilLoaded();
					allParamDescs.push_back(&fragProgram->getParamDesc());
				}

				GpuProgramHandle geomProgram = curPass->getGeometryProgram();
				if(geomProgram)
				{
					geomProgram.waitUntilLoaded();
					allParamDescs.push_back(&geomProgram->getParamDesc());
				}

				GpuProgramHandle hullProgram = curPass->getHullProgram();
				if(hullProgram)
				{
					hullProgram.waitUntilLoaded();
					allParamDescs.push_back(&hullProgram->getParamDesc());
				}

				GpuProgramHandle domainProgram = curPass->getDomainProgram();
				if(domainProgram)
				{
					domainProgram.waitUntilLoaded();
					allParamDescs.push_back(&domainProgram->getParamDesc());
				}

				GpuProgramHandle computeProgram = curPass->getComputeProgram();
				if(computeProgram)
				{
					computeProgram.waitUntilLoaded();
					allParamDescs.push_back(&computeProgram->getParamDesc());
				}
			}

			// Fill out various helper structures
			set<String>::type validParameters = determineValidParameters(allParamDescs);
			set<String>::type validShareableParamBlocks = determineValidShareableParamBlocks(allParamDescs);
			map<String, String>::type paramToParamBlockMap = determineParameterToBlockMapping(allParamDescs);
			map<String, GpuParamBlockPtr>::type paramBlocks;

			// Create param blocks
			const map<String, SHADER_PARAM_BLOCK_DESC>::type& shaderDesc = mShader->getParamBlocks();
			for(auto iter = validShareableParamBlocks.begin(); iter != validShareableParamBlocks.end(); ++iter)
			{
				bool isShared = false;
				GpuParamBlockUsage usage = GPBU_STATIC;

				auto iterFind = shaderDesc.find(*iter);
				if(iterFind != shaderDesc.end())
				{
					isShared = iterFind->second.shared;
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

				GpuParamBlockPtr newParamBlockBuffer;
				if(!isShared)
					newParamBlockBuffer = HardwareBufferManager::instance().createGpuParamBlock(blockDesc, usage);

				paramBlocks[*iter] = newParamBlockBuffer;
				mValidShareableParamBlocks.insert(*iter);
			}

			// Create data param mappings
			const map<String, SHADER_DATA_PARAM_DESC>::type& dataParamDesc = mShader->getDataParams();
			for(auto iter = dataParamDesc.begin(); iter != dataParamDesc.end(); ++iter)
			{
				auto findIter = validParameters.find(iter->first);

				// Not valid so we skip it
				if(findIter == validParameters.end())
					continue;

				auto findBlockIter = paramToParamBlockMap.find(iter->first);

				if(findBlockIter == paramToParamBlockMap.end())
					CM_EXCEPT(InternalErrorException, "Parameter doesn't exist in param to param block map but exists in valid param map.");

				String& paramBlockName = findBlockIter->second;
				mValidParams.insert(iter->first);
			}

			// Create object param mappings
			const map<String, SHADER_OBJECT_PARAM_DESC>::type& objectParamDesc = mShader->getObjectParams();
			for(auto iter = objectParamDesc.begin(); iter != objectParamDesc.end(); ++iter)
			{
				auto findIter = validParameters.find(iter->first);

				// Not valid so we skip it
				if(findIter == validParameters.end())
					continue;

				mValidParams.insert(iter->first);
			}

			for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
			{
				PassPtr curPass = mBestTechnique->getPass(i);
				PassParametersPtr params = PassParametersPtr(new PassParameters());

				GpuProgramHandle vertProgram = curPass->getVertexProgram();
				if(vertProgram)
					params->mVertParams = vertProgram->createParameters();

				GpuProgramHandle fragProgram = curPass->getFragmentProgram();
				if(fragProgram)
					params->mFragParams = fragProgram->createParameters();

				GpuProgramHandle geomProgram = curPass->getGeometryProgram();
				if(geomProgram)
					params->mGeomParams = geomProgram->createParameters();	

				GpuProgramHandle hullProgram = curPass->getHullProgram();
				if(hullProgram)
					params->mHullParams = hullProgram->createParameters();

				GpuProgramHandle domainProgram = curPass->getDomainProgram();
				if(domainProgram)
					params->mDomainParams = domainProgram->createParameters();

				GpuProgramHandle computeProgram = curPass->getComputeProgram();
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
								GpuParamBlockPtr blockBuffer = paramBlocks[paramBlockName];

								paramPtr->setParamBlock(paramBlockName, blockBuffer);
							}
						}

						// Create non-shareable ones
						const GpuParamDesc& desc = paramPtr->getParamDesc();
						for(auto iterBlockDesc = desc.paramBlocks.begin(); iterBlockDesc != desc.paramBlocks.end(); ++iterBlockDesc)
						{
							if(!iterBlockDesc->second.isShareable)
							{
								GpuParamBlockPtr newParamBlockBuffer = HardwareBufferManager::instance().createGpuParamBlock(iterBlockDesc->second);
								paramPtr->setParamBlock(iterBlockDesc->first, newParamBlockBuffer);
							}
						}
					}
				}
			}
		}
	}

	set<String>::type Material::determineValidParameters(const vector<const GpuParamDesc*>::type& paramDescs) const
	{
		map<String, const GpuParamDataDesc*>::type foundDataParams;
		map<String, const GpuParamObjectDesc*>::type foundObjectParams;

		map<String, bool>::type validParameters;

		for(auto iter = paramDescs.begin(); iter != paramDescs.end(); ++iter)
		{
			const GpuParamDesc& curDesc = **iter;

			// Check regular data params
			for(auto iter2 = curDesc.params.begin(); iter2 != curDesc.params.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamDataDesc& curParam = iter2->second;

				auto objectFindIter = foundObjectParams.find(iter2->first);
				if(objectFindIter != foundObjectParams.end())
					isParameterValid = false; // Data param but we found another as object param with the same name

				if(isParameterValid)
				{
					auto dataFindIter = foundDataParams.find(iter2->first);
					if(dataFindIter == foundDataParams.end())
					{
						validParameters[iter2->first] = true;
						foundDataParams[iter2->first] = &curParam;
					}
					else
					{
						const GpuParamDataDesc* otherParam = dataFindIter->second;
						if(!areParamsEqual(curParam, *otherParam, true))
							isParameterValid = false;
					}
				}

				if(!isParameterValid)
				{
					if(validParameters[iter2->first]) // Do this check so we only report this error once
						LOGWRN("Found two parameters with the same name but different contents: " + iter2->first);

					validParameters[iter2->first] = false;
				}
			}

			// Check sampler params
			for(auto iter2 = curDesc.samplers.begin(); iter2 != curDesc.samplers.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamObjectDesc& curParam = iter2->second;

				auto dataFindIter = foundDataParams.find(iter2->first);
				if(dataFindIter != foundDataParams.end())
					isParameterValid = false; // Object param but we found another as data param with the same name

				if(isParameterValid)
				{
					auto objectFindIter = foundObjectParams.find(iter2->first);
					if(objectFindIter == foundObjectParams.end())
					{
						validParameters[iter2->first] = true;
						foundObjectParams[iter2->first] = &curParam;
					}
					else
					{
						const GpuParamObjectDesc* otherParam = objectFindIter->second;
						if(!areParamsEqual(curParam, *otherParam))
							isParameterValid = false;
					}
				}

				if(!isParameterValid)
				{
					if(validParameters[iter2->first]) // Do this check so we only report this error once
						LOGWRN("Found two parameters with the same name but different contents: " + iter2->first);

					validParameters[iter2->first] = false;
				}
			}

			// Check texture params
			for(auto iter2 = curDesc.textures.begin(); iter2 != curDesc.textures.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamObjectDesc& curParam = iter2->second;

				auto dataFindIter = foundDataParams.find(iter2->first);
				if(dataFindIter != foundDataParams.end())
					isParameterValid = false; // Object param but we found another as data param with the same name

				if(isParameterValid)
				{
					auto objectFindIter = foundObjectParams.find(iter2->first);
					if(objectFindIter == foundObjectParams.end())
					{
						validParameters[iter2->first] = true;
						foundObjectParams[iter2->first] = &curParam;
					}
					else
					{
						const GpuParamObjectDesc* otherParam = objectFindIter->second;
						if(!areParamsEqual(curParam, *otherParam))
							isParameterValid = false;
					}
				}

				if(!isParameterValid)
				{
					if(validParameters[iter2->first]) // Do this check so we only report this error once
						LOGWRN("Found two parameters with the same name but different contents: " + iter2->first);

					validParameters[iter2->first] = false;
				}
			}

			// Check buffer params
			for(auto iter2 = curDesc.buffers.begin(); iter2 != curDesc.buffers.end(); ++iter2)
			{
				bool isParameterValid = true;
				const GpuParamObjectDesc& curParam = iter2->second;

				auto dataFindIter = foundDataParams.find(iter2->first);
				if(dataFindIter != foundDataParams.end())
					isParameterValid = false; // Object param but we found another as data param with the same name

				if(isParameterValid)
				{
					auto objectFindIter = foundObjectParams.find(iter2->first);
					if(objectFindIter == foundObjectParams.end())
					{
						validParameters[iter2->first] = true;
						foundObjectParams[iter2->first] = &curParam;
					}
					else
					{
						const GpuParamObjectDesc* otherParam = objectFindIter->second;
						if(!areParamsEqual(curParam, *otherParam))
							isParameterValid = false;
					}
				}

				if(!isParameterValid)
				{
					if(validParameters[iter2->first]) // Do this check so we only report this error once
						LOGWRN("Found two parameters with the same name but different contents: " + iter2->first);

					validParameters[iter2->first] = false;
				}
			}
		}

		set<String>::type validParamsReturn;
		for(auto iter = validParameters.begin(); iter != validParameters.end(); ++iter)
		{
			if(iter->second)
				validParamsReturn.insert(iter->first);
		}

		return validParamsReturn;
	}

	set<String>::type Material::determineValidShareableParamBlocks(const vector<const GpuParamDesc*>::type& paramDescs) const
	{
		// Make sure param blocks with the same name actually are the same
		map<String, std::pair<String, const GpuParamDesc*>>::type uniqueParamBlocks;
		map<String, bool>::type validParamBlocks;

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

		set<String>::type validParamBlocksReturn;
		for(auto iter = validParamBlocks.begin(); iter != validParamBlocks.end(); ++iter)
		{
			if(iter->second)
				validParamBlocksReturn.insert(iter->first);
		}

		return validParamBlocksReturn;
	}

	map<String, String>::type Material::determineParameterToBlockMapping(const vector<const GpuParamDesc*>::type& paramDescs)
	{
		map<String, String>::type paramToParamBlock;

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
		bool equal = paramA.arraySize == paramB.arraySize && paramA.elementSize == paramB.elementSize && paramA.type == paramB.type;

		if(!ignoreBufferOffsets)
			equal &= paramA.cpuMemOffset == paramB.cpuMemOffset && paramA.gpuMemOffset == paramB.gpuMemOffset;

		return equal;
	}

	bool Material::areParamsEqual(const GpuParamObjectDesc& paramA, const GpuParamObjectDesc& paramB) const
	{
		return paramA.type == paramB.type;
	}

	void Material::throwIfNotInitialized() const
	{
		if(mShader == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Material does not have shader set.");
		}

		if(mBestTechnique == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Shader does not contain a supported technique.");
		}
	}

	void Material::setTexture(const String& name, TextureHandle& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
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
					if(paramPtr->hasTexture(name))
						paramPtr->setTexture(name, value);
				}
			}
		}
	}

	void Material::setSamplerState(const String& name, SamplerStatePtr samplerState)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
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
					if(paramPtr->hasSamplerState(name))
						paramPtr->setSamplerState(name, samplerState);
				}
			}
		}
	}

	void Material::setFloat(const String& name, float value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setColor(const String& name, const Color& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setVec2(const String& name, const Vector2& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setVec3(const String& name, const Vector3& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setVec4(const String& name, const Vector4& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setMat3(const String& name, const Matrix3& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}

	void Material::setMat4(const String& name, const Matrix4& value)
	{
		throwIfNotInitialized();

		auto iterFind = mValidParams.find(name);
		if(iterFind == mValidParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name);
			return;
		}

		setParam(name, value);
	}


	void Material::setParamBlock(const String& name, GpuParamBlockPtr paramBlock)
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
						paramPtr->setParam(name, paramBlock);
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
			CM_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mShader->getBestTechnique()->getPass(passIdx);
	}

	PassParametersPtr Material::getPassParameters(UINT32 passIdx) const
	{
		if(passIdx < 0 || passIdx >= mParametersPerPass.size())
			CM_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mParametersPerPass[passIdx];
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