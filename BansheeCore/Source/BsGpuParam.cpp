#include "BsGpuParam.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	GpuDataParamBase::GpuDataParamBase()
		:mParamDesc(nullptr)
	{ }

	GpuDataParamBase::GpuDataParamBase(GpuParamDataDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData)
		:mParamDesc(paramDesc), mInternalData(internalData)
	{ }

	bool GpuDataParamBase::getIsDestroyed() const
	{
		return mInternalData->mIsDestroyed;
	}

	GpuParamBlock* GpuDataParamBase::getParamBlock(UINT32 slotIdx) const
	{
		return mInternalData->mParamBlocks[slotIdx];
	}

	bool GpuDataParamBase::getTransposeMatrices() const
	{
		return mInternalData->mTransposeMatrices;
	}

	void GpuDataParamBase::markCoreDirty() 
	{ 
		mInternalData->mCoreDirtyFlags = 0xFFFFFFFF; 
	}

	/************************************************************************/
	/* 									STRUCT	                     		*/
	/************************************************************************/

	GpuParamStruct::GpuParamStruct()
		:mParamDesc(nullptr)
	{ }

	GpuParamStruct::GpuParamStruct(GpuParamDataDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData)
		:mParamDesc(paramDesc), mInternalData(internalData)
	{ }

	void GpuParamStruct::set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);

#if BS_DEBUG_MODE
		if(sizeBytes > elementSizeBytes)
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

		GpuParamBlock* paramBlock = mInternalData->mParamBlocks[mParamDesc->paramBlockSlot];
		paramBlock->write((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);

		// Set unused bytes to 0
		if(sizeBytes < elementSizeBytes)
		{
			UINT32 diffSize = elementSizeBytes - sizeBytes;
			paramBlock->zeroOut((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride)  * sizeof(UINT32)+sizeBytes, diffSize);
		}

		mInternalData->mCoreDirtyFlags = 0xFFFFFFFF;
	}

	void GpuParamStruct::get(void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		UINT32 elementSizeBytes = mParamDesc->elementSize * sizeof(UINT32);

#if BS_DEBUG_MODE
		if(sizeBytes > elementSizeBytes)
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

		GpuParamBlock* paramBlock = mInternalData->mParamBlocks[mParamDesc->paramBlockSlot];
		paramBlock->read((mParamDesc->cpuMemOffset + arrayIdx * mParamDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);
	}

	UINT32 GpuParamStruct::getElementSize() const
	{
		if(mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		return mParamDesc->elementSize * sizeof(UINT32);
	}

	/************************************************************************/
	/* 								TEXTURE		                     		*/
	/************************************************************************/

	GpuParamTexture::GpuParamTexture()
		:mParamDesc(nullptr)
	{ }

	GpuParamTexture::GpuParamTexture(GpuParamObjectDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData)
		: mParamDesc(paramDesc), mInternalData(internalData)
	{ }

	void GpuParamTexture::set(const HTexture& texture)
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		mInternalData->mTextures[mParamDesc->slot] = texture;
		mInternalData->mCoreDirtyFlags = 0xFFFFFFFF;
	}

	HTexture GpuParamTexture::get()
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		return mInternalData->mTextures[mParamDesc->slot];
	}

	/************************************************************************/
	/* 								SAMPLER STATE                      		*/
	/************************************************************************/

	GpuParamSampState::GpuParamSampState()
		:mParamDesc(nullptr)
	{ }

	GpuParamSampState::GpuParamSampState(GpuParamObjectDesc* paramDesc, const std::shared_ptr<GpuParamsInternalData>& internalData)
		: mParamDesc(nullptr), mInternalData(internalData)
	{ }

	void GpuParamSampState::set(const HSamplerState& samplerState)
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		mInternalData->mSamplerStates[mParamDesc->slot] = samplerState;
		mInternalData->mCoreDirtyFlags = 0xFFFFFFFF;
	}

	HSamplerState GpuParamSampState::get()
	{
		if (mInternalData->mIsDestroyed)
			BS_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		return mInternalData->mSamplerStates[mParamDesc->slot];
	}
}