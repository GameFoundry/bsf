#include "BsMaterialParams.h"
#include "BsProfilerCPU.h"

namespace BansheeEngine
{
	__MaterialParams::__MaterialParams(const HShader& shader)
	{
		gProfilerCPU().beginSample("Create material params");

		UINT32 bufferSize = 0;

		auto& dataParams = shader->getDataParams();
		for (auto& param : dataParams)
		{
			UINT32 arraySize = param.second.arraySize > 1 ? param.second.arraySize : 1;
			const GpuParamDataTypeInfo& typeInfo = GpuParams::PARAM_SIZES.lookup[(int)param.second.type];
			UINT32 paramSize = typeInfo.numColumns * typeInfo.numRows * typeInfo.baseTypeSize;

			bufferSize += arraySize * paramSize;

		}

		auto& textureParams = shader->getTextureParams();
		auto& samplerParams = shader->getSamplerParams();

		mNumTextureParams = (UINT32)textureParams.size();
		mNumSamplerParams = (UINT32)samplerParams.size();

		mDataParamsBuffer = mAlloc.alloc(bufferSize);
		memset(mDataParamsBuffer, 0, bufferSize);

		mStructParams = mAlloc.construct<StructParamData>(mNumStructParams);
		mTextureParams = mAlloc.construct<TextureParamData>(mNumTextureParams);
		mSamplerStateParams = mAlloc.construct<SamplerStatePtr>(mNumSamplerParams);

		UINT32 mDataBufferIdx = 0;
		UINT32 mStructIdx = 0;
		UINT32 mTextureIdx = 0;
		UINT32 mSamplerIdx = 0;

		for (auto& entry : dataParams)
		{
			ParamData& dataParam = mParams[entry.first];

			UINT32 arraySize = entry.second.arraySize > 1 ? entry.second.arraySize : 1;
			dataParam.arraySize = arraySize;
			dataParam.type = ParamType::Data;
			dataParam.dataType = entry.second.type;
			
			const GpuParamDataTypeInfo& typeInfo = GpuParams::PARAM_SIZES.lookup[(int)dataParam.dataType];
			UINT32 paramSize = typeInfo.numColumns * typeInfo.numRows * typeInfo.baseTypeSize;

			dataParam.index = mDataBufferIdx;
			mDataBufferIdx += arraySize * paramSize;

			if(entry.second.type == GPDT_STRUCT)
			{
				for (UINT32 i = 0; i < arraySize; i++)
				{
					StructParamData& param = mStructParams[mStructIdx];
					param.dataSize = entry.second.elementSize;
					param.data = mAlloc.alloc(param.dataSize);

					dataParam.index = mStructIdx;
					mStructIdx++;
				}
			}
		}

		for (auto& entry : textureParams)
		{
			ParamData& dataParam = mParams[entry.first];

			dataParam.arraySize = 1;
			dataParam.type = ParamType::Texture;
			dataParam.dataType = GPDT_UNKNOWN;
			dataParam.index = mTextureIdx;

			TextureParamData& param = mTextureParams[mTextureIdx];
			param.isLoadStore = false;

			mTextureIdx++;
		}

		for (auto& entry : samplerParams)
		{
			ParamData& dataParam = mParams[entry.first];

			dataParam.arraySize = 1;
			dataParam.type = ParamType::Sampler;
			dataParam.dataType = GPDT_UNKNOWN;
			dataParam.index = mSamplerIdx;

			mSamplerIdx++;
		}

		gProfilerCPU().endSample("Create material params");
	}

	__MaterialParams::~__MaterialParams()
	{
		if (mStructParams != nullptr)
		{
			for (UINT32 i = 0; mNumStructParams; i++)
				mAlloc.free(mStructParams[i].data);
		}

		mAlloc.free(mDataParamsBuffer);
		mAlloc.destruct(mStructParams, mNumStructParams);
		mAlloc.destruct(mTextureParams, mNumTextureParams);
		mAlloc.destruct(mSamplerStateParams, mNumSamplerParams);

		mAlloc.clear();
	}

