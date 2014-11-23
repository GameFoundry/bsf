#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsDebug.h"
#include "BsException.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsMatrix3.h"
#include "BsMatrix4.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class containing some non-templated methods for
	 *			all types for GpuParams
	 */
	class BS_CORE_EXPORT GpuParamBase
	{
	protected:
		GpuParamBase();
		GpuParamBase(const SPtr<GpuParams>& parent);

		/**
		 * @brief	Marks the core data as dirty, signifying the core thread it should update it.
		 */
		void markCoreDirty();

	protected:
		SPtr<GpuParams> mParent;
	};

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			GpuDataParam.
	 *
	 * @see		TGpuDataParam
	 */
	class BS_CORE_EXPORT GpuDataParamBase : public GpuParamBase
	{
	protected:
		GpuDataParamBase();
		GpuDataParamBase(GpuParamDataDesc* paramDesc, const SPtr<GpuParams>& parent);

		/**
		 * @brief	Returns true if matrices need to be transposed when reading/writing them.
		 */
		bool getTransposeMatrices() const;

		/**
		 * @copydoc	GpuParamBlockBuffer::write
		 */
		bool write(UINT32 offset, const void* data, UINT32 size);

		/**
		 * @copydoc	GpuParamBlockBuffer::read
		 */
		bool read(UINT32 offset, void* data, UINT32 size);

		/**
		 * @copydoc	GpuParamBlockBuffer::zeroOut
		 */
		void zeroOut(UINT32 offset, UINT32 size);

	protected:
		GpuParamDataDesc* mParamDesc;
	};

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			GpuObjectParam.
	 *
	 * @see		TGpuObjectParam
	 */
	class BS_CORE_EXPORT GpuObjectParamBase : public GpuParamBase
	{
	protected:
		GpuObjectParamBase();
		GpuObjectParamBase(GpuParamObjectDesc* paramDesc, const SPtr<GpuParams>& parent);

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		HTexture getTexture(UINT32 slot);

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		HSamplerState getSamplerState(UINT32 slot);

		/**
		 * @brief	Sets a texture at the specified slot.
		 */
		void setTexture(UINT32 slot, const HTexture& texture);

		/**
		 * @brief	Sets a sampler state at the specified slot.
		 */
		void setSamplerState(UINT32 slot, const HSamplerState& sampler);

		/**
		 * @brief	Sets information that determines which texture surfaces to bind
		 *			as load/store parameters.
		 */
		void setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const;

		/**
		 * @brief	Changes the type of the texture at the specified slot.
		 */
		void setIsLoadStoreTexture(UINT32 slot, bool isLoadStore);

	protected:
		GpuParamObjectDesc* mParamDesc;
	};

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			all types for GpuParamCore params
	 */
	class BS_CORE_EXPORT GpuParamCoreBase
	{
	protected:
		GpuParamCoreBase();
		GpuParamCoreBase(const SPtr<GpuParamsCore>& parent);

		/**
		 * @brief	Marks the core data as dirty, signifying the core thread it should update it.
		 */
		void markCoreDirty();

	protected:
		SPtr<GpuParamsCore> mParent;
	};

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			GpuDataParam.
	 *
	 * @see		TGpuDataParam
	 */
	class BS_CORE_EXPORT GpuDataParamCoreBase : public GpuParamCoreBase
	{
	protected:
		GpuDataParamCoreBase();
		GpuDataParamCoreBase(GpuParamDataDesc* paramDesc, const SPtr<GpuParamsCore>& parent);

		/**
		 * @brief	Returns true if matrices need to be transposed when reading/writing them.
		 */
		bool getTransposeMatrices() const;

		/**
		 * @copydoc	GpuParamBlockBuffer::write
		 */
		bool write(UINT32 offset, const void* data, UINT32 size);

		/**
		 * @copydoc	GpuParamBlockBuffer::read
		 */
		bool read(UINT32 offset, void* data, UINT32 size);

		/**
		 * @copydoc	GpuParamBlockBuffer::zeroOut
		 */
		void zeroOut(UINT32 offset, UINT32 size);

	protected:
		GpuParamDataDesc* mParamDesc;
	};

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			GpuObjectParam.
	 *
	 * @see		TGpuObjectParam
	 */
	class BS_CORE_EXPORT GpuObjectParamCoreBase : public GpuParamCoreBase
	{
	protected:
		GpuObjectParamCoreBase();
		GpuObjectParamCoreBase(GpuParamObjectDesc* paramDesc, const SPtr<GpuParamsCore>& parent);

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		SPtr<TextureCore> getTexture(UINT32 slot);

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		SPtr<SamplerStateCore> getSamplerState(UINT32 slot);

		/**
		 * @brief	Sets a texture at the specified slot.
		 */
		void setTexture(UINT32 slot, const SPtr<TextureCore>& texture);

		/**
		 * @brief	Sets a sampler state at the specified slot.
		 */
		void setSamplerState(UINT32 slot, const SPtr<SamplerStateCore>& sampler);

		/**
		 * @brief	Sets information that determines which texture surfaces to bind
		 *			as load/store parameters.
		 */
		void setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const;

		/**
		 * @brief	Changes the type of the texture at the specified slot.
		 */
		void setIsLoadStoreTexture(UINT32 slot, bool isLoadStore);

	protected:
		GpuParamObjectDesc* mParamDesc;
	};

	template<bool Core> struct TGpuParamsPtrType { };
	template<> struct TGpuParamsPtrType<false> { typedef SPtr<GpuParams> Type; };
	template<> struct TGpuParamsPtrType<true> { typedef SPtr<GpuParamsCore> Type; };

	template<bool Core> struct TGpuDataParamBaseType { };
	template<> struct TGpuDataParamBaseType<false> { typedef GpuDataParamBase Type; };
	template<> struct TGpuDataParamBaseType<true> { typedef GpuDataParamCoreBase Type;	};

	template<bool Core> struct TGpuObjectParamBaseType { };
	template<> struct TGpuObjectParamBaseType<false> { typedef GpuObjectParamBase Type; };
	template<> struct TGpuObjectParamBaseType<true> { typedef GpuObjectParamCoreBase Type; };

	template<bool Core> struct TGpuParamTextureType { };
	template<> struct TGpuParamTextureType < false > { typedef HTexture Type; };
	template<> struct TGpuParamTextureType < true > { typedef SPtr<TextureCore> Type; };

	template<bool Core> struct TGpuParamSamplerStateType { };
	template<> struct TGpuParamSamplerStateType < false > { typedef HSamplerState Type; };
	template<> struct TGpuParamSamplerStateType < true > { typedef SPtr<SamplerStateCore> Type; };

	/**
	 * @brief	A handle that allows you to set a GpuProgram parameter. Internally keeps a reference to the 
	 *			GPU parameter buffer and the necessary offsets. You should specialize this type for specific 
	 *			parameter types. 
	 *
	 *			Object of this type must be returned by a Material. Setting/Getting parameter values will internally
	 *			access a GPU parameter buffer attached to the Material this parameter was created from. Anything
	 *			rendered with that material will then use those set values.
	 * 			
	 * @note	Normally you can set a GpuProgram parameter by calling various set/get methods on a Material.
	 *			This class primarily used an as optimization in performance critical bits of code
	 * 			where it is important to locate and set parameters quickly without any lookups
	 *			(Mentioned set/get methods expect a parameter name). You just retrieve the handle 
	 *			once and then set the parameter value many times with minimal performance impact.
	 * 
	 * @see		Material
	 */
	template<class T, bool Core>
	class BS_CORE_EXPORT TGpuDataParam : public TGpuDataParamBaseType<Core>::Type
	{
	private:
		/**
		 * @brief	Policy class that allows us to re-use this template class for matrices which might
		 *			need transposing, and other types which do not. Matrix needs to be transposed for
		 *			certain render systems depending on how they store them in memory.
		 */
		template<class Type>
		struct TransposePolicy
		{
			static Type transpose(const Type& value) { return value; }
			static bool transposeEnabled(bool enabled) { return false; }
		};

		/**
		 * @brief	Transpose policy for 3x3 matrix.
		 */
		template<>
		struct TransposePolicy<Matrix3>
		{
			static Matrix3 transpose(const Matrix3& value) { return value.transpose(); }
			static bool transposeEnabled(bool enabled) { return enabled; }
		};

		/**
		* @brief	Transpose policy for 4x4 matrix.
		*/
		template<>
		struct TransposePolicy<Matrix4>
		{
			static Matrix4 transpose(const Matrix4& value) { return value.transpose(); }
			static bool transposeEnabled(bool enabled) { return enabled; }
		};

	public:
		TGpuDataParam()
		{ }

		TGpuDataParam(GpuParamDataDesc* paramDesc, const typename TGpuParamsPtrType<Core>::Type& parent)
			:TGpuDataParamBaseType<Core>::Type(paramDesc, parent)
		{ }

		/**
		 * @brief	Sets a parameter value at the specified array index. If parameter does not
		 *			contain an array leave the index at 0.
		 *
		 * @note	Like with all GPU parameters, the actual GPU buffer will not be updated until rendering
		 *			with material this parameter was created from starts on the core thread.
		 */
		void set(const T& value, UINT32 arrayIdx = 0)
		{
			if (mParent == nullptr)
				return;

#if BS_DEBUG_MODE
			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T)); // Truncate if it doesn't fit within parameter size

			if (TransposePolicy<T>::transposeEnabled(getTransposeMatrices()))
			{
				T transposed = TransposePolicy<T>::transpose(value);
				write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &transposed, sizeBytes);
			}
			else
				write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			// Set unused bytes to 0
			if(sizeBytes < elementSizeBytes)
			{
				UINT32 diffSize = elementSizeBytes - sizeBytes;
				zeroOut((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride)  * sizeof(UINT32)+sizeBytes, diffSize);
			}

			markCoreDirty();
		}

		/**
		 * @brief	Returns a value of a parameter at the specified array index. If parameter does not
		 *			contain an array leave the index at 0.
		 *
		 * @note	No GPU reads are done. Data returned was cached when it was written. 
		 */
		T get(UINT32 arrayIdx = 0)
		{
			if (mParent == nullptr)
				return T();

#if BS_DEBUG_MODE
			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T));

			T value;
			read((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			if (TransposePolicy<T>::transposeEnabled(getTransposeMatrices()))
				return TransposePolicy<T>::transpose(value);
			else
				return value;
		}
	};

	typedef TGpuDataParam<float, false> GpuParamFloat;
	typedef TGpuDataParam<Color, false> GpuParamColor;
	typedef TGpuDataParam<Vector2, false> GpuParamVec2;
	typedef TGpuDataParam<Vector3, false> GpuParamVec3;
	typedef TGpuDataParam<Vector4, false> GpuParamVec4;
	typedef TGpuDataParam<Matrix3, false> GpuParamMat3;
	typedef TGpuDataParam<Matrix4, false> GpuParamMat4;

	template class TGpuDataParam<float, false>;
	template class TGpuDataParam<Color, false>;
	template class TGpuDataParam<Vector2, false>;
	template class TGpuDataParam<Vector3, false>;
	template class TGpuDataParam<Vector4, false>;
	template class TGpuDataParam<Matrix3, false>;
	template class TGpuDataParam<Matrix4, false>;

	typedef TGpuDataParam<float, true> GpuParamFloatCore;
	typedef TGpuDataParam<Color, true> GpuParamColorCore;
	typedef TGpuDataParam<Vector2, true> GpuParamVec2Core;
	typedef TGpuDataParam<Vector3, true> GpuParamVec3Core;
	typedef TGpuDataParam<Vector4, true> GpuParamVec4Core;
	typedef TGpuDataParam<Matrix3, true> GpuParamMat3Core;
	typedef TGpuDataParam<Matrix4, true> GpuParamMat4Core;

	template class TGpuDataParam<float, true>;
	template class TGpuDataParam<Color, true>;
	template class TGpuDataParam<Vector2, true>;
	template class TGpuDataParam<Vector3, true>;
	template class TGpuDataParam<Vector4, true>;
	template class TGpuDataParam<Matrix3, true>;
	template class TGpuDataParam<Matrix4, true>;

	/**
	 * @copydoc TGpuDataParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TGpuParamStruct : public TGpuDataParamBaseType<Core>::Type
	{
	public:
		TGpuParamStruct()
		{ }

		TGpuParamStruct(GpuParamDataDesc* paramDesc, const typename TGpuParamsPtrType<Core>::Type& parent)
			:TGpuDataParamBaseType<Core>::Type(paramDesc, parent)
		{ }

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0)
		{
			if (mParent == nullptr)
				return;

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);

#if BS_DEBUG_MODE
			if (sizeBytes > elementSizeBytes)
			{
				LOGWRN("Provided element size larger than maximum element size. Maximum size: " +
					toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
			}

			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " +
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			sizeBytes = std::min(elementSizeBytes, sizeBytes);

			write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);

			// Set unused bytes to 0
			if (sizeBytes < elementSizeBytes)
			{
				UINT32 diffSize = elementSizeBytes - sizeBytes;
				zeroOut((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
			}

			markCoreDirty();
		}

		/**
		 * @copydoc	TGpuDataParam::get
		 */
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0)
		{
			if (mParent == nullptr)
				return;

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);

