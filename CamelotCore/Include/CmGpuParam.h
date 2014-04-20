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
			:mData(cm_shared_ptr<InternalData>())
		{ }

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

			UINT32 sizeBytes = sizeof(T);
			GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

			T value;
			paramBlock->read((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), &value, sizeBytes);

			if(TransposePolicy<T>::transposeEnabled(mData->transpose))
				return TransposePolicy<T>::transpose(value);
			else
				return value;
		}

		void destroy()
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

		void set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);
		void get(void* value, UINT32 sizeBytes, UINT32 arrayIdx = 0);
		UINT32 getElementSize() const;

		void destroy();
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

		void set(const HTexture& texture);
		HTexture get();

		void destroy();
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

		void set(const HSamplerState& texture);
		HSamplerState get();

		void destroy();
	private:
		GpuParamSampState(GpuParamObjectDesc* paramDesc, HSamplerState* samplerStates);

	private:
		std::shared_ptr<InternalData> mData;
	};
}