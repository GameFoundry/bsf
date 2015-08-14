#include "BsShader.h"
#include "BsTechnique.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsShaderRTTI.h"
#include "BsResources.h"
#include "BsFrameAlloc.h"
#include "BsPass.h"
#include "BsSamplerState.h"

namespace BansheeEngine
{
	template<bool Core>
	TSHADER_DESC<Core>::TSHADER_DESC()
		:queuePriority(0), queueSortType(QueueSortType::None), separablePasses(false)
	{

	}

	template<bool Core>
	void TSHADER_DESC<Core>::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type,
		StringID rendererSemantic, UINT32 arraySize, UINT32 elementSize, UINT8* defaultValue)
	{
		if(type == GPDT_STRUCT && elementSize <= 0)
			BS_EXCEPT(InvalidParametersException, "You need to provide a non-zero element size for a struct parameter.")

		SHADER_DATA_PARAM_DESC desc;
		desc.name = name;
		desc.gpuVariableName = gpuVariableName;
		desc.type = type;
		desc.arraySize = arraySize;
		desc.rendererSemantic = rendererSemantic;
		desc.elementSize = elementSize;

		dataParams[name] = desc;

		if (defaultValue != nullptr)
		{
			desc.defaultValueIdx = (UINT32)dataDefaultValues.size();
			UINT32 defaultValueSize = Shader::getDataParamSize(type);

			dataDefaultValues.resize(desc.defaultValueIdx + defaultValueSize);
			memcpy(&dataDefaultValues[desc.defaultValueIdx], defaultValue, defaultValueSize);
		}
		else
			desc.defaultValueIdx = (UINT32)-1;
	}

	template<bool Core>
	void TSHADER_DESC<Core>::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, StringID rendererSemantic)
	{
		UINT32 defaultValueIdx = (UINT32)-1;

		addParameterInternal(name, gpuVariableName, type, rendererSemantic, defaultValueIdx);
	}

	template<bool Core>
	void TSHADER_DESC<Core>::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, const SamplerStateType& defaultValue, StringID rendererSemantic)
	{
		UINT32 defaultValueIdx = (UINT32)-1;
		if (Shader::isSampler(type) && defaultValue != nullptr)
		{
			defaultValueIdx = (UINT32)samplerDefaultValues.size();
			samplerDefaultValues.push_back(defaultValue);
		}

		addParameterInternal(name, gpuVariableName, type, rendererSemantic, defaultValueIdx);
	}

	template<bool Core>
	void TSHADER_DESC<Core>::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, const TextureType& defaultValue, StringID rendererSemantic)
	{
		UINT32 defaultValueIdx = (UINT32)-1;
		if (Shader::isTexture(type) && defaultValue != nullptr)
		{
			defaultValueIdx = (UINT32)textureDefaultValues.size();
			textureDefaultValues.push_back(defaultValue);
		}

		addParameterInternal(name, gpuVariableName, type, rendererSemantic, defaultValueIdx);
	}

	template<bool Core>
	void TSHADER_DESC<Core>::addParameterInternal(const String& name, const String& gpuVariableName, GpuParamObjectType type, StringID rendererSemantic, UINT32 defaultValueIdx)
	{
		Map<String, SHADER_OBJECT_PARAM_DESC>* DEST_LOOKUP[] = { &textureParams, &bufferParams, &samplerParams };
		UINT32 destIdx = 0;
		if (Shader::isBuffer(type))
			destIdx = 1;
		else if (Shader::isSampler(type))
			destIdx = 2;

		Map<String, SHADER_OBJECT_PARAM_DESC>& paramsMap = *DEST_LOOKUP[destIdx];

		auto iterFind = paramsMap.find(name);
		if (iterFind == paramsMap.end())
		{
			SHADER_OBJECT_PARAM_DESC desc;
			desc.name = name;
			desc.type = type;
			desc.rendererSemantic = rendererSemantic;
			desc.gpuVariableNames.push_back(gpuVariableName);
			desc.defaultValueIdx = defaultValueIdx;

			paramsMap[name] = desc;
		}
		else
		{
			SHADER_OBJECT_PARAM_DESC& desc = iterFind->second;

			// If same name but different properties, we ignore this param
			if (desc.type != type || desc.rendererSemantic != rendererSemantic)
				return;

			Vector<String>& gpuVariableNames = desc.gpuVariableNames;
			bool found = false;
			for (UINT32 i = 0; i < (UINT32)gpuVariableNames.size(); i++)
			{
				if (gpuVariableNames[i] == gpuVariableName)
				{
					found = true;
					break;
				}
			}

			if (!found)
				gpuVariableNames.push_back(gpuVariableName);
		}
	}

	template<bool Core>
	void TSHADER_DESC<Core>::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage, StringID rendererSemantic)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.name = name;
		desc.shared = shared;
		desc.usage = usage;
		desc.rendererSemantic = rendererSemantic;

		paramBlocks[name] = desc;
	}

	template struct TSHADER_DESC<false>;
	template struct TSHADER_DESC<true>;

	template<bool Core>
	TShader<Core>::TShader(const String& name, const TSHADER_DESC<Core>& desc, const Vector<SPtr<TechniqueType>>& techniques)
		:mName(name), mDesc(desc), mTechniques(techniques)
	{ }

	template<bool Core>
	TShader<Core>::~TShader() 
	{ }

	template<bool Core>
	GpuParamType TShader<Core>::getParamType(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return GPT_DATA;

		auto findIterTexture = mDesc.textureParams.find(name);
		if (findIterTexture != mDesc.textureParams.end())
			return GPT_TEXTURE;

		auto findIterBuffer = mDesc.bufferParams.find(name);
		if (findIterBuffer != mDesc.bufferParams.end())
			return GPT_BUFFER;

		auto findIterSampler = mDesc.samplerParams.find(name);
		if (findIterSampler != mDesc.samplerParams.end())
			return GPT_SAMPLER;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	template<bool Core>
	const SHADER_DATA_PARAM_DESC& TShader<Core>::getDataParamDesc(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return findIterData->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	template<bool Core>
	const SHADER_OBJECT_PARAM_DESC& TShader<Core>::getTextureParamDesc(const String& name) const
	{
		auto findIterObject = mDesc.textureParams.find(name);
		if (findIterObject != mDesc.textureParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	template<bool Core>
	const SHADER_OBJECT_PARAM_DESC& TShader<Core>::getSamplerParamDesc(const String& name) const
	{
		auto findIterObject = mDesc.samplerParams.find(name);
		if (findIterObject != mDesc.samplerParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	template<bool Core>
	const SHADER_OBJECT_PARAM_DESC& TShader<Core>::getBufferParamDesc(const String& name) const
	{
		auto findIterObject = mDesc.bufferParams.find(name);
		if (findIterObject != mDesc.bufferParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	template<bool Core>
	bool TShader<Core>::hasDataParam(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return true;

		return false;
	}

	template<bool Core>
	bool TShader<Core>::hasTextureParam(const String& name) const
	{
		auto findIterObject = mDesc.textureParams.find(name);
		if (findIterObject != mDesc.textureParams.end())
			return true;

		return false;
	}

	template<bool Core>
	bool TShader<Core>::hasSamplerParam(const String& name) const
	{
		auto findIterObject = mDesc.samplerParams.find(name);
		if (findIterObject != mDesc.samplerParams.end())
			return true;

		return false;
	}

	template<bool Core>
	bool TShader<Core>::hasBufferParam(const String& name) const
	{
		auto findIterObject = mDesc.bufferParams.find(name);
		if (findIterObject != mDesc.bufferParams.end())
			return true;

		return false;
	}

	template<bool Core>
	typename TShader<Core>::TextureType TShader<Core>::getDefaultTexture(UINT32 index) const
	{
		if (index < (UINT32)mDesc.textureDefaultValues.size())
			return mDesc.textureDefaultValues[index];

		return TextureType();
	}

	template<bool Core>
	typename TShader<Core>::SamplerStateType TShader<Core>::getDefaultSampler(UINT32 index) const
	{
		if (index < (UINT32)mDesc.samplerDefaultValues.size())
			return mDesc.samplerDefaultValues[index];

		return SamplerStateType();
	}

	template<bool Core>
	UINT8* TShader<Core>::getDefaultValue(UINT32 index) const
	{
		if (index < (UINT32)mDesc.dataDefaultValues.size())
			return (UINT8*)&mDesc.dataDefaultValues[index];

		return nullptr;
	}

	template<bool Core>
	SPtr<typename TShader<Core>::TechniqueType> TShader<Core>::getBestTechnique() const
	{
		for (auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
		{
			if ((*iter)->isSupported())
			{
				return *iter;
			}
		}

		return nullptr;

		// TODO - Low priority. Instead of returning null use an extremely simple technique that will be supported almost everywhere as a fallback.
	}

	template class TShader < false > ;
	template class TShader < true >;

	ShaderCore::ShaderCore(const String& name, const SHADER_DESC_CORE& desc, const Vector<SPtr<TechniqueCore>>& techniques)
		:TShader(name, desc, techniques)
	{
		
	}

	SPtr<ShaderCore> ShaderCore::create(const String& name, const SHADER_DESC_CORE& desc, const Vector<SPtr<TechniqueCore>>& techniques)
	{
		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(name, desc, techniques);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);
		shaderCorePtr->initialize();

		return shaderCorePtr;
	}

	Shader::Shader(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques)
		:TShader(name, desc, techniques)
	{
		mMetaData = bs_shared_ptr_new<ShaderMetaData>();
	}

	SPtr<ShaderCore> Shader::getCore() const
	{
		return std::static_pointer_cast<ShaderCore>(mCoreSpecific);
	}

	void Shader::setIncludeFiles(const Vector<String>& includes)
	{
		SPtr<ShaderMetaData> meta = std::static_pointer_cast<ShaderMetaData>(getMetaData());
		meta->includes = includes;
	}

	SPtr<CoreObjectCore> Shader::createCore() const
	{
		Vector<SPtr<TechniqueCore>> techniques;
		for (auto& technique : mTechniques)
			techniques.push_back(technique->getCore());

		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(mName, convertDesc(mDesc), techniques);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);

		return shaderCorePtr;
	}

	SHADER_DESC_CORE Shader::convertDesc(const SHADER_DESC& desc) const
	{
		SHADER_DESC_CORE output;
		output.dataParams = desc.dataParams;
		output.textureParams = desc.textureParams;
		output.samplerParams = desc.samplerParams;
		output.bufferParams = desc.bufferParams;
		output.paramBlocks = desc.paramBlocks;

		output.queuePriority = desc.queuePriority;
		output.queueSortType = desc.queueSortType;
		output.separablePasses = desc.separablePasses;
		
		// Ignoring default values as they are not needed for syncing since
		// they're initialized through the material.
		return output;
	}

	void Shader::getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies)
	{
		for (auto& technique : mTechniques)
			dependencies.push_back(technique);
	}

	bool Shader::isSampler(GpuParamObjectType type)
	{
		switch(type)
		{
			case GPOT_SAMPLER1D:
			case GPOT_SAMPLER2D:
			case GPOT_SAMPLER3D:
			case GPOT_SAMPLERCUBE:
			case GPOT_SAMPLER2DMS:
				return true;
		}

		return false;
	}

	bool Shader::isTexture(GpuParamObjectType type)
	{
		switch(type)
		{
		case GPOT_TEXTURE1D:
		case GPOT_TEXTURE2D:
		case GPOT_TEXTURE3D:
		case GPOT_TEXTURECUBE:
		case GPOT_TEXTURE2DMS:
			return true;
		}

		return false;
	}

	bool Shader::isBuffer(GpuParamObjectType type)
	{
		switch(type)
		{
		case GPOT_BYTE_BUFFER:
		case GPOT_STRUCTURED_BUFFER:
		case GPOT_RWBYTE_BUFFER:
		case GPOT_RWAPPEND_BUFFER:
		case GPOT_RWCONSUME_BUFFER:
		case GPOT_RWSTRUCTURED_BUFFER:
		case GPOT_RWSTRUCTURED_BUFFER_WITH_COUNTER:
		case GPOT_RWTYPED_BUFFER:
			return true;
		}

		return false;
	}

	struct ShaderDataParamsSizes
	{
		ShaderDataParamsSizes()
		{
			memset(LOOKUP, 0, sizeof(LOOKUP));

			LOOKUP[(UINT32)GPDT_FLOAT1] = 4;
			LOOKUP[(UINT32)GPDT_FLOAT2] = 8;
			LOOKUP[(UINT32)GPDT_FLOAT3] = 12;
			LOOKUP[(UINT32)GPDT_FLOAT4] = 16;
			LOOKUP[(UINT32)GPDT_MATRIX_2X2] = 16;
			LOOKUP[(UINT32)GPDT_MATRIX_2X3] = 24;
			LOOKUP[(UINT32)GPDT_MATRIX_2X4] = 32;
			LOOKUP[(UINT32)GPDT_MATRIX_3X2] = 24;
			LOOKUP[(UINT32)GPDT_MATRIX_3X3] = 36;
			LOOKUP[(UINT32)GPDT_MATRIX_3X4] = 52;
			LOOKUP[(UINT32)GPDT_MATRIX_4X2] = 32;
			LOOKUP[(UINT32)GPDT_MATRIX_4X3 ] = 52;
			LOOKUP[(UINT32)GPDT_MATRIX_4X4] = 64;
			LOOKUP[(UINT32)GPDT_INT1] = 4;
			LOOKUP[(UINT32)GPDT_INT2] = 8;
			LOOKUP[(UINT32)GPDT_INT3] = 12;
			LOOKUP[(UINT32)GPDT_INT4] = 16;
			LOOKUP[(UINT32)GPDT_BOOL] = 1;
		}

		static const UINT32 NUM_DATA_PARAMS = 25;
		UINT32 LOOKUP[NUM_DATA_PARAMS];
	};

	UINT32 Shader::getDataParamSize(GpuParamDataType type)
	{
		static const ShaderDataParamsSizes PARAM_SIZES;

		UINT32 idx = (UINT32)type;
		if (idx < sizeof(PARAM_SIZES.LOOKUP))
			return PARAM_SIZES.LOOKUP[idx];

		return 0;
	}

	HShader Shader::create(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques)
	{
		ShaderPtr newShader = _createPtr(name, desc, techniques);

		return static_resource_cast<Shader>(gResources()._createResourceHandle(newShader));
	}

	ShaderPtr Shader::_createPtr(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques)
	{
		ShaderPtr newShader = bs_core_ptr<Shader>(new (bs_alloc<Shader>()) Shader(name, desc, techniques));
		newShader->_setThisPtr(newShader);
		newShader->initialize();

		return newShader;
	}

	ShaderPtr Shader::createEmpty()
	{
		ShaderPtr newShader = bs_core_ptr<Shader>(new (bs_alloc<Shader>()) Shader());
		newShader->_setThisPtr(newShader);

		return newShader;
	}

	RTTITypeBase* Shader::getRTTIStatic()
	{
		return ShaderRTTI::instance();
	}

	RTTITypeBase* Shader::getRTTI() const
	{
		return Shader::getRTTIStatic();
	}

	RTTITypeBase* ShaderMetaData::getRTTIStatic()
	{
		return ShaderMetaDataRTTI::instance();
	}

	RTTITypeBase* ShaderMetaData::getRTTI() const
	{
		return ShaderMetaData::getRTTIStatic();
	}
}