#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmShader.h"

namespace CamelotEngine
{
	class CM_EXPORT ShaderRTTI : public RTTIType<Shader, Resource, ShaderRTTI>
	{
	private:
		TechniquePtr getTechnique(Shader* obj, UINT32 idx) { return obj->mTechniques[idx]; }
		void setTechnique(Shader* obj, UINT32 idx, TechniquePtr val) { obj->mTechniques[idx] = val; }
		UINT32 getTechniqueArraySize(Shader* obj) { return (UINT32)obj->mTechniques.size(); }
		void setTechniqueArraySize(Shader* obj, UINT32 size) { obj->mTechniques.resize(size); }

		String& getName(Shader* obj) { return obj->mName; }
		void setName(Shader* obj, String& name) { obj->mName = name; }

		SHADER_DATA_PARAM_DESC& getDataParam(Shader* obj, UINT32 idx) 
		{ 
			auto iter = obj->mDataParams.begin();
			for(UINT32 i = 0; i < idx; i++) ++iter;

			return iter->second; 
		}

		void setDataParam(Shader* obj, UINT32 idx, SHADER_DATA_PARAM_DESC& val) { obj->mDataParams[val.name] = val; }
		UINT32 getDataParamsArraySize(Shader* obj) { return (UINT32)obj->mDataParams.size(); }
		void setDataParamsArraySize(Shader* obj, UINT32 size) {  } // Do nothing

		SHADER_OBJECT_PARAM_DESC& getObjectParam(Shader* obj, UINT32 idx) 
		{ 
			auto iter = obj->mObjectParams.begin();
			for(UINT32 i = 0; i < idx; i++) ++iter;

			return iter->second; 
		}

		void setObjectParam(Shader* obj, UINT32 idx, SHADER_OBJECT_PARAM_DESC& val) { obj->mObjectParams[val.name] = val; }
		UINT32 getObjectParamsArraySize(Shader* obj) { return (UINT32)obj->mObjectParams.size(); }
		void setObjectParamsArraySize(Shader* obj, UINT32 size) {  } // Do nothing

		SHADER_PARAM_BLOCK_DESC& getParamBlock(Shader* obj, UINT32 idx) 
		{ 
			auto iter = obj->mParamBlocks.begin();
			for(UINT32 i = 0; i < idx; i++) ++iter;

			return iter->second; 
		}

		void setParamBlock(Shader* obj, UINT32 idx, SHADER_PARAM_BLOCK_DESC& val) { obj->mParamBlocks[val.name] = val; }
		UINT32 getParamBlocksArraySize(Shader* obj) { return (UINT32)obj->mParamBlocks.size(); }
		void setParamBlocksArraySize(Shader* obj, UINT32 size) {  } // Do nothing

	public:
		ShaderRTTI()
		{
			addReflectablePtrArrayField("mTechniques", 0, &ShaderRTTI::getTechnique, &ShaderRTTI::getTechniqueArraySize, 
				&ShaderRTTI::setTechnique, &ShaderRTTI::setTechniqueArraySize);
			addPlainField("mName", 1, &ShaderRTTI::getName, &ShaderRTTI::setName);

			addPlainArrayField("mDataParams", 2, &ShaderRTTI::getDataParam, &ShaderRTTI::getDataParamsArraySize, 
				&ShaderRTTI::setDataParam, &ShaderRTTI::setDataParamsArraySize);
			addPlainArrayField("mObjectParams", 3, &ShaderRTTI::getObjectParam, &ShaderRTTI::getObjectParamsArraySize, 
				&ShaderRTTI::setObjectParam, &ShaderRTTI::setObjectParamsArraySize);
			addPlainArrayField("mParamBlocks", 4, &ShaderRTTI::getParamBlock, &ShaderRTTI::getParamBlocksArraySize, 
				&ShaderRTTI::setParamBlock, &ShaderRTTI::setParamBlocksArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Shader";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Shader;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<Shader>(new Shader());
		}
	};
}