#include "CmBindableGpuParams.h"
#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmBindableGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmDebug.h"
#include "CmFrameAlloc.h"

namespace CamelotFramework
{
	BindableGpuParams::BindableGpuParams(const GpuParamsPtr& params, FrameAlloc* allocator)
		:mOwnsData(true), mParamDesc(params->getParamDesc()), mData(nullptr), mNumParamBlocks(0), mAllocator(allocator),
		mNumTextures(0), mNumSamplerStates(0),mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{
		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 paramBlockBufferSize = params->mNumParamBlocks * sizeof(BindableGpuParamBlock*);
		UINT32 paramBlockBuffersBufferSize = params->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = params->mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = params->mNumSamplerStates * sizeof(HSamplerState);

		UINT32 bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;
		for(UINT32 i = 0; i < params->mNumParamBlocks; i++)
		{
			if(params->mParamBlockBuffers[i] != nullptr)
				bufferSize += sizeof(BindableGpuParamBlock) + params->mParamBlockBuffers[i]->getSize();
		}

		mData = (UINT8*)allocator->alloc(bufferSize);
		mNumParamBlocks = params->mNumParamBlocks;
		mNumTextures = params->mNumTextures;
		mNumSamplerStates = params->mNumSamplerStates;

		UINT8* dataIter = mData;
		mParamBlocks = (BindableGpuParamBlock**)dataIter;
		dataIter += paramBlockBufferSize;

		mParamBlockBuffers = (GpuParamBlockBufferPtr*)dataIter;
		dataIter += paramBlockBuffersBufferSize;

		mTextures = (HTexture*)dataIter;
		dataIter += textureBufferSize;

		mSamplerStates = (HSamplerState*)dataIter;
		dataIter += samplerStateBufferSize;

		// Copy data
		memcpy(mParamBlockBuffers, params->mParamBlockBuffers, paramBlockBuffersBufferSize);
		memcpy(mTextures, params->mTextures, textureBufferSize);
		memcpy(mSamplerStates, params->mSamplerStates, samplerStateBufferSize);

		for(UINT32 i = 0; i < params->mNumParamBlocks; i++)
		{
			if(params->mParamBlockBuffers[i] != nullptr)
			{
				GpuParamBlock* paramBlock = params->mParamBlockBuffers[i]->getParamBlock();

				UINT32 bufferSize = paramBlock->getSize();
				mParamBlocks[i] = (BindableGpuParamBlock*)dataIter;

				dataIter += sizeof(BindableGpuParamBlock);
				mParamBlocks[i]->mData = dataIter;

				dataIter += bufferSize;
				memcpy(mParamBlocks[i]->mData, paramBlock->getData(), bufferSize);

				mParamBlocks[i]->mSize = bufferSize;
				mParamBlocks[i]->mDirty = paramBlock->isDirty();
			}
		}
	}

	BindableGpuParams::BindableGpuParams(const BindableGpuParams& source)
		:mParamDesc(source.mParamDesc)
	{
		mOwnsData = true;
		source.mOwnsData = false;

		mAllocator = source.mAllocator;
		mData = source.mData;
		mNumParamBlocks = source.mNumParamBlocks;
		mNumTextures = source.mNumTextures;
		mNumSamplerStates = source.mNumSamplerStates;

		mParamBlocks = source.mParamBlocks;
		mParamBlockBuffers = source.mParamBlockBuffers;
		mTextures = source.mTextures;
		mSamplerStates = source.mSamplerStates;
	}

	BindableGpuParams::~BindableGpuParams()
	{
		if(mOwnsData && mData != nullptr)
		{
			mAllocator->dealloc(mData);
		}
	}

	GpuParamBlockBufferPtr BindableGpuParams::getParamBlockBuffer(UINT32 slot) const
	{
		if(slot < 0 || slot >= mNumParamBlocks)
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		return mParamBlockBuffers[slot];
	}

	GpuParamBlockBufferPtr BindableGpuParams::getParamBlockBuffer(const String& name) const
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return nullptr;
		}

		return mParamBlockBuffers[iterFind->second.slot];
	}

	HTexture BindableGpuParams::getTexture(UINT32 slot)
	{
		if(slot < 0 || slot >= mNumTextures)
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextures[slot];
	}

	HSamplerState BindableGpuParams::getSamplerState(UINT32 slot)
	{
		if(slot < 0 || slot >= mNumSamplerStates)
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
		}

		return mSamplerStates[slot];
	}

	void BindableGpuParams::updateHardwareBuffers()
	{
		for(size_t i = 0; i < mNumParamBlocks; i++)
		{
			if(mParamBlocks[i] != nullptr && mParamBlockBuffers[i] != nullptr)
			{
				if(mParamBlocks[i]->isDirty())
					mParamBlocks[i]->uploadToBuffer(mParamBlockBuffers[i]);
			}
		}
	}
}