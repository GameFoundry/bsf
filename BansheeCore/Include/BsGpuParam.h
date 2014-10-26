#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlock.h"
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
	struct GpuParamsInternalData;

	/**
	 * @brief	Base class containing some non-templated methods for
	 *			GpuDataParam.
	 *
	 * @see		TGpuDataParam
	 */
	class BS_CORE_EXPORT GpuDataParamBase
	{
	protected:
		GpuDataParamBase();
		GpuDataParamBase(GpuParamDataDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData);

		/**
		 * @brief	Checks if the gpu param is still valid or were the parent GpuParams destroyed.
		 */
		bool isDestroyed() const;

		/**
		 * @brief	Gets a parameter block at the specified slot index.
		 */
		GpuParamBlockPtr getParamBlock(UINT32 slotIdx) const;

		/**
		 * @brief	Returns true if matrices need to be transposed when reading/writing them.
		 */
		bool getTransposeMatrices() const;

		/**
		 * @brief	Marks the core data as dirty, signifying the core thread it should update it.
		 */
		void markCoreDirty();

	protected:
		GpuParamDataDesc* mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;
	};

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
	 *
	 * @note	Sim thread only.
	 */
	template<class T>
	class BS_CORE_EXPORT TGpuDataParam : public GpuDataParamBase
	{
	private:
		friend class GpuParams;

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

		/**
		 * @brief	Sets a parameter value at the specified array index. If parameter does not
		 *			contain an array leave the index at 0.
		 *
		 * @note	Like with all GPU parameters, the actual GPU buffer will not be updated until rendering
		 *			with material this parameter was created from starts on the core thread.
		 */
		void set(const T& value, UINT32 arrayIdx = 0)
		{
			if (isDestroyed())
				BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

#if BS_DEBUG_MODE
			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T)); // Truncate if it doesn't fit within parameter size
			GpuParamBlockPtr paramBlock = getParamBlock(mParamDesc->paramBlockSlot);

			if (paramBlock == nullptr)
				return;

			if (TransposePolicy<T>::transposeEnabled(getTransposeMatrices()))
			{
				T transposed = TransposePolicy<T>::transpose(value);
				paramBlock->write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &transposed, sizeBytes);
			}
			else
				paramBlock->write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			// Set unused bytes to 0
			if(sizeBytes < elementSizeBytes)
			{
				UINT32 diffSize = elementSizeBytes - sizeBytes;
				paramBlock->zeroOut((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride)  * sizeof(UINT32)+sizeBytes, diffSize);
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
			if (isDestroyed())
				BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

#if BS_DEBUG_MODE
			if (arrayIdx >= mParamDesc->arraySize)
			{
				BS_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(mParamDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T));
			GpuParamBlockPtr paramBlock = getParamBlock(mParamDesc->paramBlockSlot);

			if (paramBlock == nullptr)
				return T();
			
			T value;
			paramBlock->read((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			if (TransposePolicy<T>::transposeEnabled(getTransposeMatrices()))
				return TransposePolicy<T>::transpose(value);
			else
				return value;
		}

	private:
		TGpuDataParam(GpuParamDataDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData)
			:GpuDataParamBase(paramDesc, internalData)
		{ }
	};

	typedef TGpuDataParam<float> GpuParamFloat;
	typedef TGpuDataParam<Color> GpuParamColor;
	typedef TGpuDataParam<Vector2> GpuParamVec2;
	typedef TGpuDataParam<Vector3> GpuParamVec3;
	typedef TGpuDataParam<Vector4> GpuParamVec4;
	typedef TGpuDataParam<Matrix3> GpuParamMat3;
	typedef TGpuDataParam<Matrix4> GpuParamMat4;

	template class TGpuDataParam<float>;
	template class TGpuDataParam<Color>;
	template class TGpuDataParam<Vector2>;
	template class TGpuDataParam<Vector3>;
	template class TGpuDataParam<Vector4>;
	template class TGpuDataParam<Matrix3>;
	template class TGpuDataParam<Matrix4>;

	/**
	 * @copydoc GpuDataParamBase
	 */
	class BS_CORE_EXPORT GpuParamStruct
	{
	private:
		friend class GpuParams;

	public:
		GpuParamStruct();

		/**
		 * @copydoc	GpuDataParamBase::set
		 */
		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @copydoc	GpuDataParamBase::get
		 */
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);

		/**
		 * @brief	Returns the size of the struct in bytes.
		 */
		UINT32 getElementSize() const;

	private:
		GpuParamStruct(GpuParamDataDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData);

	private:
		GpuParamDataDesc* mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;
	};

	/**
	 * @copydoc GpuDataParamBase
	 */
	class BS_CORE_EXPORT GpuParamTexture
	{
	private:
		friend class GpuParams;

	public:
		GpuParamTexture();

		/**
		 * @copydoc	GpuDataParamBase::set
		 */
		void set(const HTexture& texture);

		/**
		 * @copydoc	GpuDataParamBase::get
		 */
		HTexture get();
		
	private:
		GpuParamTexture(GpuParamObjectDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData);

	private:
		GpuParamObjectDesc* mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;
	};

	/**
	 * @copydoc GpuDataParamBase
	 */
	class BS_CORE_EXPORT GpuParamLoadStoreTexture
	{
	private:
		friend class GpuParams;

	public:
		GpuParamLoadStoreTexture();

		/**
		 * @copydoc	GpuDataParamBase::set
		 */
		void set(const HTexture& texture, const TextureSurface& surface);

		/**
		 * @copydoc	GpuDataParamBase::get
		 */
		HTexture get();
		
	private:
		GpuParamLoadStoreTexture(GpuParamObjectDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData);

	private:
		GpuParamObjectDesc* mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;
	};

	/**
	 * @copydoc GpuDataParamBase
	 */
	class BS_CORE_EXPORT GpuParamSampState
	{
	private:
		friend class GpuParams;

	public:
		GpuParamSampState();

		/**
		 * @copydoc	GpuDataParamBase::set
		 */
		void set(const HSamplerState& texture);

		/**
		 * @copydoc	GpuDataParamBase::get
		 */
		HSamplerState get();

	private:
		GpuParamSampState(GpuParamObjectDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData);

	private:
		GpuParamObjectDesc* mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;
	};
}