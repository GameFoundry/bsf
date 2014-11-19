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

		TMaterialDataParam(const Vector<TGpuDataParam<T, Core>>& params)
			:mParams(params)
		{ }
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