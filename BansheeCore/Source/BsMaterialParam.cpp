#include "BsMaterialParam.h"

namespace BansheeEngine
{
	void MaterialParamStruct::set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		for (auto& param : mParams)
			param.set(value, sizeBytes, arrayIdx);
	}

	void MaterialParamStruct::get(void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if (mParams.size() == 0)
		{
			value = nullptr;
			return;
		}

		return mParams[0].get(value, sizeBytes, arrayIdx); // They should all have the same value
	}

	UINT32 MaterialParamStruct::getElementSize() const
	{
		if (mParams.size() == 0)
			return 0;

		return mParams[0].getElementSize();
	}

	MaterialParamStruct::MaterialParamStruct(const Vector<GpuParamStruct>& params)
		:mParams(params)
	{ }

	void MaterialParamTexture::set(const HTexture& texture)
	{
		for (auto& param : mParams)
			param.set(texture);
	}

	HTexture MaterialParamTexture::get()
	{
		if (mParams.size() == 0)
			return HTexture();

		return mParams[0].get(); // They should all have the same value
	}

	MaterialParamTexture::MaterialParamTexture(const Vector<GpuParamTexture>& params)
		:mParams(params)
	{ }

	void MaterialParamLoadStoreTexture::set(const HTexture& texture, const TextureSurface& surface)
	{
		for (auto& param : mParams)
			param.set(texture, surface);
	}

	HTexture MaterialParamLoadStoreTexture::get()
	{
		if (mParams.size() == 0)
			return HTexture();

		return mParams[0].get(); // They should all have the same value
	}

	MaterialParamLoadStoreTexture::MaterialParamLoadStoreTexture(const Vector<GpuParamLoadStoreTexture>& params)
		:mParams(params)
	{ }

	void MaterialParamSampState::set(const HSamplerState& sampState)
	{
		for (auto& param : mParams)
			param.set(sampState);
	}

	HSamplerState MaterialParamSampState::get()
	{
		if (mParams.size() == 0)
			return HSamplerState();

		return mParams[0].get(); // They should all have the same value
	}

	MaterialParamSampState::MaterialParamSampState(const Vector<GpuParamSampState>& params)
		:mParams(params)
	{ }
}