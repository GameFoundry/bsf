#include "BsGpuParamsProxy.h"
#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsBindableGpuParamBlock.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsDebug.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	/**
	* @brief	Specialized class for send GPU parameters to the core thread. Represents a single
	* 			parameter block buffer and is used for temporary storage of GPU parameters.
	*
	* @note		Due to the way allocation is handled, this class is not allowed to have a destructor.
	*
	* @see		BindableGpuParams
	*/
	class BS_CORE_EXPORT GpuParamsBlockProxy
	{
	public:
		GpuParamsBlockProxy()
			:mDirty(true), mData(nullptr), mSize(0)
		{ }

		/**
		* @brief	Uploads the current data to the specified buffer, and marks the block a non-dirty.
		*
		* @note	Core thread only.
		*/
		void uploadToBuffer(GpuParamBlockBufferPtr buffer)
		{
			buffer->writeData(mData);
			mDirty = false;
		}

		/**
		* @brief	Query if this object is dirty. If dirty the data of this block
		* 			will need to be uploaded to a GPU buffer.
		*
		* @note	Core thread only.
		*/
		bool isDirty() const { return mDirty; }
	protected:
		friend class GpuParams;
		friend class GpuParamsProxy;

		UINT8* mData;
		UINT32 mSize;
		bool mDirty;
	};

	GpuParamsProxy::GpuParamsProxy(const GpuParamsPtr& params, FrameAlloc* allocator)
		:mParamDesc(params->getParamDesc()), mData(nullptr), mNumParamBlocks(0), mAllocator(allocator),
		mNumTextures(0), mNumSamplerStates(0),mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{
		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 paramBlockBufferSize = params->mInternalData->mNumParamBlocks * sizeof(GpuParamsBlockProxy*);
		UINT32 paramBlockBuffersBufferSize = params->mInternalData->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = params->mInternalData->mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = params->mInternalData->mNumSamplerStates * sizeof(HSamplerState);

		UINT32 bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;
		for (UINT32 i = 0; i < params->mInternalData->mNumParamBlocks; i++)
		{
			if (params->mInternalData->mParamBlockBuffers[i] != nullptr)
				bufferSize += sizeof(GpuParamsBlockProxy)+params->mInternalData->mParamBlockBuffers[i]->getSize();
		}

		mData = (UINT8*)allocator->alloc(bufferSize);
		mNumParamBlocks = params->mInternalData->mNumParamBlocks;
		mNumTextures = params->mInternalData->mNumTextures;
		mNumSamplerStates = params->mInternalData->mNumSamplerStates;

		UINT8* dataIter = mData;
		mParamBlocks = (GpuParamsBlockProxy**)dataIter;
		dataIter += paramBlockBufferSize;

		mParamBlockBuffers = (GpuParamBlockBufferPtr*)dataIter;
		dataIter += paramBlockBuffersBufferSize;

		mTextures = (HTexture*)dataIter;
		dataIter += textureBufferSize;

		mSamplerStates = (HSamplerState*)dataIter;
		dataIter += samplerStateBufferSize;

		// Copy data
		memcpy(mParamBlockBuffers, params->mInternalData->mParamBlockBuffers, paramBlockBuffersBufferSize);
		memcpy(mTextures, params->mInternalData->mTextures, textureBufferSize);
		memcpy(mSamplerStates, params->mInternalData->mSamplerStates, samplerStateBufferSize);

		for (UINT32 i = 0; i < params->mInternalData->mNumParamBlocks; i++)
		{
			if (params->mInternalData->mParamBlockBuffers[i] != nullptr)
			{
				GpuParamBlock* paramBlock = params->mInternalData->mParamBlockBuffers[i]->getParamBlock();

				UINT32 bufferSize = paramBlock->getSize();
				mParamBlocks[i] = (GpuParamsBlockProxy*)dataIter;

				dataIter += sizeof(GpuParamsBlockProxy);
				mParamBlocks[i]->mData = dataIter;

				dataIter += bufferSize;
				memcpy(mParamBlocks[i]->mData, paramBlock->getData(), bufferSize);

				mParamBlocks[i]->mSize = bufferSize;
				mParamBlocks[i]->mDirty = paramBlock->isDirty();
			}
		}
	}

	GpuParamsProxy::~GpuParamsProxy()
	{
		if(mData != nullptr)
		{
			mAllocator->dealloc(mData);
		}
	}

	GpuParamBlockBufferPtr GpuParamsProxy::getParamBlockBuffer(UINT32 slot) const
	{
		if(slot < 0 || slot >= mNumParamBlocks)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		return mParamBlockBuffers[slot];
	}

	GpuParamBlockBufferPtr GpuParamsProxy::getParamBlockBuffer(const String& name) const
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return nullptr;
		}

		return mParamBlockBuffers[iterFind->second.slot];
	}

	HTexture GpuParamsProxy::getTexture(UINT32 slot)
	{
		if(slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextures[slot];
	}

	HSamplerState GpuParamsProxy::getSamplerState(UINT32 slot)
	{
		if(slot < 0 || slot >= mNumSamplerStates)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
		}

		return mSamplerStates[slot];
	}

	void GpuParamsProxy::updateHardwareBuffers()
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