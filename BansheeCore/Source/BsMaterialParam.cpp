#include "BsMaterialParam.h"
#include "BsVector2I.h"
#include "BsVectorNI.h"

namespace BansheeEngine
{
	template<class T, bool Core>
	TMaterialDataParam<T, Core>::TMaterialDataParam(const SPtr<Vector<TGpuDataParam<T, Core>>>& params)
		:mParams(params)
	{ }

	template<class T, bool Core>
	void TMaterialDataParam<T, Core>::set(const T& value, UINT32 arrayIdx)
	{
		for (auto& param : *mParams)
			param.set(value, arrayIdx);
	}

	template<class T, bool Core>
	T TMaterialDataParam<T, Core>::get(UINT32 arrayIdx)
	{
		if (mParams->size() == 0)
			return T();

		return (*mParams)[0].get(arrayIdx); // They should all have the same value
	}

	template<bool Core>
	TMaterialParamStruct<Core>::TMaterialParamStruct(const SPtr<Vector<TGpuParamStruct<Core>>>& params)
		:mParams(params)
	{ }

	template<bool Core>
	void TMaterialParamStruct<Core>::set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		for (auto& param : *mParams)
			param.set(value, sizeBytes, arrayIdx);
	}

	template<bool Core>
	void TMaterialParamStruct<Core>::get(void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if (mParams->size() == 0)
		{
			value = nullptr;
			return;
		}

		return (*mParams)[0].get(value, sizeBytes, arrayIdx); // They should all have the same value
	}

	template<bool Core>
	UINT32 TMaterialParamStruct<Core>::getElementSize() const
	{
		if (mParams->size() == 0)
			return 0;

		return (*mParams)[0].getElementSize();
	}

	template<bool Core>
	TMaterialParamTexture<Core>::TMaterialParamTexture(const SPtr<Vector<TGpuParamTexture<Core>>>& params)
		:mParams(params)
	{ }

	template<bool Core>
	void TMaterialParamTexture<Core>::set(const TextureType& texture)
	{
		for (auto& param : *mParams)
			param.set(texture);
	}

	template<bool Core>
	typename TMaterialParamTexture<Core>::TextureType TMaterialParamTexture<Core>::get()
	{
		if (mParams->size() == 0)
			return TextureType();

		return (*mParams)[0].get(); // They should all have the same value
	}

	template<bool Core>
	TMaterialParamLoadStoreTexture<Core>::TMaterialParamLoadStoreTexture(const SPtr<Vector<TGpuParamLoadStoreTexture<Core>>>& params)
		:mParams(params)
	{ }

	template<bool Core>
	void TMaterialParamLoadStoreTexture<Core>::set(const TextureType& texture, const TextureSurface& surface)
	{
		for (auto& param : *mParams)
			param.set(texture, surface);
	}

	template<bool Core>
	typename TMaterialParamLoadStoreTexture<Core>::TextureType TMaterialParamLoadStoreTexture<Core>::get()
	{
		if (mParams->size() == 0)
			return TextureType();

		return (*mParams)[0].get(); // They should all have the same value
	}

	template<bool Core>
	TMaterialParamSampState<Core>::TMaterialParamSampState(const SPtr<Vector<TGpuParamSampState<Core>>>& params)
		:mParams(params)
	{ }

	template<bool Core>
	void TMaterialParamSampState<Core>::set(const SamplerType& sampState)
	{
		for (auto& param : *mParams)
			param.set(sampState);
	}

	template<bool Core>
	typename TMaterialParamSampState<Core>::SamplerType TMaterialParamSampState<Core>::get()
	{
		if (mParams->size() == 0)
			return SamplerType();

		return (*mParams)[0].get(); // They should all have the same value
	}

	template class TMaterialDataParam<float, false>;
	template class TMaterialDataParam<int, false>;
	template class TMaterialDataParam<Color, false>;
	template class TMaterialDataParam<Vector2, false>;
	template class TMaterialDataParam<Vector3, false>;
	template class TMaterialDataParam<Vector4, false>;
	template class TMaterialDataParam<Vector2I, false>;
	template class TMaterialDataParam<Vector3I, false>;
	template class TMaterialDataParam<Vector4I, false>;
	template class TMaterialDataParam<Matrix2, false>;
	template class TMaterialDataParam<Matrix2x3, false>;
	template class TMaterialDataParam<Matrix2x4, false>;
	template class TMaterialDataParam<Matrix3, false>;
	template class TMaterialDataParam<Matrix3x2, false>;
	template class TMaterialDataParam<Matrix3x4, false>;
	template class TMaterialDataParam<Matrix4, false>;
	template class TMaterialDataParam<Matrix4x2, false>;
	template class TMaterialDataParam<Matrix4x3, false>;

	template class TMaterialDataParam<float, true>;
	template class TMaterialDataParam<int, true>;
	template class TMaterialDataParam<Color, true>;
	template class TMaterialDataParam<Vector2, true>;
	template class TMaterialDataParam<Vector3, true>;
	template class TMaterialDataParam<Vector4, true>;
	template class TMaterialDataParam<Vector2I, true>;
	template class TMaterialDataParam<Vector3I, true>;
	template class TMaterialDataParam<Vector4I, true>;
	template class TMaterialDataParam<Matrix2, true>;
	template class TMaterialDataParam<Matrix2x3, true>;
	template class TMaterialDataParam<Matrix2x4, true>;
	template class TMaterialDataParam<Matrix3, true>;
	template class TMaterialDataParam<Matrix3x2, true>;
	template class TMaterialDataParam<Matrix3x4, true>;
	template class TMaterialDataParam<Matrix4, true>;
	template class TMaterialDataParam<Matrix4x2, true>;
	template class TMaterialDataParam<Matrix4x3, true>;

	template class TMaterialParamStruct<false>;
	template class TMaterialParamStruct<true>;

	template class TMaterialParamTexture<false>;
	template class TMaterialParamTexture<true>;

	template class TMaterialParamLoadStoreTexture<false>;
	template class TMaterialParamLoadStoreTexture<true>;

	template class TMaterialParamSampState<false>;
	template class TMaterialParamSampState<true>;
}