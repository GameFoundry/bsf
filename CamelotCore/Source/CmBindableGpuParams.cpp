#include "CmBindableGpuParams.h"
#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmBindableGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	BindableGpuParams::BindableGpuParams(const GpuParamDesc& sourceParamDesc)
		:mOwnsData(true), mParamDesc(sourceParamDesc), mData(nullptr), mNumParamBlocks(0), 
		mNumTextures(0), mNumSamplerStates(0),mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{
		// Actual allocation of all data happens in parent GpuParams
	}

	BindableGpuParams::BindableGpuParams(const BindableGpuParams& source)
		:mParamDesc(source.mParamDesc)
	{
		mOwnsData = true;
		source.mOwnsData = false;

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
			cm_free(mData);
			// TODO - Dealloc using stack
		}
	}

	// TODO - Forbid copying but allow move semantics
	
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