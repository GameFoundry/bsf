#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsVector2.h"
#include "BsTexture.h"
#include "BsSamplerState.h"
#include "BsFrameAlloc.h"
#include "BsDebug.h"
#include "BsException.h"

namespace BansheeEngine
{
	GpuParamsBase::GpuParamsBase(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		:mParamDesc(paramDesc), mTransposeMatrices(transposeMatrices), mNumParamBlocks(0), mNumSamplerStates(0),
		mNumTextures(0), mTextureInfo(nullptr)
	{
		for (auto& paramBlock : mParamDesc->paramBlocks)
		{
			if ((paramBlock.second.slot + 1) > mNumParamBlocks)
				mNumParamBlocks = paramBlock.second.slot + 1;
		}

		for (auto& texture : mParamDesc->textures)
		{
			if ((texture.second.slot + 1) > mNumTextures)
				mNumTextures = texture.second.slot + 1;
		}

		for (auto& sampler : mParamDesc->samplers)
		{
			if ((sampler.second.slot + 1) > mNumSamplerStates)
				mNumSamplerStates = sampler.second.slot + 1;
		}

		mTextureInfo = bs_newN<BoundTextureInfo>(mNumTextures);
	}

	GpuParamsBase::~GpuParamsBase()
	{
		bs_deleteN(mTextureInfo, mNumTextures);
	}

UINT32 GpuParamsBase::getDataParamSize(const String& name) const
	{
		GpuParamDataDesc* desc = getParamDesc(name);
		if(desc != nullptr)
			return desc->elementSize * 4;

		return 0;
	}

	bool GpuParamsBase::hasParam(const String& name) const
	{
		return getParamDesc(name) != nullptr;
	}

	bool GpuParamsBase::hasTexture(const String& name) const
	{
		auto paramIter = mParamDesc->textures.find(name);
		if(paramIter != mParamDesc->textures.end())
			return true;

		return false;
	}

	bool GpuParamsBase::hasSamplerState(const String& name) const
	{
		auto paramIter = mParamDesc->samplers.find(name);
		if(paramIter != mParamDesc->samplers.end())
			return true;

		return false;
	}

	bool GpuParamsBase::hasParamBlock(const String& name) const
	{
		auto paramBlockIter = mParamDesc->paramBlocks.find(name);
		if(paramBlockIter != mParamDesc->paramBlocks.end())
			return true;

		return false;
	}

	GpuParamDataDesc* GpuParamsBase::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc->params.find(name);
		if (paramIter != mParamDesc->params.end())
			return &paramIter->second;

		return nullptr;
	}

