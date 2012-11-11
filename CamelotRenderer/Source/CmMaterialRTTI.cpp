#include "CmMaterialRTTI.h"

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

		vector<GpuProgramParametersPtr>::type allParams;
		for(size_t i = 0; i < material->mParameters.size(); i++)
		{
			if(material->mParameters[i].mFragParams != nullptr)
				allParams.push_back(material->mParameters[i].mFragParams);

			if(material->mParameters[i].mVertParams != nullptr)
				allParams.push_back(material->mParameters[i].mVertParams);

			if(material->mParameters[i].mGeomParams != nullptr)
				allParams.push_back(material->mParameters[i].mGeomParams);
		}

		for(size_t i = 0; i < allParams.size(); i++)
		{
			const GpuNamedConstants& namedConstants = allParams[i]->getConstantDefinitions();

			float tempValue[16];
			for(auto iter = namedConstants.map.begin(); iter != namedConstants.map.end(); ++iter)
			{
				const GpuConstantDefinition& def = iter->second;

				if(def.constType == GCT_SAMPLER2D)
				{
					TextureRef value;
					allParams[i]->_readTexture(iter->second.physicalIndex, value);
					params->mTextureParams[iter->first] = value;
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
						params->mFloatParams[iter->first] = FloatParam(params->mFloatBuffer.size(), def.constType, fieldSize);

						for(size_t j = 0; j < fieldSize; j++)
							params->mFloatBuffer.push_back(tempValue[j]);
					}
					else
						gDebug().logWarning("While saving material found multiple parameters with the same name. Only saving the first.");
				}
			}
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
		//std::shared_ptr<MaterialParams> params = boost::any_cast<std::shared_ptr<MaterialParams>>(material->mRTTIData);

		//vector<GpuProgramParametersPtr>::type allParams;
		//for(size_t i = 0; i < material->mParameters.size(); i++)
		//{
		//	if(material->mParameters[i].mFragParams != nullptr)
		//		allParams.push_back(material->mParameters[i].mFragParams);

		//	if(material->mParameters[i].mVertParams != nullptr)
		//		allParams.push_back(material->mParameters[i].mVertParams);

		//	if(material->mParameters[i].mGeomParams != nullptr)
		//		allParams.push_back(material->mParameters[i].mGeomParams);
		//}

		//for(size_t i = 0; i < allParams.size(); i++)
		//{
		//	const GpuNamedConstants& namedConstants = allParams[i]->getConstantDefinitions();

		//	float tempValue[16];
		//	for(auto iter = namedConstants.map.begin(); iter != namedConstants.map.end(); ++iter)
		//	{
		//		const GpuConstantDefinition& def = iter->second;

		//		if(def.constType == GCT_SAMPLER2D)
		//		{
		//			auto iterFind = params->mTextureParams.find(iter->first);

		//			if(iterFind != params->mTextureParams.end())
		//				allParams[i]->setNamedConstant(iter->first, iterFind->second);
		//		}
		//		else
		//		{
		//			auto iterFind = params->mFloatParams.find(iter->first);

		//			if(iterFind != params->mFloatParams.end() && iterFind->second.mType == def.constType)
		//			{
		//				FloatParam param = iterFind->second;
		//				UINT32 fieldSize = def.getElementSize(def.constType, false);

		//				if(fieldSize != param.mCount)
		//					CM_EXCEPT(InternalErrorException, "Deserializing material parameter but field sizes don't match.");

		//				if(fieldSize > 16)
		//					CM_EXCEPT(InternalErrorException, "Field size larger than the supported size.");

		//				for(size_t j = 0; j < fieldSize; j++)
		//					tempValue[j] = params->mFloatBuffer[param.mBufferIdx + j];

		//				allParams[i]->_writeRawConstants(def.physicalIndex, tempValue, fieldSize);
		//			}
		//		}
		//	}
		//}

		material->mRTTIData = nullptr; // This will delete temporary data as it's stored in a unique ptr

		//material->initBestTechnique();
	}
}