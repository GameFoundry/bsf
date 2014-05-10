#pragma once

#include "CmPrerequisites.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlock.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"

namespace BansheeEngine
{
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
	class CM_EXPORT GpuDataParamBase
	{
	private:
		friend class GpuParams;

		/**
		 * @brief	Internal data that is shared between GpuDataParam instances.
		 */
		struct InternalData
		{
			InternalData(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks, bool transpose)
				:paramDesc(paramDesc), paramBlocks(paramBlocks), transpose(transpose), isDestroyed(false)
			{ }

			InternalData()
				:paramDesc(nullptr), paramBlocks(nullptr), transpose(false), isDestroyed(true)
			{ }

			~InternalData()
			{ }

			GpuParamDataDesc* paramDesc;
			GpuParamBlock** paramBlocks;
			bool transpose;
			bool isDestroyed;
		};

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
		GpuDataParamBase()
			:mData(cm_shared_ptr<InternalData>())
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
			if(mData->isDestroyed)
				CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

			GpuParamDataDesc* paramDesc = mData->paramDesc;

#if CM_DEBUG_MODE
			if(arrayIdx >= paramDesc->arraySize)
			{
				CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = paramDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T)); // Truncate if it doesn't fit within parameter size
			GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

			if(TransposePolicy<T>::transposeEnabled(mData->transpose))
			{
				T transposed = TransposePolicy<T>::transpose(value);
				paramBlock->write((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &transposed, sizeBytes);
			}
			else
				paramBlock->write((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			// Set unused bytes to 0
			if(sizeBytes < elementSizeBytes)
			{
				UINT32 diffSize = elementSizeBytes - sizeBytes;
				paramBlock->zeroOut((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
			}
		}

		/**
		 * @brief	Returns a value of a parameter at the specified array index. If parameter does not
		 *			contain an array leave the index at 0.
		 *
		 * @note	No GPU reads are done. Data returned was cached when it was written. 
		 */
		T get(UINT32 arrayIdx = 0)
		{
			if(mData->isDestroyed)
				CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

			GpuParamDataDesc* paramDesc = mData->paramDesc;

#if CM_DEBUG_MODE
			if(arrayIdx >= paramDesc->arraySize)
			{
				CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 elementSizeBytes = paramDesc->elementSize * sizeof(UINT32);
			UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T));
			GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

			T value;
			paramBlock->read((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			if(TransposePolicy<T>::transposeEnabled(mData->transpose))
				return TransposePolicy<T>::transpose(value);
			else
				return value;
		}

		/**
		 * @brief	Called by the material when this handle is no longer valid (shader changed or material
		 *			got destroyed).
		 */
		void _destroy()
		{
			mData->isDestroyed = true;
		}

	private:
		GpuDataParamBase(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks, bool transpose)
			:mData(cm_shared_ptr<InternalData>(paramDesc, paramBlocks, transpose))
		{ }

	private:
		std::shared_ptr<InternalData> mData;
	};

	typedef GpuDataParamBase<float> GpuParamFloat;
	typedef GpuDataParamBase<Vector2> GpuParamVec2;
	typedef GpuDataParamBase<Vector3> GpuParamVec3;
	typedef GpuDataParamBase<Vector4> GpuParamVec4;
	typedef GpuDataParamBase<Matrix3> GpuParamMat3;
	typedef GpuDataParamBase<Matrix4> GpuParamMat4;

	/**
	 * @copydoc GpuDataParamBase
	 */
	class CM_EXPORT GpuParamStruct
	{
	private:
		friend class GpuParams;

		struct InternalData
		{
			InternalData(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks);
			InternalData();
			~InternalData();

			GpuParamDataDesc* paramDesc;
			GpuParamBlock** paramBlocks;
			bool isDestroyed;
		};

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

		/**
		 * @copydoc	GpuDataParamBase::_destroy
		 */
		void _destroy();
	private:
		GpuParamStruct(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks);

	private:
		std::shared_ptr<InternalData> mData;
	};

	/**
	 * @copydoc GpuDataParamBase
	 */
	class CM_EXPORT GpuParamTexture
	{
	private:
		friend class GpuParams;

		struct InternalData
		{
			InternalData(GpuParamObjectDesc* paramDesc, HTexture* textures);
			InternalData();
			~InternalData();

			GpuParamObjectDesc* paramDesc;
			HTexture* textures;
			bool isDestroyed;
		};

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
		
		/**
		* @copydoc	GpuDataParamBase::_destroy
		*/
		void _destroy();
	private:
		GpuParamTexture(GpuParamObjectDesc* paramDesc, HTexture* textures);

	private:
		std::shared_ptr<InternalData> mData;
	};

	/**
	 * @copydoc GpuDataParamBase
	 */
	class CM_EXPORT GpuParamSampState
	{
	private:
		friend class GpuParams;

		struct InternalData
		{
			InternalData(GpuParamObjectDesc* paramDesc, HSamplerState* samplerStates);
			InternalData();
			~InternalData();

			GpuParamObjectDesc* paramDesc;
			HSamplerState* samplerStates;
			bool isDestroyed;
		};

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

		/**
		* @copydoc	GpuDataParamBase::_destroy
		*/
		void _destroy();
	private:
		GpuParamSampState(GpuParamObjectDesc* paramDesc, HSamplerState* samplerStates);

	private:
		std::shared_ptr<InternalData> mData;
	};
}