	bool GpuParamsBase::isLoadStoreTexture(UINT32 slot) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextureInfo[slot].isLoadStore;
	}

	void GpuParamsBase::setIsLoadStoreTexture(UINT32 slot, bool isLoadStore)
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		mTextureInfo[slot].isLoadStore = isLoadStore;
	}

	const TextureSurface& GpuParamsBase::getLoadStoreSurface(UINT32 slot) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextureInfo[slot].surface;
	}

	void GpuParamsBase::setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		mTextureInfo[slot].surface = surface;
	}

	GpuParamsCore::GpuParamsCore(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		: TGpuParams(paramDesc, transposeMatrices)
	{

	}

	void GpuParamsCore::updateHardwareBuffers()
	{
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			if (mParamBlockBuffers[i] != nullptr)
			{
				mParamBlockBuffers[i]->flushToGPU();
			}
		}
	}

	void GpuParamsCore::syncToCore(const CoreSyncData& data)
	{
		UINT32 textureInfoSize = mNumTextures * sizeof(BoundTextureInfo);
		UINT32 paramBufferSize = mNumParamBlocks * sizeof(SPtr<GpuParamBlockBufferCore>);
		UINT32 textureArraySize = mNumTextures * sizeof(SPtr<TextureCore>);
		UINT32 samplerArraySize = mNumSamplerStates * sizeof(SPtr<SamplerStateCore>);

		UINT32 totalSize = textureInfoSize + paramBufferSize + textureArraySize + samplerArraySize;

		UINT32 textureInfoOffset = 0;
		UINT32 paramBufferOffset = textureInfoOffset + textureInfoSize;
		UINT32 textureArrayOffset = paramBufferOffset + paramBufferSize;
		UINT32 samplerArrayOffset = textureArrayOffset + textureArraySize;

		assert(data.getBufferSize() == totalSize);

		UINT8* dataPtr = data.getBuffer();

		BoundTextureInfo* textureInfos = (BoundTextureInfo*)(dataPtr + textureInfoOffset);
		SPtr<GpuParamBlockBufferCore>* paramBuffers = (SPtr<GpuParamBlockBufferCore>*)(dataPtr + paramBufferOffset);
		SPtr<TextureCore>* textures = (SPtr<TextureCore>*)(dataPtr + textureArrayOffset);
		SPtr<SamplerStateCore>* samplers = (SPtr<SamplerStateCore>*)(dataPtr + samplerArrayOffset);

		// Copy & destruct
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			mParamBlockBuffers[i] = paramBuffers[i];
			paramBuffers[i].~SPtr<GpuParamBlockBufferCore>();
		}

		for (UINT32 i = 0; i < mNumTextures; i++)
		{
			mTextureInfo[i] = textureInfos[i];
			textureInfos[i].~BoundTextureInfo();

			mTextures[i] = textures[i];
			textures[i].~SPtr<TextureCore>();
		}

		for (UINT32 i = 0; i < mNumSamplerStates; i++)
		{
			mSamplerStates[i] = samplers[i];
			samplers[i].~SPtr<SamplerStateCore>();
		}
	}

	SPtr<GpuParamsCore> GpuParamsCore::create(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
	{
		GpuParamsCore* params = new (bs_alloc<GpuParamsCore>()) GpuParamsCore(paramDesc, transposeMatrices);
		SPtr<GpuParamsCore> paramsPtr = bs_shared_ptr<GpuParamsCore, GenAlloc>(params);
		paramsPtr->_setThisPtr(paramsPtr);

		return paramsPtr;
	}

	GpuParams::GpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		: TGpuParams(paramDesc, transposeMatrices)
	{

	}

	SPtr<GpuParamsCore> GpuParams::getCore() const
	{
		return std::static_pointer_cast<GpuParamsCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuParams::createCore() const
	{
		GpuParamsCore* obj = new (bs_alloc<GpuParamsCore>()) GpuParamsCore(mParamDesc, mTransposeMatrices);

		SPtr<CoreObjectCore> coreObj = bs_shared_ptr<GpuParamsCore, GenAlloc>(obj);
		coreObj->_setThisPtr(coreObj);

		return coreObj;
	}

	void GpuParams::_markCoreDirty()
	{
		markCoreDirty();
	}

	SPtr<GpuParams> GpuParams::create(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
	{
		GpuParams* params = new (bs_alloc<GpuParams>()) GpuParams(paramDesc, transposeMatrices);
		SPtr<GpuParams> paramsPtr = bs_core_ptr<GpuParams, GenAlloc>(params);
		paramsPtr->_setThisPtr(paramsPtr);
		paramsPtr->initialize();
		
		return paramsPtr;
	}

	CoreSyncData GpuParams::syncToCore(FrameAlloc* allocator)
	{
		UINT32 textureInfoSize = mNumTextures * sizeof(BoundTextureInfo);
		UINT32 paramBufferSize = mNumParamBlocks * sizeof(SPtr<GpuParamBlockBufferCore>);
		UINT32 textureArraySize = mNumTextures * sizeof(SPtr<TextureCore>);
		UINT32 samplerArraySize = mNumSamplerStates * sizeof(SPtr<SamplerStateCore>);

		UINT32 totalSize = textureInfoSize + paramBufferSize + textureArraySize + samplerArraySize;

		UINT32 textureInfoOffset = 0;
		UINT32 paramBufferOffset = textureInfoOffset + textureInfoSize;
		UINT32 textureArrayOffset = paramBufferOffset + paramBufferSize;
		UINT32 samplerArrayOffset = textureArrayOffset + textureArraySize;

		UINT8* data = allocator->alloc(totalSize);

		BoundTextureInfo* textureInfos = (BoundTextureInfo*)(data + textureInfoOffset);
		SPtr<GpuParamBlockBufferCore>* paramBuffers = (SPtr<GpuParamBlockBufferCore>*)(data + paramBufferOffset);
		SPtr<TextureCore>* textures = (SPtr<TextureCore>*)(data + textureArrayOffset);
		SPtr<SamplerStateCore>* samplers = (SPtr<SamplerStateCore>*)(data + samplerArrayOffset);

		// Construct & copy
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			new (&paramBuffers[i]) SPtr<GpuParamBlockBufferCore>();

			if (mParamBlockBuffers[i] != nullptr)
				paramBuffers[i] = mParamBlockBuffers[i]->getCore();
		}

		for (UINT32 i = 0; i < mNumTextures; i++)
		{
			new (&textureInfos[i]) BoundTextureInfo();
			textureInfos[i] = mTextureInfo[i];

			new (&textures[i]) SPtr<TextureCore>();

			if (mTextures[i].isLoaded())
				textures[i] = mTextures[i]->getCore();
			else
				textures[i] = nullptr;
		}

		for (UINT32 i = 0; i < mNumSamplerStates; i++)
		{
			new (&samplers[i]) SPtr<SamplerStateCore>();

			if (mSamplerStates[i].isLoaded())
				samplers[i] = mSamplerStates[i]->getCore();
			else
				samplers[i] = nullptr;
		}

		return CoreSyncData(data, totalSize);
	}
}