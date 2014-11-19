#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsVector2.h"
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
}