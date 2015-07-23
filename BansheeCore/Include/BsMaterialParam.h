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
		TMaterialDataParam(const SPtr<Vector<TGpuDataParam<T, Core>>>& params);
		TMaterialDataParam() { }
		
		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const T& value, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		T get(UINT32 arrayIdx = 0);

	protected:
		SPtr<Vector<TGpuDataParam<T, Core>>> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamStruct
	{
	public:
		TMaterialParamStruct(const SPtr<Vector<TGpuParamStruct<Core>>>& params);
		TMaterialParamStruct() { }

		/**
		 * @copydoc	TGpuParamStruct::set
		 */
		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	TGpuParamStruct::get
		 */
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	TGpuParamStruct::getElementSize
		 */
		UINT32 getElementSize() const;

	protected:
		SPtr<Vector<TGpuParamStruct<Core>>> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamTexture
	{
	public:
		typedef typename TGpuParamTextureType<Core>::Type TextureType;

		TMaterialParamTexture(const SPtr<Vector<TGpuParamTexture<Core>>>& params);
		TMaterialParamTexture() { }

		/**
		 * @copydoc	GpuParamTexture::set
		 */
		void set(const TextureType& texture);

		/**
		 * @copydoc	GpuParamTexture::get
		 */
		TextureType get();

	protected:
		SPtr<Vector<TGpuParamTexture<Core>>> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamLoadStoreTexture
	{
	public:
		typedef typename TGpuParamTextureType<Core>::Type TextureType;

		TMaterialParamLoadStoreTexture(const SPtr<Vector<TGpuParamLoadStoreTexture<Core>>>& params);
		TMaterialParamLoadStoreTexture() { }

		/**
		 * @copydoc	GpuParamLoadStoreTexture::set
		 */
		void set(const TextureType& texture, const TextureSurface& surface);

		/**
		 * @copydoc	GpuParamLoadStoreTexture::get
		 */
		TextureType get();

	protected:
		SPtr<Vector<TGpuParamLoadStoreTexture<Core>>> mParams;
	};

	/**
	 * @copydoc	TMaterialDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterialParamSampState
	{
	public:
		typedef typename TGpuParamSamplerStateType<Core>::Type SamplerType;

		TMaterialParamSampState(const SPtr<Vector<TGpuParamSampState<Core>>>& params);
		TMaterialParamSampState() { }

		/**
		 * @copydoc	GpuParamSampState::set
		 */
		void set(const SamplerType& sampState);

		/**
		 * @copydoc	GpuParamSampState::get
		 */
		SamplerType get();

	protected:
		SPtr<Vector<TGpuParamSampState<Core>>> mParams;
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

	typedef TMaterialParamStruct<false> MaterialParamStruct;
	typedef TMaterialParamStruct<true> MaterialParamStructCore;

	typedef TMaterialParamTexture<false> MaterialParamTexture;
	typedef TMaterialParamTexture<true> MaterialParamTextureCore;

	typedef TMaterialParamLoadStoreTexture<false> MaterialParamLoadStoreTexture;
	typedef TMaterialParamLoadStoreTexture<true> MaterialParamLoadStoreTextureCore;

	typedef TMaterialParamSampState<false> MaterialParamSampState;
	typedef TMaterialParamSampState<true> MaterialParamSampStateCore;
}