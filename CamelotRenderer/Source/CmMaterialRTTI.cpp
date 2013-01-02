#include "CmMaterialRTTI.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine
{
	RTTITypeBase* MaterialRTTI::MaterialParams::getRTTIStatic()
	{
		return MaterialParamsRTTI::instance();
	}

	RTTITypeBase* MaterialRTTI::MaterialParams::getRTTI() const
	{
		return MaterialParams::getRTTIStatic();
	}

	void MaterialRTTI::onSerializationStarted(IReflectable* obj)
	{
		Material* material = static_cast<Material*>(obj);
		std::shared_ptr<MaterialParams> params = std::shared_ptr<MaterialParams>(new MaterialParams());

		vector<GpuParamsPtr>::type allParams;
		for(size_t i = 0; i < material->mParameters.size(); i++)
		{
			if(material->mParameters[i]->mFragParams != nullptr)
				allParams.push_back(material->mParameters[i]->mFragParams);

			if(material->mParameters[i]->mVertParams != nullptr)
				allParams.push_back(material->mParameters[i]->mVertParams);

			if(material->mParameters[i]->mGeomParams != nullptr)
				allParams.push_back(material->mParameters[i]->mGeomParams);
		}

		for(size_t i = 0; i < allParams.size(); i++)
		{
			const GpuParamDesc& paramDesc = allParams[i]->getParamDesc();

			for(auto iter = paramDesc.textures.begin(); iter != paramDesc.textures.end(); ++iter)
			{
				params->mTextureParams[iter->first] = allParams[i]->getTexture(iter->second.slot);
			}

			for(auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
			{
				params->mSamplerStateParams[iter->first] = allParams[i]->getSamplerState(iter->second.slot);
			}

			for(auto iter = paramDesc.params.begin(); iter != paramDesc.params.end(); ++iter)
			{
				// TODO - Need to save float/int/bool parameters!
			}

			/*const GpuNamedConstants& namedConstants = allParams[i]->getConstantDefinitions();

			float tempValue[16];
			for(auto iter = namedConstants.map.begin(); iter != namedConstants.map.end(); ++iter)
			{
				const GpuConstantDefinition& def = iter->second;

				if(def.constType == GCT_SAMPLER2D)
				{
					TextureHandle texture;
					allParams[i]->getTexture(iter->second.physicalIndex, texture);
					params->mTextureParams[iter->first] = texture;

					SamplerStatePtr samplerState = allParams[i]->getSamplerState(iter->second.physicalIndex);
					params->mSamplerStateParams[iter->first] = samplerState;
				}
				else
				{
					UINT32 fieldSize = def.getElementSize(def.constType, false);
					switch(def.constType)
					{
					case GCT_FLOAT1:
					case GCT_FLOAT2:
					case GCT_FLOAT3:
					case GCT_FLOAT4:
					case GCT_MATRIX_3X3:
					case GCT_MATRIX_4X4:
					case GCT_SAMPLER2D:
						break;
					default:
						CM_EXCEPT(InternalErrorException, "Material parameter type not supported! Type: " + toString(def.constType));
					}

					if(fieldSize > 16)
						CM_EXCEPT(InternalErrorException, "Field size larger than the supported size.");

					allParams[i]->_readRawConstants(iter->second.physicalIndex, fieldSize, tempValue);

					auto iterFind = params->mFloatParams.find(iter->first);
					if(iterFind == params->mFloatParams.end())
					{
						params->mFloatParams[iter->first] = FloatParam((UINT32)params->mFloatBuffer.size(), def.constType, fieldSize);

						for(UINT32 j = 0; j < fieldSize; j++)
							params->mFloatBuffer.push_back(tempValue[j]);
					}
					else
						gDebug().logWarning("While saving material found multiple parameters with the same name. Only saving the first.");
				}
			}*/
		}

		material->mRTTIData = params;
	}

	void MaterialRTTI::onSerializationEnded(IReflectable* obj)
	{
		Material* material = static_cast<Material*>(obj);

		material->mRTTIData = nullptr; // This will delete temporary data as it's stored in a unique ptr
	}

	void MaterialRTTI::onDeserializationStarted(IReflectable* obj)
	{
		// Do nothing
	}

	void MaterialRTTI::onDeserializationEnded(IReflectable* obj)
	{
		Material* material = static_cast<Material*>(obj);
		if(material->mRTTIData.empty())
			return;

		material->initBestTechnique();

		std::shared_ptr<MaterialParams> params = boost::any_cast<std::shared_ptr<MaterialParams>>(material->mRTTIData);

		vector<GpuParamsPtr>::type allParams;
		for(size_t i = 0; i < material->mParameters.size(); i++)
		{
			if(material->mParameters[i]->mFragParams != nullptr)
				allParams.push_back(material->mParameters[i]->mFragParams);

			if(material->mParameters[i]->mVertParams != nullptr)
				allParams.push_back(material->mParameters[i]->mVertParams);

			if(material->mParameters[i]->mGeomParams != nullptr)
				allParams.push_back(material->mParameters[i]->mGeomParams);
		}

		for(size_t i = 0; i < allParams.size(); i++)
		{
			const GpuParamDesc& paramDesc = allParams[i]->getParamDesc();

			for(auto iter = paramDesc.textures.begin(); iter != paramDesc.textures.end(); ++iter)
			{
				auto iterFind = params->mTextureParams.find(iter->first);

				if(iterFind != params->mTextureParams.end())
					allParams[i]->setTexture(iter->first, iterFind->second);
			}

			for(auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
			{
				auto iterFind = params->mSamplerStateParams.find(iter->first);

				if(iterFind != params->mSamplerStateParams.end())
					allParams[i]->setSamplerState(iter->first, iterFind->second);
			}

			for(auto iter = paramDesc.params.begin(); iter != paramDesc.params.end(); ++iter)
			{
				// TODO - Need to load float/int/bool parameters!
			}


			/*const GpuNamedConstants& namedConstants = allParams[i]->getConstantDefinitions();

			float tempValue[16];
			for(auto iter = namedConstants.map.begin(); iter != namedConstants.map.end(); ++iter)
			{
				const GpuConstantDefinition& def = iter->second;

				if(def.constType == GCT_SAMPLER2D)
				{
					auto iterFind = params->mTextureParams.find(iter->first);

					if(iterFind != params->mTextureParams.end())
						allParams[i]->setNamedConstant(iter->first, iterFind->second);

					auto iterFind2 = params->mSamplerStateParams.find(iter->first);

					if(iterFind2 != params->mSamplerStateParams.end())
						allParams[i]->setNamedConstant(iter->first, iterFind2->second);
				}
				else
				{
					auto iterFind = params->mFloatParams.find(iter->first);

					if(iterFind != params->mFloatParams.end() && iterFind->second.mType == def.constType)
					{
						FloatParam param = iterFind->second;
						UINT32 fieldSize = def.getElementSize(def.constType, false);

						if(fieldSize != param.mCount)
							CM_EXCEPT(InternalErrorException, "Deserializing material parameter but field sizes don't match.");

						if(fieldSize > 16)
							CM_EXCEPT(InternalErrorException, "Field size larger than the supported size.");

						for(size_t j = 0; j < fieldSize; j++)
							tempValue[j] = params->mFloatBuffer[param.mBufferIdx + j];

						allParams[i]->_writeRawConstants(def.physicalIndex, tempValue, fieldSize);
					}
				}
			}*/
		}

		material->mRTTIData = nullptr; // This will delete temporary data as it's stored in a unique ptr
	}
}