#include "BsMaterialParams.h"

namespace BansheeEngine
{
	__MaterialParams::__MaterialParams(const HShader& shader)
	{
		GpuParamDataType dataTypes[] = { GPDT_FLOAT1, GPDT_FLOAT2, GPDT_FLOAT3, GPDT_FLOAT4, GPDT_INT1, GPDT_INT2, GPDT_INT3,
			GPDT_INT4, GPDT_MATRIX_2X2, GPDT_MATRIX_2X3, GPDT_MATRIX_2X4, GPDT_MATRIX_3X3, GPDT_MATRIX_3X2, GPDT_MATRIX_3X4,
			GPDT_MATRIX_4X4, GPDT_MATRIX_4X2, GPDT_MATRIX_4X3, GPDT_COLOR, GPDT_STRUCT };
		UINT32* dataCounts[] = { &mNumFloatParams, &mNumVec2Params, &mNumVec3Params, &mNumVec4Params, &mNumIntParams, 
			&mNumVec2IParams, &mNumVec3IParams, &mNumVec4IParams, &mNumMat2Params, &mNumMat2x3Params, &mNumMat2x4Params, 
			&mNumMat3Params, &mNumMat3x2Params, &mNumMat3x4Params, &mNumMat4Params, &mNumMat4x2Params, &mNumMat4x3Params, 
			&mNumColorParams, &mNumStructParams };

		auto& dataParams = shader->getDataParams();
		for (auto& param : dataParams)
		{
			for (UINT32 i = 0; i < sizeof(dataTypes); i++)
			{
				if (param.second.type == dataTypes[i])
				{
					UINT32 count = param.second.arraySize > 1 ? param.second.arraySize : 1;

					*dataCounts[i] += count;
					break;
				}
			}
		}

		auto& textureParams = shader->getTextureParams();
		auto& samplerParams = shader->getSamplerParams();

		mNumTextureParams = (UINT32)textureParams.size();
		mNumSamplerParams = (UINT32)samplerParams.size();

		mFloatParams = mAlloc.construct<float>(mNumFloatParams);
		mVec2Params = mAlloc.construct<Vector2>(mNumVec2Params);
		mVec3Params = mAlloc.construct<Vector3>(mNumVec3Params);
		mVec4Params = mAlloc.construct<Vector4>(mNumVec4Params);

		mIntParams = mAlloc.construct<int>(mNumIntParams);
		mVec2IParams = mAlloc.construct<Vector2I>(mNumVec2IParams);
		mVec3IParams = mAlloc.construct<Vector3I>(mNumVec3IParams);
		mVec4IParams = mAlloc.construct<Vector4I>(mNumVec4IParams);

		mMat2Params = mAlloc.construct<Matrix2>(mNumMat2Params);
		mMat2x3Params = mAlloc.construct<Matrix2x3>(mNumMat2x3Params);
		mMat2x4Params = mAlloc.construct<Matrix2x4>(mNumMat2x4Params);
		mMat3Params = mAlloc.construct<Matrix3>(mNumMat3Params);
		mMat3x2Params = mAlloc.construct<Matrix3x2>(mNumMat3x2Params);
		mMat3x4Params = mAlloc.construct<Matrix3x4>(mNumMat3x4Params);
		mMat4Params = mAlloc.construct<Matrix4>(mNumMat4Params);
		mMat4x2Params = mAlloc.construct<Matrix4x2>(mNumMat4x2Params);
		mMat4x3Params = mAlloc.construct<Matrix4x3>(mNumMat4x3Params);

		mColorParams = mAlloc.construct<Color>(mNumColorParams);
		mStructParams = mAlloc.construct<StructParamData>(mNumStructParams);
		mTextureParams = mAlloc.construct<TextureParamData>(mNumTextureParams);
		mSamplerStateParams = mAlloc.construct<SamplerStatePtr>(mNumSamplerParams);

		UINT32 mFloatIdx = 0;
		UINT32 mVec2Idx = 0;
		UINT32 mVec3Idx = 0;
		UINT32 mVec4Idx = 0;
		UINT32 mIntIdx = 0;
		UINT32 mVec2IIdx = 0;
		UINT32 mVec3IIdx = 0;
		UINT32 mVec4IIdx = 0;
		UINT32 mMat2Idx = 0;
		UINT32 mMat2x3Idx = 0;
		UINT32 mMat2x4Idx = 0;
		UINT32 mMat3Idx = 0;
		UINT32 mMat3x2Idx = 0;
		UINT32 mMat3x4Idx = 0;
		UINT32 mMat4Idx = 0;
		UINT32 mMat4x2Idx = 0;
		UINT32 mMat4x3Idx = 0;
		UINT32 mColorIdx = 0;
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
			dataParam.index = 0;

			switch (entry.second.type)
			{
			case GPDT_FLOAT1:
				for (UINT32 i = 0; i < arraySize; i++)
					mFloatParams[mFloatIdx + i] = 0.0f;

				dataParam.index = mFloatIdx;
				mFloatIdx += arraySize;
				break;
			case GPDT_FLOAT2:
				dataParam.index = mVec2Idx;
				mVec2Idx += arraySize;
				break;
			case GPDT_FLOAT3:
				dataParam.index = mVec3Idx;
				mVec3Idx += arraySize;
				break;
			case GPDT_FLOAT4:
				dataParam.index = mVec4Idx;
				mVec4Idx += arraySize;
				break;
			case GPDT_INT1:
				dataParam.index = mIntIdx;
				mIntIdx += arraySize;
				break;
			case GPDT_INT2:
				dataParam.index = mVec2IIdx;
				mVec2IIdx += arraySize;
				break;
			case GPDT_INT3:
				dataParam.index = mVec3IIdx;
				mVec3IIdx += arraySize;
				break;
			case GPDT_INT4:
				dataParam.index = mVec4IIdx;
				mVec4IIdx += arraySize;
				break;
			case GPDT_MATRIX_2X2:
				dataParam.index = mMat2Idx;
				mMat2Idx += arraySize;
				break;
			case GPDT_MATRIX_2X3:
				dataParam.index = mMat2x3Idx;
				mMat2x3Idx += arraySize;
				break;
			case GPDT_MATRIX_2X4:
				dataParam.index = mMat2x4Idx;
				mMat2x4Idx += arraySize;
				break;
			case GPDT_MATRIX_3X3:
				dataParam.index = mMat3Idx;
				mMat3Idx += arraySize;
				break;
			case GPDT_MATRIX_3X2:
				dataParam.index = mMat3x2Idx;
				mMat3x2Idx += arraySize;
				break;
			case GPDT_MATRIX_3X4:
				dataParam.index = mMat3x4Idx;
				mMat3x4Idx += arraySize;
				break;
			case GPDT_MATRIX_4X4:
				dataParam.index = mMat4Idx;
				mMat4Idx += arraySize;
				break;
			case GPDT_MATRIX_4X2:
				dataParam.index = mMat4x2Idx;
				mMat4x2Idx += arraySize;
				break;
			case GPDT_MATRIX_4X3:
				dataParam.index = mMat4x3Idx;
				mMat4x3Idx += arraySize;
				break;
			case GPDT_COLOR:
				dataParam.index = mColorIdx;
				mColorIdx += arraySize;
				break;
			case GPDT_STRUCT:
			{
				StructParamData& param = mStructParams[mStructIdx];
				param.dataSize = entry.second.elementSize;
				param.data = mAlloc.alloc(param.dataSize);

				dataParam.index = mStructIdx;
				mStructIdx += arraySize;
			}
			break;
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
	}

	__MaterialParams::~__MaterialParams()
	{
		if (mStructParams != nullptr)
		{
			for (UINT32 i = 0; mNumStructParams; i++)
				mAlloc.free(mStructParams[i].data);
		}

		mAlloc.destruct(mFloatParams, mNumFloatParams);
		mAlloc.destruct(mVec2Params, mNumVec2Params);
		mAlloc.destruct(mVec3Params, mNumVec3Params);
		mAlloc.destruct(mVec4Params, mNumVec4Params);

		mAlloc.destruct(mIntParams, mNumIntParams);
		mAlloc.destruct(mVec2IParams, mNumVec2IParams);
		mAlloc.destruct(mVec3IParams, mNumVec3IParams);
		mAlloc.destruct(mVec4IParams, mNumVec4IParams);

		mAlloc.destruct(mMat2Params, mNumMat2Params);
		mAlloc.destruct(mMat2x3Params, mNumMat2x3Params);
		mAlloc.destruct(mMat2x4Params, mNumMat2x4Params);

		mAlloc.destruct(mMat3Params, mNumMat3Params);
		mAlloc.destruct(mMat3x2Params, mNumMat3x2Params);
		mAlloc.destruct(mMat3x4Params, mNumMat3x4Params);

		mAlloc.destruct(mMat4Params, mNumMat4Params);
		mAlloc.destruct(mMat4x2Params, mNumMat4x2Params);
		mAlloc.destruct(mMat4x3Params, mNumMat4x3Params);

		mAlloc.destruct(mColorParams, mNumColorParams);
		mAlloc.destruct(mStructParams, mNumStructParams);
		mAlloc.destruct(mTextureParams, mNumTextureParams);
		mAlloc.destruct(mSamplerStateParams, mNumSamplerParams);

		mAlloc.clear();
	}

	void __MaterialParams::getStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx)
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

	void __MaterialParams::getTexture(const String& name, HTexture& value)
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

	void __MaterialParams::getLoadStoreTexture(const String& name, HTexture& value, TextureSurface& surface)
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

	void __MaterialParams::getSamplerState(const String& name, SamplerStatePtr& value)
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
		UINT32 arrayIdx)
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

	void __MaterialParams::getStructData(UINT32 index, void* value, UINT32 size)
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

	void __MaterialParams::getTexture(UINT32 index, HTexture& value)
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

	void __MaterialParams::getLoadStoreTexture(UINT32 index, HTexture& value, TextureSurface& surface)
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

	void __MaterialParams::getSamplerState(UINT32 index, SamplerStatePtr& value)
	{
		value = mSamplerStateParams[index];
	}

	void __MaterialParams::setSamplerState(UINT32 index, const SamplerStatePtr& value)
	{
		mSamplerStateParams[index] = value;
	}
}