#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlock.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsBindableGpuParamBlock.h"
#include "BsVector2.h"
#include "BsDebug.h"
#include "BsException.h"

namespace BansheeEngine
{
	GpuParamsInternalData::GpuParamsInternalData()
		:mTransposeMatrices(false), mData(nullptr), mNumParamBlocks(0), mNumTextures(0), mNumSamplerStates(0),
		mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{ }

	GpuParams::GpuParams(GpuParamDesc& paramDesc, bool transposeMatrices)
		:mParamDesc(paramDesc)
	{
		mInternalData = bs_shared_ptr<GpuParamsInternalData>();
		mInternalData->mTransposeMatrices = transposeMatrices;

		for(auto& paramBlock : mParamDesc.paramBlocks)
		{
			if ((paramBlock.second.slot + 1) > mInternalData->mNumParamBlocks)
				mInternalData->mNumParamBlocks = paramBlock.second.slot + 1;
		}

		for(auto& texture : mParamDesc.textures)
		{
			if ((texture.second.slot + 1) > mInternalData->mNumTextures)
				mInternalData->mNumTextures = texture.second.slot + 1;
		}

		for(auto& sampler : mParamDesc.samplers)
		{
			if ((sampler.second.slot + 1) > mInternalData->mNumSamplerStates)
				mInternalData->mNumSamplerStates = sampler.second.slot + 1;
		}

		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 bufferSize = 0;
		UINT32 paramBlockOffset = 0;
		UINT32 paramBlockBufferOffset = 0;
		UINT32 textureOffset = 0;
		UINT32 samplerStateOffset = 0;

		getInternalBufferData(bufferSize, paramBlockOffset, paramBlockBufferOffset, textureOffset, samplerStateOffset);

		mInternalData->mData = (UINT8*)bs_alloc(bufferSize);
		mInternalData->mParamBlocks = (GpuParamBlock**)(mInternalData->mData + paramBlockOffset);
		mInternalData->mParamBlockBuffers = (GpuParamBlockBufferPtr*)(mInternalData->mData + paramBlockBufferOffset);
		mInternalData->mTextures = (HTexture*)(mInternalData->mData + textureOffset);
		mInternalData->mSamplerStates = (HSamplerState*)(mInternalData->mData + samplerStateOffset);

		// Ensure everything is constructed
		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			mInternalData->mParamBlocks[i] = nullptr;

			GpuParamBlockBufferPtr* ptrToIdx = (&mInternalData->mParamBlockBuffers[i]);
			ptrToIdx = new (&mInternalData->mParamBlockBuffers[i]) GpuParamBlockBufferPtr(nullptr);
		}

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
		{
			HTexture* ptrToIdx = (&mInternalData->mTextures[i]);
			ptrToIdx = new (&mInternalData->mTextures[i]) HTexture();
		}

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
		{
			HSamplerState* ptrToIdx = (&mInternalData->mSamplerStates[i]);
			ptrToIdx = new (&mInternalData->mSamplerStates[i]) HSamplerState();
		}
	}

	GpuParams::GpuParams(GpuParamDesc& paramDesc, PrivatelyConstruct& dummy)
		:mParamDesc(paramDesc)
	{
		mInternalData = bs_shared_ptr<GpuParamsInternalData>();
	}

	GpuParams::~GpuParams()
	{
		mInternalData->mIsDestroyed = true;

		// Ensure everything is destructed
		for (UINT32 i = 0; i < mInternalData->mNumParamBlocks; i++)
		{
			mInternalData->mParamBlockBuffers[i].~shared_ptr();
		}

		for (UINT32 i = 0; i < mInternalData->mNumTextures; i++)
			mInternalData->mTextures[i].~ResourceHandle();

		for (UINT32 i = 0; i < mInternalData->mNumSamplerStates; i++)
			mInternalData->mSamplerStates[i].~ResourceHandle();

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
	}

	void GpuParams::setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		mInternalData->mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;
		mInternalData->mParamBlocks[iterFind->second.slot] = paramBlockBuffer->getParamBlock();
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
		auto paramIter = mParamDesc.textures.find(name);
		if(paramIter != mParamDesc.textures.end())
			return true;

		return false;
	}

	bool GpuParams::hasSamplerState(const String& name) const
	{
		auto paramIter = mParamDesc.samplers.find(name);
		if(paramIter != mParamDesc.samplers.end())
			return true;

		return false;
	}

	bool GpuParams::hasParamBlock(const String& name) const
	{
		auto paramBlockIter = mParamDesc.paramBlocks.find(name);
		if(paramBlockIter != mParamDesc.paramBlocks.end())
			return true;

		return false;
	}

	void GpuParams::getStructParam(const String& name, GpuParamStruct& output) const
	{
		auto iterFind = mParamDesc.params.find(name);

		if (iterFind == mParamDesc.params.end() || iterFind->second.type != GPDT_STRUCT)
			BS_EXCEPT(InvalidParametersException, "Cannot find struct parameter with the name '" + name + "'");

		output = GpuParamStruct(&iterFind->second, mInternalData);
	}

	void GpuParams::getTextureParam(const String& name, GpuParamTexture& output) const
	{
		auto iterFind = mParamDesc.textures.find(name);

		if (iterFind == mParamDesc.textures.end())
			BS_EXCEPT(InvalidParametersException, "Cannot find texture parameter with the name '" + name + "'");

		output = GpuParamTexture(&iterFind->second, mInternalData);
	}

	void GpuParams::getSamplerStateParam(const String& name, GpuParamSampState& output) const
	{
		auto iterFind = mParamDesc.samplers.find(name);

		if (iterFind == mParamDesc.samplers.end())
			BS_EXCEPT(InvalidParametersException, "Cannot find sampler state parameter with the name '" + name + "'");

		output = GpuParamSampState(&iterFind->second, mInternalData);
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc.params.find(name);
		if(paramIter != mParamDesc.params.end())
			return &paramIter->second;

		return nullptr;
	}

	GpuParamsPtr GpuParams::clone() const
	{
		GpuParamsPtr myClone = bs_shared_ptr<GpuParams>(mParamDesc, PrivatelyConstruct());
		myClone->mInternalData->mIsDestroyed = mInternalData->mIsDestroyed;
		myClone->mInternalData->mTransposeMatrices = mInternalData->mTransposeMatrices;
		myClone->mInternalData->mNumParamBlocks = mInternalData->mNumParamBlocks;
		myClone->mInternalData->mNumTextures = mInternalData->mNumTextures;
		myClone->mInternalData->mNumSamplerStates = mInternalData->mNumSamplerStates;

		UINT32 bufferSize = 0;
		UINT32 paramBlockOffset = 0;
		UINT32 paramBlockBufferOffset = 0;
		UINT32 textureOffset = 0;
		UINT32 samplerStateOffset = 0;

		getInternalBufferData(bufferSize, paramBlockOffset, paramBlockBufferOffset, textureOffset, samplerStateOffset);

		myClone->mInternalData->mData = (UINT8*)bs_alloc(bufferSize);
		memcpy(myClone->mInternalData->mData, mInternalData->mData, bufferSize);

		myClone->mInternalData->mParamBlocks = (GpuParamBlock**)(myClone->mInternalData->mData + paramBlockOffset);
		myClone->mInternalData->mParamBlockBuffers = (GpuParamBlockBufferPtr*)(myClone->mInternalData->mData + paramBlockBufferOffset);
		myClone->mInternalData->mTextures = (HTexture*)(myClone->mInternalData->mData + textureOffset);
		myClone->mInternalData->mSamplerStates = (HSamplerState*)(myClone->mInternalData->mData + samplerStateOffset);

		return myClone;
	}

	void GpuParams::getInternalBufferData(UINT32& bufferSize, UINT32& paramBlockOffset, UINT32& paramBlockBufferOffset,
		UINT32& textureOffset, UINT32& samplerStateOffset) const
	{
		UINT32 paramBlockBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlock*);
		UINT32 paramBlockBuffersBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = mInternalData->mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = mInternalData->mNumSamplerStates * sizeof(HSamplerState);

		bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;
		paramBlockOffset = 0;
		paramBlockBufferOffset = paramBlockOffset + paramBlockBufferSize;
		textureOffset = paramBlockBufferOffset + paramBlockBuffersBufferSize;
		samplerStateOffset = textureOffset + textureBufferSize;
	}
}