#if BS_DEBUG_MODE
			if (sizeBytes > elementSizeBytes)
			{
				LOGWRN("Provided element size larger than maximum element size. Maximum size: " +
					toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
			}

			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " +
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif
			sizeBytes = std::min(elementSizeBytes, sizeBytes);

			read((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);
		}


		/**
		 * @brief	Returns the size of the struct in bytes.
		 */
		UINT32 getElementSize() const
		{
			if (mParent == nullptr)
				return 0;

			return mParamDesc->elementSize * sizeof(UINT32);
		}
	};

	typedef TGpuParamStruct<false> GpuParamStruct;
	typedef TGpuParamStruct<true> GpuParamStructCore;

	template class TGpuParamStruct<false>;
	template class TGpuParamStruct<true>;

	/**
	 * @copydoc TGpuObjectParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TGpuParamTexture : public TGpuObjectParamBaseType<Core>::Type
	{
	private:
		friend class GpuParams;
		friend class GpuParamsCore;

	public:
		TGpuParamTexture()
		{ }

		TGpuParamTexture(GpuParamObjectDesc* paramDesc, const typename TGpuParamsPtrType<Core>::Type& parent)
			:TGpuObjectParamBaseType<Core>::Type(paramDesc, parent)
		{ }

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const typename TGpuParamTextureType<Core>::Type& texture)
		{
			if (mParent == nullptr)
				return;

			setTexture(mParamDesc->slot, texture);
			setIsLoadStoreTexture(mParamDesc->slot, false);

			markCoreDirty();
		}

		/**
		 * @copydoc	TGpuDataParam::get
		 */
		typename TGpuParamTextureType<Core>::Type get()
		{
			if (mParent == nullptr)
				return TGpuParamTextureType<Core>::Type();

			return getTexture(mParamDesc->slot);
		}
	};

	typedef TGpuParamTexture<false> GpuParamTexture;
	typedef TGpuParamTexture<true> GpuParamTextureCore;

	template class TGpuParamTexture<false>;
	template class TGpuParamTexture<true>;

	/**
	 * @copydoc TGpuObjectParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TGpuParamLoadStoreTexture : public TGpuObjectParamBaseType<Core>::Type
	{
	private:
		friend class GpuParams;
		friend class GpuParamsCore;

	public:
		TGpuParamLoadStoreTexture()
		{ }

		TGpuParamLoadStoreTexture(GpuParamObjectDesc* paramDesc, const typename TGpuParamsPtrType<Core>::Type& parent)
			:TGpuObjectParamBaseType<Core>::Type(paramDesc, parent)
		{ }

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const typename TGpuParamTextureType<Core>::Type& texture, const TextureSurface& surface)
		{
			if (mParent == nullptr)
				return;

			setTexture(mParamDesc->slot, texture);
			setIsLoadStoreTexture(mParamDesc->slot, true);
			setLoadStoreSurface(mParamDesc->slot, surface);

			markCoreDirty();
		}

		/**
		 * @copydoc	TGpuDataParam::get
		 */
		typename TGpuParamTextureType<Core>::Type get()
		{
			if (mParent == nullptr)
				return TGpuParamTextureType<Core>::Type();

			return getTexture(mParamDesc->slot);
		}
	};

	typedef TGpuParamLoadStoreTexture<false> GpuParamLoadStoreTexture;
	typedef TGpuParamLoadStoreTexture<true> GpuParamLoadStoreTextureCore;

	template class TGpuParamLoadStoreTexture<false>;
	template class TGpuParamLoadStoreTexture<true>;

	/**
	 * @copydoc TGpuObjectParam
	 */
	template<bool Core>
	class BS_CORE_EXPORT TGpuParamSampState : public TGpuObjectParamBaseType<Core>::Type
	{
	private:
		friend class GpuParams;
		friend class GpuParamsCore;

	public:
		TGpuParamSampState()
		{ }

		TGpuParamSampState(GpuParamObjectDesc* paramDesc, const typename TGpuParamsPtrType<Core>::Type& parent)
			:TGpuObjectParamBaseType<Core>::Type(paramDesc, parent)
		{ }

		/**
		 * @copydoc	TGpuDataParam::set
		 */
		void set(const typename TGpuParamSamplerStateType<Core>::Type& samplerState)
		{
			if (mParent == nullptr)
				return;

			setSamplerState(mParamDesc->slot, samplerState);

			markCoreDirty();
		}

		/**
		 * @copydoc	TGpuDataParam::get
		 */
		typename TGpuParamSamplerStateType<Core>::Type get()
		{
			if (mParent == nullptr)
				return TGpuParamSamplerStateType<Core>::Type();

			return getSamplerState(mParamDesc->slot);
		}
	};

	typedef TGpuParamSampState<false> GpuParamSampState;
	typedef TGpuParamSampState<true> GpuParamSampStateCore;

	template class TGpuParamSampState<false>;
	template class TGpuParamSampState<true>;
}