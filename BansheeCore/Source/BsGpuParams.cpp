#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlock.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsVector2.h"
#include "BsFrameAlloc.h"
#include "BsDebug.h"
#include "BsException.h"

namespace BansheeEngine
{
	GpuParamsInternalData::GpuParamsInternalData()
		:mTransposeMatrices(false), mData(nullptr), mNumParamBlocks(0), mNumTextures(0), mNumSamplerStates(0), mFrameAlloc(nullptr),
		mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr), mCoreDirtyFlags(0xFFFFFFFF),
		mIsDestroyed(false), mTextureInfo(nullptr)
	{ }

	GpuParams::GpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		:mParamDesc(paramDesc)
	{
		mInternalData = bs_shared_ptr<GpuParamsInternalData>();
		mInternalData->mTransposeMatrices = transposeMatrices;

		for(auto& paramBlock : mParamDesc->paramBlocks)
		{
			if ((paramBlock.second.slot + 1) > mInternalData->mNumParamBlocks)
				mInternalData->mNumParamBlocks = paramBlock.second.slot + 1;
		}

		for(auto& texture : mParamDesc->textures)
		{
			if ((texture.second.slot + 1) > mInternalData->mNumTextures)
				mInternalData->mNumTextures = texture.second.slot + 1;
		}

		for(auto& sampler : mParamDesc->samplers)
		{
			if ((sampler.second.slot + 1) > mInternalData->mNumSamplerStates)
				mInternalData->mNumSamplerStates = sampler.second.slot + 1;
		}

		constructInternalBuffers();
	}

	GpuParams::GpuParams(const GpuParamDescPtr& paramDesc, PrivatelyConstruct& dummy)
		:mParamDesc(paramDesc)
	{
		
	}

	GpuParams::~GpuParams()
	{
		mInternalData->mIsDestroyed = true;

		// Ensure everything is destructed
		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			mInternalData->mParamBlocks[i].~shared_ptr();
			mInternalData->mParamBlockBuffers[i].~shared_ptr();
		}

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
			mInternalData->mTextures[i].~ResourceHandle();

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
			mInternalData->mTextureInfo[i].~BoundTextureInfo();

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
			mInternalData->mSamplerStates[i].~ResourceHandle();

		if (mInternalData->mFrameAlloc != nullptr)
			mInternalData->mFrameAlloc->dealloc(mInternalData->mData);
		else
			bs_free(mInternalData->mData);
	}

	void GpuParams::setParamBlockBuffer(UINT32 slot, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		if (slot < 0 || slot >= mInternalData->mNumParamBlocks)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mInternalData->mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		mInternalData->mParamBlockBuffers[slot] = paramBlockBuffer;
		mInternalData->mParamBlocks[slot] = paramBlockBuffer->getParamBlock();

		markCoreDirty();
	}

	void GpuParams::setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		auto iterFind = mParamDesc->paramBlocks.find(name);

		if(iterFind == mParamDesc->paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		mInternalData->mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;
		mInternalData->mParamBlocks[iterFind->second.slot] = paramBlockBuffer != nullptr ? paramBlockBuffer->getParamBlock() : nullptr;

		markCoreDirty();
	}

	UINT32 GpuParams::getDataParamSize(const String& name) const
	{
		GpuParamDataDesc* desc = getParamDesc(name);
		if(desc != nullptr)
			return desc->elementSize * 4;

		return 0;
	}

	bool GpuParams::hasParam(const String& name) const
	{
		return getParamDesc(name) != nullptr;
	}

	bool GpuParams::hasTexture(const String& name) const
	{
		auto paramIter = mParamDesc->textures.find(name);
		if(paramIter != mParamDesc->textures.end())
			return true;

		return false;
	}

	bool GpuParams::hasSamplerState(const String& name) const
	{
		auto paramIter = mParamDesc->samplers.find(name);
		if(paramIter != mParamDesc->samplers.end())
			return true;

		return false;
	}

	bool GpuParams::hasParamBlock(const String& name) const
	{
		auto paramBlockIter = mParamDesc->paramBlocks.find(name);
		if(paramBlockIter != mParamDesc->paramBlocks.end())
			return true;

		return false;
	}

	void GpuParams::getStructParam(const String& name, GpuParamStruct& output) const
	{
		auto iterFind = mParamDesc->params.find(name);

		if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_STRUCT)
			BS_EXCEPT(InvalidParametersException, "Cannot find struct parameter with the name '" + name + "'");

		output = GpuParamStruct(&iterFind->second, mInternalData);
	}

	void GpuParams::getTextureParam(const String& name, GpuParamTexture& output) const
	{
		auto iterFind = mParamDesc->textures.find(name);

		if (iterFind == mParamDesc->textures.end())
			BS_EXCEPT(InvalidParametersException, "Cannot find texture parameter with the name '" + name + "'");

		output = GpuParamTexture(&iterFind->second, mInternalData);
	}

	void GpuParams::getLoadStoreTextureParam(const String& name, GpuParamLoadStoreTexture& output) const
	{
		auto iterFind = mParamDesc->textures.find(name);

		if (iterFind == mParamDesc->textures.end())
			BS_EXCEPT(InvalidParametersException, "Cannot find texture parameter with the name '" + name + "'");

		output = GpuParamLoadStoreTexture(&iterFind->second, mInternalData);
	}

	void GpuParams::getSamplerStateParam(const String& name, GpuParamSampState& output) const
	{
		auto iterFind = mParamDesc->samplers.find(name);

		if (iterFind == mParamDesc->samplers.end())
			BS_EXCEPT(InvalidParametersException, "Cannot find sampler state parameter with the name '" + name + "'");

		output = GpuParamSampState(&iterFind->second, mInternalData);
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc->params.find(name);
		if(paramIter != mParamDesc->params.end())
			return &paramIter->second;

		return nullptr;
	}

	GpuParamBlockBufferPtr GpuParams::getParamBlockBuffer(UINT32 slot) const
	{
		if (slot < 0 || slot >= mInternalData->mNumParamBlocks)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mInternalData->mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		return mInternalData->mParamBlockBuffers[slot];
	}

	HTexture GpuParams::getTexture(UINT32 slot)
	{
		if (slot < 0 || slot >= mInternalData->mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mInternalData->mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mInternalData->mTextures[slot];
	}

	HSamplerState GpuParams::getSamplerState(UINT32 slot)
	{
		if (slot < 0 || slot >= mInternalData->mNumSamplerStates)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mInternalData->mNumSamplerStates - 1) + ". Requested: " + toString(slot));
		}

		return mInternalData->mSamplerStates[slot];
	}

	bool GpuParams::isLoadStoreTexture(UINT32 slot) const
	{
		if (slot < 0 || slot >= mInternalData->mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mInternalData->mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mInternalData->mTextureInfo[slot].isLoadStore;
	}

	const TextureSurface& GpuParams::getLoadStoreSurface(UINT32 slot) const
	{
		if (slot < 0 || slot >= mInternalData->mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mInternalData->mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mInternalData->mTextureInfo[slot].surface;
	}

	void GpuParams::updateHardwareBuffers()
	{
		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			if (mInternalData->mParamBlocks[i] != nullptr && mInternalData->mParamBlockBuffers[i] != nullptr)
			{
				if (mInternalData->mParamBlocks[i]->isDirty())
					mInternalData->mParamBlocks[i]->uploadToBuffer(mInternalData->mParamBlockBuffers[i]);
			}
		}
	}

	void GpuParams::_updateFromCopy(const GpuParamsPtr& copy)
	{
		assert(copy->mInternalData->mNumParamBlocks == mInternalData->mNumParamBlocks);
		assert(copy->mInternalData->mNumTextures == mInternalData->mNumTextures);
		assert(copy->mInternalData->mNumSamplerStates == mInternalData->mNumSamplerStates);

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
		{
			mInternalData->mTextures[i] = copy->mInternalData->mTextures[i];
			mInternalData->mTextureInfo[i] = copy->mInternalData->mTextureInfo[i];
		}

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
		{
			mInternalData->mSamplerStates[i] = copy->mInternalData->mSamplerStates[i];
		}

		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			GpuParamBlockPtr destParamBlock = mInternalData->mParamBlocks[i];
			GpuParamBlockPtr srcParamBlock = copy->mInternalData->mParamBlocks[i];
			if (destParamBlock != nullptr && srcParamBlock != nullptr)
			{
				destParamBlock->write(0, srcParamBlock->getData(), srcParamBlock->getSize());
			}
		}
	}

	GpuParamsPtr GpuParams::_clone(FrameAlloc* frameAlloc, bool onlyDirtyBlocks) const
	{
		GpuParamsPtr myClone = nullptr;
		
		if (frameAlloc != nullptr)
		{
			StdFrameAlloc<std::shared_ptr<GpuParams>> myAlloc(frameAlloc);
			myClone = std::allocate_shared<GpuParams>(myAlloc, mParamDesc, PrivatelyConstruct());
			myClone->mInternalData = std::allocate_shared<GpuParamsInternalData>(myAlloc);
		}
		else
		{
			myClone = bs_shared_ptr<GpuParams>(mParamDesc, PrivatelyConstruct());;
			myClone->mInternalData = bs_shared_ptr<GpuParamsInternalData>();
		}
		
		myClone->mInternalData->mIsDestroyed = mInternalData->mIsDestroyed;
		myClone->mInternalData->mTransposeMatrices = mInternalData->mTransposeMatrices;
		myClone->mInternalData->mNumParamBlocks = mInternalData->mNumParamBlocks;
		myClone->mInternalData->mNumTextures = mInternalData->mNumTextures;
		myClone->mInternalData->mNumSamplerStates = mInternalData->mNumSamplerStates;

		myClone->constructInternalBuffers(frameAlloc);

		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			GpuParamBlockBufferPtr buffer = mInternalData->mParamBlockBuffers[i];
			if (buffer != nullptr && (!onlyDirtyBlocks || buffer->getParamBlock()->isDirty()))
			{
				GpuParamBlockPtr newBlock = nullptr;
				if (frameAlloc != nullptr)
				{
					StdFrameAlloc<GpuParams> myAlloc(frameAlloc);
					newBlock = std::allocate_shared<GpuParamBlock>(myAlloc, frameAlloc, buffer->getSize());
				}
				else
				{
					newBlock = bs_shared_ptr<GpuParamBlock>(buffer->getSize());;
				}

				newBlock->write(0, buffer->getParamBlock()->getData(), buffer->getSize());
				myClone->mInternalData->mParamBlocks[i] = newBlock;
			}
			else
				myClone->mInternalData->mParamBlocks[i] = nullptr;

			myClone->mInternalData->mParamBlockBuffers[i] = buffer;
		}

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
		{
			myClone->mInternalData->mTextures[i] = mInternalData->mTextures[i];
			myClone->mInternalData->mTextureInfo[i] = mInternalData->mTextureInfo[i];
		}

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
		{
			myClone->mInternalData->mSamplerStates[i] = mInternalData->mSamplerStates[i];
		}

		return myClone;
	}

	void GpuParams::constructInternalBuffers(FrameAlloc* frameAlloc)
	{
		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 bufferSize = 0;
		UINT32 paramBlockOffset = 0;
		UINT32 paramBlockBufferOffset = 0;
		UINT32 textureOffset = 0;
		UINT32 samplerStateOffset = 0;
		UINT32 textureInfoOffset = 0;

		UINT32 paramBlockBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlockPtr);
		UINT32 paramBlockBuffersBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = mInternalData->mNumTextures * sizeof(HTexture);
		UINT32 textureInfoBufferSize = mInternalData->mNumTextures * sizeof(BoundTextureInfo);
		UINT32 samplerStateBufferSize = mInternalData->mNumSamplerStates * sizeof(HSamplerState);

		bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize + textureInfoBufferSize;
		paramBlockOffset = 0;
		paramBlockBufferOffset = paramBlockOffset + paramBlockBufferSize;
		textureOffset = paramBlockBufferOffset + paramBlockBuffersBufferSize;
		samplerStateOffset = textureOffset + textureBufferSize;
		textureInfoOffset = samplerStateOffset + samplerStateBufferSize;

		if (frameAlloc != nullptr)
		{
			mInternalData->mData = frameAlloc->alloc(bufferSize);
			mInternalData->mFrameAlloc = frameAlloc;
		}
		else
		{
			mInternalData->mData = (UINT8*)bs_alloc(bufferSize);
			mInternalData->mFrameAlloc = nullptr;
		}

		mInternalData->mParamBlocks = (GpuParamBlockPtr*)(mInternalData->mData + paramBlockOffset);
		mInternalData->mParamBlockBuffers = (GpuParamBlockBufferPtr*)(mInternalData->mData + paramBlockBufferOffset);
		mInternalData->mTextures = (HTexture*)(mInternalData->mData + textureOffset);
		mInternalData->mSamplerStates = (HSamplerState*)(mInternalData->mData + samplerStateOffset);
		mInternalData->mTextureInfo = (BoundTextureInfo*)(mInternalData->mData + textureInfoOffset);

		// Ensure everything is constructed
		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			{
				GpuParamBlockPtr* ptrToIdx = (&mInternalData->mParamBlocks[i]);
				ptrToIdx = new (&mInternalData->mParamBlocks[i]) GpuParamBlockPtr(nullptr);
			}

			{
			GpuParamBlockBufferPtr* ptrToIdx = (&mInternalData->mParamBlockBuffers[i]);
			ptrToIdx = new (&mInternalData->mParamBlockBuffers[i]) GpuParamBlockBufferPtr(nullptr);
		}
		}

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
		{
			{
				HTexture* ptrToIdx = (&mInternalData->mTextures[i]);
				ptrToIdx = new (&mInternalData->mTextures[i]) HTexture();
			}

			{
				BoundTextureInfo* ptrToIdx = (&mInternalData->mTextureInfo[i]);
				ptrToIdx = new (&mInternalData->mTextureInfo[i]) BoundTextureInfo();
			}
		}

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
		{
			HSamplerState* ptrToIdx = (&mInternalData->mSamplerStates[i]);
			ptrToIdx = new (&mInternalData->mSamplerStates[i]) HSamplerState();
		}
	}

	bool GpuParams::_isCoreDirty() const 
	{ 
		return mInternalData->mCoreDirtyFlags != 0; 
	}

	void GpuParams::_markCoreClean()
	{
		mInternalData->mCoreDirtyFlags = 0;

		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			if (mInternalData->mParamBlocks[i] != nullptr)
				mInternalData->mParamBlocks[i]->setDirty(false);
		}
	}

	void GpuParams::markCoreDirty() 
	{ 
		mInternalData->mCoreDirtyFlags = 0xFFFFFFFF; 
	}
}