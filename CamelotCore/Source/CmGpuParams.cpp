#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmBindableGpuParamBlock.h"
#include "CmVector2.h"
#include "CmDebug.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuParams::GpuParams(GpuParamDesc& paramDesc)
		:mParamDesc(paramDesc), mTransposeMatrices(false), mData(nullptr), mNumParamBlocks(0), mNumTextures(0), mNumSamplerStates(0),
		mParamBlocks(nullptr), mParamBlockBuffers(nullptr), mTextures(nullptr), mSamplerStates(nullptr)
	{
		for(auto iter = mParamDesc.paramBlocks.begin(); iter != mParamDesc.paramBlocks.end(); ++iter)
		{
			if((iter->second.slot + 1) > mNumParamBlocks)
				mNumParamBlocks = iter->second.slot + 1;
		}

		for(auto iter = mParamDesc.textures.begin(); iter != mParamDesc.textures.end(); ++iter)
		{
			if((iter->second.slot + 1) > mNumTextures)
				mNumTextures = iter->second.slot + 1;
		}

		for(auto iter = mParamDesc.samplers.begin(); iter != mParamDesc.samplers.end(); ++iter)
		{
			if((iter->second.slot + 1) > mNumSamplerStates)
				mNumSamplerStates = iter->second.slot + 1;
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
	}

	GpuParams::~GpuParams()
	{
		// Ensure everything is destructed
		for(UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			if(mParamBlocks[i] != nullptr)
				cm_delete<PoolAlloc>(mParamBlocks[i]);

			mParamBlockBuffers[i].~shared_ptr();
		}

		for(UINT32 i = 0; i < mNumTextures; i++)
			mTextures[i].~ResourceHandle();

		for(UINT32 i = 0; i < mNumSamplerStates; i++)
			mSamplerStates[i].~ResourceHandle();

		cm_free(mData);
	}

	void GpuParams::setParamBlockBuffer(UINT32 slot, GpuParamBlockBufferPtr paramBlockBuffer)
	{
		if(slot < 0 || slot >= mNumParamBlocks)
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		if(mParamBlocks[slot] != nullptr)
			cm_delete<PoolAlloc>(mParamBlocks[slot]);

		mParamBlocks[slot] = cm_new<GpuParamBlock, PoolAlloc>(paramBlockBuffer->getSize());
		mParamBlockBuffers[slot] = paramBlockBuffer;
	}

	void GpuParams::setParamBlockBuffer(const String& name, GpuParamBlockBufferPtr paramBlockBuffer)
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		if(mParamBlocks[iterFind->second.slot] != nullptr)
			cm_delete<PoolAlloc>(mParamBlocks[iterFind->second.slot]);

		mParamBlocks[iterFind->second.slot] = cm_new<GpuParamBlock, PoolAlloc>(paramBlockBuffer->getSize());
		mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;
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

	void GpuParams::setParam(const String& name, float value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, int value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(int), arrayIndex);
	}

	void GpuParams::setParam(const String& name, bool value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(bool), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector4& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 4 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector3& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 3 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector2& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 2 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Matrix4& mat, UINT32 arrayIndex)
	{
		if (mTransposeMatrices)
		{
			Matrix4 transMat = mat.transpose();
			setParam(name, (void*)&transMat, 16 * sizeof(float), arrayIndex);
		}
		else
		{
			setParam(name, (void*)&mat, 16 * sizeof(float), arrayIndex);
		}
	}

	void GpuParams::setParam(const String& name, const Matrix3& mat, UINT32 arrayIndex)
	{
		if (mTransposeMatrices)
		{
			Matrix3 transMat = mat.transpose();
			setParam(name, (void*)&transMat, 9 * sizeof(float), arrayIndex);
		}
		else
		{
			setParam(name, (void*)&mat, 9 * sizeof(float), arrayIndex);
		}
	}

	void GpuParams::setParam(const String& name, const Color& color, UINT32 arrayIndex)
	{
		setParam(name, (void*)&color, 4 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const void* value, UINT32 sizeBytes, UINT32 arrayIndex)
	{
		GpuParamDataDesc* desc = getParamDesc(name);

		if(desc == nullptr)
		{
			LOGWRN("Cannot find parameter with the name '" + name + "'");
			return;
		}

		if(arrayIndex >= desc->arraySize)
		{
			CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
				toString(desc->arraySize) + ". Requested size: " + toString(arrayIndex));
		}

		UINT32 elementSizeBytes = desc->elementSize * sizeof(UINT32);
		if(sizeBytes > elementSizeBytes)
		{
			CM_EXCEPT(InvalidParametersException, "Provided element size larger than maximum element size. Maximum size: " + 
				toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
		}

		GpuParamBlock* paramBlock = mParamBlocks[desc->paramBlockSlot];

		if(paramBlock == nullptr)
		{
			LOGWRN("Parameter exists but there is no ParamBlock set.");
			return;
		}

		paramBlock->write((desc->cpuMemOffset + arrayIndex * desc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);

		// Set unused bytes to 0
		if(sizeBytes < elementSizeBytes)
		{
			UINT32 diffSize = elementSizeBytes - sizeBytes;
			paramBlock->zeroOut((desc->cpuMemOffset + arrayIndex * desc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
		}
	}

	void GpuParams::setTexture(const String& name, const HTexture& val)
	{
		auto paramIter = mParamDesc.textures.find(name);
		if(paramIter == mParamDesc.textures.end())
		{
			LOGWRN("Texture with the name '" + name + "' doesn't exist.");
			return;
		}

		mTextures[paramIter->second.slot] = val;
	}

	void GpuParams::setSamplerState(const String& name, const HSamplerState& val)
	{
		auto paramIter = mParamDesc.samplers.find(name);
		if(paramIter == mParamDesc.samplers.end())
		{
			LOGWRN("Sampler with the name '" + name + "' doesn't exist.");
			return;
		}

		mSamplerStates[paramIter->second.slot] = val;
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc.params.find(name);
		if(paramIter != mParamDesc.params.end())
			return &paramIter->second;

		return nullptr;
	}

	BindableGpuParams GpuParams::createBindableCopy(const GpuParamsPtr& params)
	{
		// Allocate everything in a single block of memory to get rid of extra memory allocations
		UINT32 paramBlockBufferSize = params->mNumParamBlocks * sizeof(GpuParamBlock*);
		UINT32 paramBlockBuffersBufferSize = params->mNumParamBlocks * sizeof(GpuParamBlockBufferPtr);
		UINT32 textureBufferSize = params->mNumTextures * sizeof(HTexture);
		UINT32 samplerStateBufferSize = params->mNumSamplerStates * sizeof(HSamplerState);

		UINT32 bufferSize = sizeof(BindableGpuParamBlock) + paramBlockBufferSize + paramBlockBuffersBufferSize + textureBufferSize + samplerStateBufferSize;
		for(UINT32 i = 0; i < params->mNumParamBlocks; i++)
		{
			if(params->mParamBlocks[i] != nullptr)
				bufferSize += params->mParamBlocks[i]->getSize();
		}

		// TODO - Alloc using stack
		BindableGpuParams bindableParams(params->mParamDesc);
		bindableParams.mData = (UINT8*)cm_alloc(bufferSize);
		bindableParams.mNumParamBlocks = params->mNumParamBlocks;
		bindableParams.mNumTextures = params->mNumTextures;
		bindableParams.mNumSamplerStates = params->mNumSamplerStates;

		UINT8* dataIter = bindableParams.mData;
		bindableParams.mParamBlocks = (BindableGpuParamBlock**)dataIter;
		dataIter += paramBlockBufferSize;

		bindableParams.mParamBlockBuffers = (GpuParamBlockBufferPtr*)dataIter;
		dataIter += paramBlockBuffersBufferSize;

		bindableParams.mTextures = (HTexture*)dataIter;
		dataIter += textureBufferSize;

		bindableParams.mSamplerStates = (HSamplerState*)dataIter;
		dataIter += samplerStateBufferSize;

		// Copy data
		memcpy(bindableParams.mParamBlocks, params->mParamBlocks, paramBlockBufferSize);
		memcpy(bindableParams.mParamBlockBuffers, params->mParamBlockBuffers, paramBlockBuffersBufferSize);
		memcpy(bindableParams.mTextures, params->mTextures, textureBufferSize);
		memcpy(bindableParams.mSamplerStates, params->mSamplerStates, samplerStateBufferSize);

		for(UINT32 i = 0; i < params->mNumParamBlocks; i++)
		{
			if(params->mParamBlocks[i] != nullptr)
			{
				UINT32 bufferSize = params->mParamBlocks[i]->getSize();
				bindableParams.mParamBlocks[i] = (BindableGpuParamBlock*)dataIter;
				dataIter += sizeof(BindableGpuParamBlock);

				bindableParams.mParamBlocks[i]->mData = dataIter;
				dataIter += bufferSize;

				memcpy(bindableParams.mParamBlocks[i]->mData, params->mParamBlocks[i]->getData(), bufferSize);
				bindableParams.mParamBlocks[i]->mSize = bufferSize;
				bindableParams.mParamBlocks[i]->mDirty = params->mParamBlocks[i]->isDirty();
			}
		}

		return bindableParams;
	}
}