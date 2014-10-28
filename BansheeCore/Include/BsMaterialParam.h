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
	template<class T>
	class BS_CORE_EXPORT TMaterialDataParam
	{
	public:
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

	private:
		friend class Material;

		TMaterialDataParam(const Vector<TGpuDataParam<T>>& params)
			:mParams(params)
		{ }
	protected:
		Vector<TGpuDataParam<T>> mParams;
	};

	typedef TMaterialDataParam<float> MaterialParamFloat;
	typedef TMaterialDataParam<Color> MaterialParamColor;
	typedef TMaterialDataParam<Vector2> MaterialParamVec2;
	typedef TMaterialDataParam<Vector3> MaterialParamVec3;
	typedef TMaterialDataParam<Vector4> MaterialParamVec4;
	typedef TMaterialDataParam<Matrix3> MaterialParamMat3;
	typedef TMaterialDataParam<Matrix4> MaterialParamMat4;

	/**
	 * @copydoc	TMaterialDataParam
	 */
	class BS_CORE_EXPORT MaterialParamStruct
	{
	public:
		MaterialParamStruct() { }

		/**
		 * @copydoc	GpuParamStruct::set
		 */
		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	GpuParamStruct::get
		 */
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	GpuParamStruct::getElementSize
		 */
		UINT32 getElementSize() const;

	private:
		friend class Material;

		MaterialParamStruct(const Vector<GpuParamStruct>& params);
	protected:
		Vector<GpuParamStruct> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	class BS_CORE_EXPORT MaterialParamTexture
	{
	public:
		MaterialParamTexture() { }

		/**
		 * @copydoc	GpuParamTexture::set
		 */
		void set(const HTexture& texture);

		/**
		 * @copydoc	GpuParamTexture::get
		 */
		HTexture get();

	private:
		friend class Material;

		MaterialParamTexture(const Vector<GpuParamTexture>& params);

	protected:
		Vector<GpuParamTexture> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	class BS_CORE_EXPORT MaterialParamLoadStoreTexture
	{
	public:
		MaterialParamLoadStoreTexture() { }

		/**
		 * @copydoc	GpuParamLoadStoreTexture::set
		 */
		void set(const HTexture& texture, const TextureSurface& surface);

		/**
		 * @copydoc	GpuParamLoadStoreTexture::get
		 */
		HTexture get();

	private:
		friend class Material;

		MaterialParamLoadStoreTexture(const Vector<GpuParamLoadStoreTexture>& params);
	protected:
		Vector<GpuParamLoadStoreTexture> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	class BS_CORE_EXPORT MaterialParamSampState
	{
	public:
		MaterialParamSampState() { }

		/**
		 * @copydoc	GpuParamSampState::set
		 */
		void set(const HSamplerState& sampState);

		/**
		 * @copydoc	GpuParamSampState::get
		 */
		HSamplerState get();

	private:
		friend class Material;

		MaterialParamSampState(const Vector<GpuParamSampState>& params);
	protected:
		Vector<GpuParamSampState> mParams;
	};
}