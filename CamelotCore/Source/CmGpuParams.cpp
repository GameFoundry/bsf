#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmBindableGpuParamBlock.h"
#include "CmVector2.h"
#include "CmDebug.h"
#include "CmException.h"

namespace BansheeEngine
{
	GpuParams::GpuParams(GpuParamDesc& paramDesc, bool transposeMatrices)
		:mParamDesc(paramDesc), mTransposeMatrices(transposeMatrices), mData(nullptr), mNumParamBlocks(0), mNumTextures(0), mNumSamplerStates(0),
		mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{
		for(auto& paramBlock : mParamDesc.paramBlocks)
		{
			if((paramBlock.second.slot + 1) > mNumParamBlocks)
				mNumParamBlocks = paramBlock.second.slot + 1;
		}

		for(auto& texture : mParamDesc.textures)
		{
			if((texture.second.slot + 1) > mNumTextures)
				mNumTextures = texture.second.slot + 1;
		}

		for(auto& sampler : mParamDesc.samplers)
		{
			if((sampler.second.slot + 1) > mNumSamplerStates)
				mNumSamplerStates = sampler.second.slot + 1;
		}

		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 paramBlockBufferSize = mNumParamBlocks * sizeof(GpuParamBlock*);
		UINT32 paramBlockBuffersBufferSize = mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = mNumSamplerStates * sizeof(HSamplerState);

		UINT32 bufferSize = paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;

		mData = (UINT8*)cm_alloc(bufferSize);
		
		UINT8* dataIter = mData;
		mParamBlocks = (GpuParamBlock**)dataIter;
		dataIter += paramBlockBufferSize;

		mParamBlockBuffers = (GpuParamBlockBufferPtr*)dataIter;
		dataIter += paramBlockBuffersBufferSize;

		mTextures = (HTexture*)dataIter;
		dataIter += textureBufferSize;

		mSamplerStates = (HSamplerState*)dataIter;

		// Ensure everything is constructed
		for(UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			mParamBlocks[i] = nullptr;

			GpuParamBlockBufferPtr* ptrToIdx = (&mParamBlockBuffers[i]);
			ptrToIdx = new (&mParamBlockBuffers[i]) GpuParamBlockBufferPtr(nullptr);
		}

		for(UINT32 i = 0; i < mNumTextures; i++)
		{
			HTexture* ptrToIdx = (&mTextures[i]);
			ptrToIdx = new (&mTextures[i]) HTexture();
		}

		for(UINT32 i = 0; i < mNumSamplerStates; i++)
		{
			HSamplerState* ptrToIdx = (&mSamplerStates[i]);
			ptrToIdx = new (&mSamplerStates[i]) HSamplerState();
		}

		// Create parameter handles
		for(auto& param : mParamDesc.params)
		{
			switch(param.second.type)
			{
			case GPDT_FLOAT1:
				mFloatParams[param.second.name] = GpuParamFloat(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_FLOAT2:
				mVec2Params[param.second.name] = GpuParamVec2(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_FLOAT3:
				mVec3Params[param.second.name] = GpuParamVec3(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_FLOAT4:
				mVec4Params[param.second.name] = GpuParamVec4(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_MATRIX_3X3:
				mMat3Params[param.second.name] = GpuParamMat3(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_MATRIX_4X4:
				mMat4Params[param.second.name] = GpuParamMat4(&param.second, mParamBlocks, mTransposeMatrices);
				break;
			case GPDT_STRUCT:
				mStructParams[param.second.name] = GpuParamStruct(&param.second, mParamBlocks);
				break;
			}
		}

		for(auto& texture : mParamDesc.textures)
			mTextureParams[texture.second.name] = GpuParamTexture(&texture.second, mTextures);

		for(auto& sampler : mParamDesc.samplers)
			mSampStateParams[sampler.second.name] = GpuParamSampState(&sampler.second, mSamplerStates);
	}

	GpuParams::~GpuParams()
	{
		// Free params
		for(auto& param : mFloatParams)
			param.second._destroy();

		for(auto& param : mVec2Params)
			param.second._destroy();

		for(auto& param : mVec3Params)
			param.second._destroy();

		for(auto& param : mVec4Params)
			param.second._destroy();

		for(auto& param : mMat3Params)
			param.second._destroy();

		for(auto& param : mMat4Params)
			param.second._destroy();

		for(auto& param : mStructParams)
			param.second._destroy();

		for(auto& param : mTextureParams)
			param.second._destroy();

		for(auto& param : mSampStateParams)
			param.second._destroy();

		// Ensure everything is destructed
		for(UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			mParamBlockBuffers[i].~shared_ptr();
		}

		for(UINT32 i = 0; i < mNumTextures; i++)
			mTextures[i].~ResourceHandle();

		for(UINT32 i = 0; i < mNumSamplerStates; i++)
			mSamplerStates[i].~ResourceHandle();

		cm_free(mData);
	}

	void GpuParams::setParamBlockBuffer(UINT32 slot, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		if(slot < 0 || slot >= mNumParamBlocks)
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		mParamBlockBuffers[slot] = paramBlockBuffer;
		mParamBlocks[slot] = paramBlockBuffer->getParamBlock();
	}

	void GpuParams::setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer)
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;
		mParamBlocks[iterFind->second.slot] = paramBlockBuffer->getParamBlock();
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
		auto iterFind = mStructParams.find(name);

		if(iterFind == mStructParams.end())
			CM_EXCEPT(InvalidParametersException, "Cannot find struct parameter with the name '" + name + "'");

		output = iterFind->second;
	}

	void GpuParams::getTextureParam(const String& name, GpuParamTexture& output) const
	{
		auto iterFind = mTextureParams.find(name);

		if(iterFind == mTextureParams.end())
			CM_EXCEPT(InvalidParametersException, "Cannot find texture parameter with the name '" + name + "'");

		output = iterFind->second;
	}

	void GpuParams::getSamplerStateParam(const String& name, GpuParamSampState& output) const
	{
		auto iterFind = mSampStateParams.find(name);

		if(iterFind == mSampStateParams.end())
			CM_EXCEPT(InvalidParametersException, "Cannot find sampler state parameter with the name '" + name + "'");

		output = iterFind->second;
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc.params.find(name);
		if(paramIter != mParamDesc.params.end())
			return &paramIter->second;

		return nullptr;
	}
}