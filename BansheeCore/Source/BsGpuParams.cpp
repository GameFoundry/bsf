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
		UINT32 paramBlockBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlock*);
		UINT32 paramBlockBuffersBufferSize = mInternalData->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = mInternalData->mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = mInternalData->mNumSamplerStates * sizeof(HSamplerState);

		UINT32 bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;

		mInternalData->mData = (UINT8*)bs_alloc(bufferSize);
		
		UINT8* dataIter = mInternalData->mData;
		mInternalData->mParamBlocks = (GpuParamBlock**)dataIter;
		dataIter += paramBlockBufferSize;

		mInternalData->mParamBlockBuffers = (GpuParamBlockBufferPtr*)dataIter;
		dataIter += paramBlockBuffersBufferSize;

		mInternalData->mTextures = (HTexture*)dataIter;
		dataIter += textureBufferSize;

		mInternalData->mSamplerStates = (HSamplerState*)dataIter;

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
}