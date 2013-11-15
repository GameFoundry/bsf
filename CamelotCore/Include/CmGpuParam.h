#pragma once

#include "CmPrerequisites.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"

namespace CamelotFramework
{
	/**
	 * @brief	A handle that allows you to set a GpuProgram parameter.
	 * 			
	 * @note	This is primarily used an as optimization is performance critical bits of code
	 * 			where it is important to locate and set parameters quickly without any lookups.
	 * 			You just retrieve the handle once and then set the parameter value many times 
	 * 			with minimal performance impact.
	 */
	template<class T>
	class CM_EXPORT GpuDataParamBase
	{
	private:
		friend class GpuParams;

		struct GpuDataParamData
		{
			GpuDataParamData(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks, bool transpose)
				:paramDesc(paramDesc), paramBlocks(paramBlocks), transpose(transpose)
			{ }

			~GpuDataParamData()
			{ }

			GpuParamDataDesc* paramDesc;
			GpuParamBlock** paramBlocks;
			bool transpose;
		};

		template<class Type>
		struct TransposePolicy
		{
			static Type transpose(const Type& value) { return value; }
			static bool transposeEnabled(bool enabled) { return false; }
		};

		template<>
		struct TransposePolicy<Matrix3>
		{
			static Matrix3 transpose(const Matrix3& value) { return value.transpose(); }
			static bool transposeEnabled(bool enabled) { return enabled; }
		};

		template<>
		struct TransposePolicy<Matrix4>
		{
			static Matrix4 transpose(const Matrix4& value) { return value.transpose(); }
			static bool transposeEnabled(bool enabled) { return enabled; }
		};

	public:
		GpuDataParamBase()
		{ }

		void set(const T& value, UINT32 arrayIdx = 0)
		{
			GpuParamDataDesc* paramDesc = mData->paramDesc;

#if CM_DEBUG_MODE
			if(arrayIdx >= paramDesc->arraySize)
			{
				CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 sizeBytes = sizeof(T);
			GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

			if(TransposePolicy<T>::transposeEnabled(mData->transpose))
			{
				T transposed = TransposePolicy<T>::transpose(value);
				paramBlock->write((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &transposed, sizeBytes);
			}
			else
				paramBlock->write((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			// Set unused bytes to 0
			UINT32 elementSizeBytes = paramDesc->elementSize * sizeof(UINT32);
			if(sizeBytes < elementSizeBytes)
			{
				UINT32 diffSize = elementSizeBytes - sizeBytes;
				paramBlock->zeroOut((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
			}
		}

		T get(UINT32 arrayIdx = 0)
		{
			GpuParamDataDesc* paramDesc = mData->paramDesc;

#if CM_DEBUG_MODE
			if(arrayIdx >= paramDesc->arraySize)
			{
				CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
					toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
			}
#endif

			UINT32 sizeBytes = sizeof(T);
			GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

			T value;
			paramBlock->read((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			if(TransposePolicy<T>::transposeEnabled(mData->transpose))
				return TransposePolicy<T>::transpose(value);
			else
				return value;
		}

	private:
		GpuDataParamBase(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlock, bool transpose)
		{
			mData = cm_shared_ptr<GpuDataParamData>(paramDesc, paramBlock, transpose);
		}

	private:
		std::shared_ptr<GpuDataParamData> mData;
	};

	typedef GpuDataParamBase<float> GpuParamFloat;
	typedef GpuDataParamBase<Vector2> GpuParamVec2;
	typedef GpuDataParamBase<Vector3> GpuParamVec3;
	typedef GpuDataParamBase<Vector4> GpuParamVec4;
	typedef GpuDataParamBase<Matrix3> GpuParamMat3;
	typedef GpuDataParamBase<Matrix4> GpuParamMat4;
}