	void __MaterialParams::getStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx) const
	{
		const ParamData* param = getParamData(name, ParamType::Data, GPDT_STRUCT, arrayIdx);
		if (param == nullptr)
			return;

		getStructData(param->index + arrayIdx, value, size);
	}

	void __MaterialParams::setStructData(const String& name, const void* value, UINT32 size, UINT32 arrayIdx)
	{
		const ParamData* param = getParamData(name, ParamType::Data, GPDT_STRUCT, arrayIdx);
		if (param == nullptr)
			return;

		setStructData(param->index + arrayIdx, value, size);
	}

	void __MaterialParams::getTexture(const String& name, HTexture& value) const
	{
		const ParamData* param = getParamData(name, ParamType::Texture, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		getTexture(param->index, value);
	}

	void __MaterialParams::setTexture(const String& name, const HTexture& value)
	{
		const ParamData* param = getParamData(name, ParamType::Texture, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		setTexture(param->index, value);
	}

	void __MaterialParams::getLoadStoreTexture(const String& name, HTexture& value, TextureSurface& surface) const
	{
		const ParamData* param = getParamData(name, ParamType::Texture, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		getLoadStoreTexture(param->index, value, surface);
	}

	void __MaterialParams::setLoadStoreTexture(const String& name, const HTexture& value, const TextureSurface& surface)
	{
		const ParamData* param = getParamData(name, ParamType::Texture, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		setLoadStoreTexture(param->index, value, surface);
	}

	void __MaterialParams::getSamplerState(const String& name, SamplerStatePtr& value) const
	{
		const ParamData* param = getParamData(name, ParamType::Sampler, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		getSamplerState(param->index, value);
	}

	void __MaterialParams::setSamplerState(const String& name, const SamplerStatePtr& value)
	{
		const ParamData* param = getParamData(name, ParamType::Sampler, GPDT_UNKNOWN, 0);
		if (param == nullptr)
			return;

		setSamplerState(param->index, value);
	}

	const __MaterialParams::ParamData* __MaterialParams::getParamData(const String& name, ParamType type, GpuParamDataType dataType,
		UINT32 arrayIdx) const
	{
		auto iterFind = mParams.find(name);
		if (iterFind == mParams.end())
		{
			LOGWRN("Material doesn't have a parameter named " + name + ".");
			return nullptr;
		}

		const ParamData& param = iterFind->second;
		if (param.type != type || (type == ParamType::Data && param.dataType != dataType))
		{
			LOGWRN("Parameter \"" + name + "\" is not of the requested type.");
			return nullptr;
		}

		if (arrayIdx >= param.arraySize)
		{
			LOGWRN("Parameter \"" + name + "\" array index " + toString(arrayIdx) + " out of range. Array length is " +
				toString(param.arraySize) + ".");
			return nullptr;
		}

		return &param;
	}

	void __MaterialParams::getStructData(UINT32 index, void* value, UINT32 size) const
	{
		const StructParamData& structParam = mStructParams[index];
		if (structParam.dataSize != size)
		{
			LOGWRN("Size mismatch when writing to a struct. Provided size was " + toString(size) + " bytes but the "
				"struct size is" + toString(structParam.dataSize) + " bytes");
			return;
		}

		memcpy(value, structParam.data, structParam.dataSize);
	}

	void __MaterialParams::setStructData(UINT32 index, const void* value, UINT32 size)
	{
		const StructParamData& structParam = mStructParams[index];
		if (structParam.dataSize != size)
		{
			LOGWRN("Size mismatch when writing to a struct. Provided size was " + toString(size) + " bytes but the "
				"struct size is" + toString(structParam.dataSize) + " bytes");
			return;
		}

		memcpy(structParam.data, value, structParam.dataSize);
	}

	UINT32 __MaterialParams::getStructSize(UINT32 index) const
	{
		const StructParamData& structParam = mStructParams[index];
		return structParam.dataSize;
	}

	void __MaterialParams::getTexture(UINT32 index, HTexture& value) const
	{
		TextureParamData& textureParam = mTextureParams[index];
		value = textureParam.value;
	}

	void __MaterialParams::setTexture(UINT32 index, const HTexture& value)
	{
		TextureParamData& textureParam = mTextureParams[index];
		textureParam.value = value;
		textureParam.isLoadStore = false;
	}

	void __MaterialParams::getLoadStoreTexture(UINT32 index, HTexture& value, TextureSurface& surface) const
	{
		TextureParamData& textureParam = mTextureParams[index];
		value = textureParam.value;
		surface = textureParam.surface;
	}

	void __MaterialParams::setLoadStoreTexture(UINT32 index, const HTexture& value, const TextureSurface& surface)
	{
		TextureParamData& textureParam = mTextureParams[index];
		textureParam.value = value;
		textureParam.isLoadStore = true;
		textureParam.surface = surface;
	}

	void __MaterialParams::getSamplerState(UINT32 index, SamplerStatePtr& value) const
	{
		value = mSamplerStateParams[index];
	}

	void __MaterialParams::setSamplerState(UINT32 index, const SamplerStatePtr& value)
	{
		mSamplerStateParams[index] = value;
	}
}