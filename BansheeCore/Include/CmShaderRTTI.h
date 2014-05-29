#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmShader.h"
#include "CmMaterial.h"

namespace BansheeEngine
{
	template<> struct RTTIPlainType<SHADER_DATA_PARAM_DESC>
	{	
		enum { id = TID_SHADER_DATA_PARAM_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const SHADER_DATA_PARAM_DESC& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(data.arraySize, memory);
			memory = rttiWriteElem(data.hidden, memory);
			memory = rttiWriteElem(data.type, memory);
			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.gpuVariableName, memory);
			memory = rttiWriteElem(data.elementSize, memory);
		}

		static UINT32 fromMemory(SHADER_DATA_PARAM_DESC& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.arraySize, memory);
			memory = rttiReadElem(data.hidden, memory);
			memory = rttiReadElem(data.type, memory);
			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.gpuVariableName, memory);
			memory = rttiReadElem(data.elementSize, memory);

			return size;
		}

		static UINT32 getDynamicSize(const SHADER_DATA_PARAM_DESC& data)	
		{ 
			UINT64 dataSize = rttiGetElemSize(data.arraySize) + rttiGetElemSize(data.hidden) + rttiGetElemSize(data.type) + 
				rttiGetElemSize(data.name) + rttiGetElemSize(data.gpuVariableName) + rttiGetElemSize(data.elementSize) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				BS_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	template<> struct RTTIPlainType<SHADER_OBJECT_PARAM_DESC>
	{	
		enum { id = TID_SHADER_OBJECT_PARAM_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const SHADER_OBJECT_PARAM_DESC& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(data.hidden, memory);
			memory = rttiWriteElem(data.type, memory);
			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.gpuVariableName, memory);
		}

		static UINT32 fromMemory(SHADER_OBJECT_PARAM_DESC& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.hidden, memory);
			memory = rttiReadElem(data.type, memory);
			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.gpuVariableName, memory);

			return size;
		}

		static UINT32 getDynamicSize(const SHADER_OBJECT_PARAM_DESC& data)	
		{ 
			UINT64 dataSize = rttiGetElemSize(data.hidden) + rttiGetElemSize(data.type) + 
				rttiGetElemSize(data.name) + rttiGetElemSize(data.gpuVariableName) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				BS_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	template<> struct RTTIPlainType<SHADER_PARAM_BLOCK_DESC>
	{	
		enum { id = TID_SHADER_PARAM_BLOCK_DESC }; enum { hasDynamicSize = 1 };

		static void toMemory(const SHADER_PARAM_BLOCK_DESC& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			UINT32 curSize = sizeof(UINT32);
			memcpy(memory, &size, curSize);
			memory += curSize;

			memory = rttiWriteElem(data.shared, memory);
			memory = rttiWriteElem(data.usage, memory);
			memory = rttiWriteElem(data.name, memory);
		}

		static UINT32 fromMemory(SHADER_PARAM_BLOCK_DESC& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.shared, memory);
			memory = rttiReadElem(data.usage, memory);
			memory = rttiReadElem(data.name, memory);

			return size;
		}

		static UINT32 getDynamicSize(const SHADER_PARAM_BLOCK_DESC& data)	
		{ 
			UINT64 dataSize = rttiGetElemSize(data.shared) + rttiGetElemSize(data.usage) + 
				rttiGetElemSize(data.name) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				BS_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	class BS_CORE_EXPORT ShaderRTTI : public RTTIType<Shader, Resource, ShaderRTTI>
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
			return Shader::create("");
		}
	};
}