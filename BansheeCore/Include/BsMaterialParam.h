#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	/**
	 * @brief	A wrapper class that allows you to manage multiple GPU parameters
	 *			at once.
	 *
	 * @see		TGpuDataParam
	 */
	template<class T, bool Core>
	class BS_CORE_EXPORT TMaterialDataParam
	{
	public:
		TMaterialDataParam(const Vector<TGpuDataParam<T, Core>>& params)
			:mParams(params)
		{ }

		TMaterialDataParam() { }
		
		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const T& value, UINT32 arrayIdx = 0)
		{
			for (auto& param : mParams)
				param.set(value, arrayIdx);
		}

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		T get(UINT32 arrayIdx = 0)
		{
			if (mParams.size() == 0)
				return T();

			return mParams[0].get(arrayIdx); // They should all have the same value
		}

	protected:
		Vector<TGpuDataParam<T, Core>> mParams;
	};

	typedef TMaterialDataParam<float, false> MaterialParamFloat;
	typedef TMaterialDataParam<Color, false> MaterialParamColor;
	typedef TMaterialDataParam<Vector2, false> MaterialParamVec2;
	typedef TMaterialDataParam<Vector3, false> MaterialParamVec3;
	typedef TMaterialDataParam<Vector4, false> MaterialParamVec4;
	typedef TMaterialDataParam<Matrix3, false> MaterialParamMat3;
	typedef TMaterialDataParam<Matrix4, false> MaterialParamMat4;

	typedef TMaterialDataParam<float, true> MaterialParamFloatCore;
	typedef TMaterialDataParam<Color, true> MaterialParamColorCore;
	typedef TMaterialDataParam<Vector2, true> MaterialParamVec2Core;
	typedef TMaterialDataParam<Vector3, true> MaterialParamVec3Core;
	typedef TMaterialDataParam<Vector4, true> MaterialParamVec4Core;
	typedef TMaterialDataParam<Matrix3, true> MaterialParamMat3Core;
	typedef TMaterialDataParam<Matrix4, true> MaterialParamMat4Core;

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamStruct
	{
	public:
		TMaterialParamStruct(const Vector<TGpuParamStruct<Core>>& params)
			:mParams(params)
		{ }

		TMaterialParamStruct() { }

		/**
		 * @copydoc	TGpuParamStruct::set
		 */
		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0)
		{
			for (auto& param : mParams)
				param.set(value, sizeBytes, arrayIdx);
		}

		/**
		 * @copydoc	TGpuParamStruct::get
		 */
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0)
		{
			if (mParams.size() == 0)
			{
				value = nullptr;
				return;
			}

			return mParams[0].get(value, sizeBytes, arrayIdx); // They should all have the same value
		}

		/**
		 * @copydoc	TGpuParamStruct::getElementSize
		 */
		UINT32 getElementSize() const
		{
			if (mParams.size() == 0)
				return 0;

			return mParams[0].getElementSize();
		}

	protected:
		Vector<TGpuParamStruct<Core>> mParams;
	};

	typedef TMaterialParamStruct<false> MaterialParamStruct;
	typedef TMaterialParamStruct<true> MaterialParamStructCore;

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamTexture
	{
	public:
		typedef typename TGpuParamTextureType<Core>::Type TextureType;

		TMaterialParamTexture(const Vector<TGpuParamTexture<Core>>& params)
			:mParams(params)
		{ }

		TMaterialParamTexture() { }

		/**
		 * @copydoc	GpuParamTexture::set
		 */
		void set(const TextureType& texture)
		{
			for (auto& param : mParams)
				param.set(texture);
		}

		/**
		 * @copydoc	GpuParamTexture::get
		 */
		TextureType get()
		{
			if (mParams.size() == 0)
				return TextureType();

			return mParams[0].get(); // They should all have the same value
		}

	protected:
		Vector<TGpuParamTexture<Core>> mParams;
	};

	typedef TMaterialParamTexture<false> MaterialParamTexture;
	typedef TMaterialParamTexture<true> MaterialParamTextureCore;

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamLoadStoreTexture
	{
	public:
		typedef typename TGpuParamTextureType<Core>::Type TextureType;

		TMaterialParamLoadStoreTexture(const Vector<TGpuParamLoadStoreTexture<Core>>& params)
			:mParams(params)
		{ }

		TMaterialParamLoadStoreTexture() { }

		/**
		 * @copydoc	GpuParamLoadStoreTexture::set
		 */
		void set(const TextureType& texture, const TextureSurface& surface)
		{
			for (auto& param : mParams)
				param.set(texture, surface);
		}

		/**
		 * @copydoc	GpuParamLoadStoreTexture::get
		 */
		TextureType get()
		{
			if (mParams.size() == 0)
				return TextureType();

			return mParams[0].get(); // They should all have the same value
		}

	protected:
		Vector<TGpuParamLoadStoreTexture<Core>> mParams;
	};

	typedef TMaterialParamLoadStoreTexture<false> MaterialParamLoadStoreTexture;
	typedef TMaterialParamLoadStoreTexture<true> MaterialParamLoadStoreTextureCore;

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamSampState
	{
	public:
		typedef typename TGpuParamSamplerStateType<Core>::Type SamplerType;

		TMaterialParamSampState(const Vector<TGpuParamSampState<Core>>& params)
			:mParams(params)
		{ }

		TMaterialParamSampState() { }

		/**
		 * @copydoc	GpuParamSampState::set
		 */
		void set(const SamplerType& sampState)
		{
			for (auto& param : mParams)
				param.set(sampState);
		}

		/**
		 * @copydoc	GpuParamSampState::get
		 */
		SamplerType get()
		{
			if (mParams.size() == 0)
				return SamplerType();

			return mParams[0].get(); // They should all have the same value
		}

	protected:
		Vector<TGpuParamSampState<Core>> mParams;
	};

	typedef TMaterialParamSampState<false> MaterialParamSampState;
	typedef TMaterialParamSampState<true> MaterialParamSampStateCore;
}