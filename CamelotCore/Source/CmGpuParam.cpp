#include "CmGpuParam.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 									STRUCT	                     		*/
	/************************************************************************/

	GpuParamStruct::InternalData::InternalData(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks)
		:paramDesc(paramDesc), paramBlocks(paramBlocks), isDestroyed(false)
	{ }

	GpuParamStruct::InternalData::InternalData()
		:paramDesc(nullptr), paramBlocks(nullptr), isDestroyed(true)
	{ }

	GpuParamStruct::InternalData::~InternalData()
	{ }

	GpuParamStruct::GpuParamStruct()
		:mData(cm_shared_ptr<InternalData>())
	{ }

	GpuParamStruct::GpuParamStruct(GpuParamDataDesc* paramDesc, GpuParamBlock** paramBlocks)
		:mData(cm_shared_ptr<InternalData>(paramDesc, paramBlocks))
	{ }

	void GpuParamStruct::set(const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		GpuParamDataDesc* paramDesc = mData->paramDesc;
		UINT32 elementSizeBytes = paramDesc->elementSize * sizeof(UINT32);

#if CM_DEBUG_MODE
		if(sizeBytes > elementSizeBytes)
		{
			CM_EXCEPT(InvalidParametersException, "Provided element size larger than maximum element size. Maximum size: " + 
				toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
		}

		if(arrayIdx >= paramDesc->arraySize)
		{
			CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
				toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
		}
#endif

		GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];
		paramBlock->write((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);

		// Set unused bytes to 0
		if(sizeBytes < elementSizeBytes)
		{
			UINT32 diffSize = elementSizeBytes - sizeBytes;
			paramBlock->zeroOut((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
		}
	}

	void GpuParamStruct::get(void* value, UINT32 sizeBytes, UINT32 arrayIdx)
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		GpuParamDataDesc* paramDesc = mData->paramDesc;
		UINT32 elementSizeBytes = paramDesc->elementSize * sizeof(UINT32);

#if CM_DEBUG_MODE
		if(sizeBytes > elementSizeBytes)
		{
			CM_EXCEPT(InvalidParametersException, "Provided element size larger than maximum element size. Maximum size: " + 
				toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
		}

		if(arrayIdx >= paramDesc->arraySize)
		{
			CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
				toString(paramDesc->arraySize) + ". Requested size: " + toString(arrayIdx));
		}
#endif

		GpuParamBlock* paramBlock = mData->paramBlocks[paramDesc->paramBlockSlot];

		paramBlock->read((paramDesc->cpuMemOffset + arrayIdx * paramDesc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);
	}

	UINT32 GpuParamStruct::getElementSize() const
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		GpuParamDataDesc* paramDesc = mData->paramDesc;
		return paramDesc->elementSize * sizeof(UINT32);
	}

	void GpuParamStruct::_destroy()
	{
		mData->isDestroyed = true;
	}

	/************************************************************************/
	/* 								TEXTURE		                     		*/
	/************************************************************************/

	GpuParamTexture::InternalData::InternalData(GpuParamObjectDesc* paramDesc, HTexture* textures)
		:paramDesc(paramDesc), textures(textures), isDestroyed(false)
	{ }

	GpuParamTexture::InternalData::InternalData()
		:paramDesc(nullptr), textures(nullptr), isDestroyed(true)
	{ }

	GpuParamTexture::InternalData::~InternalData()
	{ }

	GpuParamTexture::GpuParamTexture()
		:mData(cm_shared_ptr<InternalData>())
	{ }

	GpuParamTexture::GpuParamTexture(GpuParamObjectDesc* paramDesc, HTexture* textures)
		:mData(cm_shared_ptr<InternalData>(paramDesc, textures))
	{ }

	void GpuParamTexture::set(const HTexture& texture)
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		mData->textures[mData->paramDesc->slot] = texture;
	}

	HTexture GpuParamTexture::get()
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		return mData->textures[mData->paramDesc->slot];
	}

	void GpuParamTexture::_destroy()
	{
		mData->isDestroyed = true;
	}

	/************************************************************************/
	/* 								SAMPLER STATE                      		*/
	/************************************************************************/

	GpuParamSampState::InternalData::InternalData(GpuParamObjectDesc* paramDesc, HSamplerState* samplerStates)
		:paramDesc(paramDesc), samplerStates(samplerStates), isDestroyed(false)
	{ }

	GpuParamSampState::InternalData::InternalData()
		:paramDesc(nullptr), samplerStates(nullptr), isDestroyed(true)
	{ }

	GpuParamSampState::InternalData::~InternalData()
	{ }

	GpuParamSampState::GpuParamSampState()
		:mData(cm_shared_ptr<InternalData>())
	{ }

	GpuParamSampState::GpuParamSampState(GpuParamObjectDesc* paramDesc, HSamplerState* samplerStates)
		:mData(cm_shared_ptr<InternalData>(paramDesc, samplerStates))
	{ }

	void GpuParamSampState::set(const HSamplerState& samplerState)
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		mData->samplerStates[mData->paramDesc->slot] = samplerState;
	}

	HSamplerState GpuParamSampState::get()
	{
		if(mData->isDestroyed)
			CM_EXCEPT(InternalErrorException, "Trying to access a destroyed gpu parameter.");

		return mData->samplerStates[mData->paramDesc->slot];
	}

	void GpuParamSampState::_destroy()
	{
		mData->isDestroyed = true;
	}
}