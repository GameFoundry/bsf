#include "BsGpuParam.h"
#include "BsGpuParams.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 							GPU PARAMS BASE                      		*/
	/************************************************************************/

	GpuParamBase::GpuParamBase()
	{ }

	GpuParamBase::GpuParamBase(const SPtr<GpuParams>& parent)
		:mParent(parent)
	{ }

	void GpuParamBase::markCoreDirty()
	{
		mParent->_markCoreDirty();
	}

	/************************************************************************/
	/* 							GPU DATA PARAMS BASE                      	*/
	/************************************************************************/

	GpuDataParamBase::GpuDataParamBase()
		: GpuParamBase(), mParamDesc(nullptr)
	{ }

	GpuDataParamBase::GpuDataParamBase(GpuParamDataDesc* paramDesc, const SPtr<GpuParams>& parent)
		: GpuParamBase(parent), mParamDesc(paramDesc)
	{ }

	bool GpuDataParamBase::getTransposeMatrices() const
	{
		return mParent->getTransposeMatrices();
	}

	bool GpuDataParamBase::write(UINT32 offset, const void* data, UINT32 size)
	{
		GpuParamBlockBufferPtr paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return false;

		paramBlock->write(offset, data, size);
		return true;
	}

	bool GpuDataParamBase::read(UINT32 offset, void* data, UINT32 size)
	{
		GpuParamBlockBufferPtr paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return false;

		paramBlock->read(offset, data, size);
		return true;
	}

	void GpuDataParamBase::zeroOut(UINT32 offset, UINT32 size)
	{
		GpuParamBlockBufferPtr paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return;

		paramBlock->zeroOut(offset, size);
	}

	/************************************************************************/
	/* 						GPU OBJECT PARAMS BASE                      	*/
	/************************************************************************/

	GpuObjectParamBase::GpuObjectParamBase()
		: GpuParamBase(), mParamDesc(nullptr)
	{ }

	GpuObjectParamBase::GpuObjectParamBase(GpuParamObjectDesc* paramDesc, const SPtr<GpuParams>& parent)
		: GpuParamBase(parent), mParamDesc(paramDesc)
	{ }

	HTexture GpuObjectParamBase::getTexture(UINT32 slot)
	{
		return mParent->getTexture(slot);
	}

	HSamplerState GpuObjectParamBase::getSamplerState(UINT32 slot)
	{
		return mParent->getSamplerState(slot);
	}

	void GpuObjectParamBase::setTexture(UINT32 slot, const HTexture& texture)
	{
		mParent->setTexture(slot, texture);
	}

	void GpuObjectParamBase::setSamplerState(UINT32 slot, const HSamplerState& sampler)
	{
		mParent->setSamplerState(slot, sampler);
	}

	void GpuObjectParamBase::setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const
	{
		mParent->setLoadStoreSurface(slot, surface);
	}

	void GpuObjectParamBase::setIsLoadStoreTexture(UINT32 slot, bool isLoadStore)
	{
		mParent->setIsLoadStoreTexture(slot, isLoadStore);
	}

	/************************************************************************/
	/* 							GPU PARAMS CORE BASE                      	*/
	/************************************************************************/

	GpuParamCoreBase::GpuParamCoreBase()
	{ }

	GpuParamCoreBase::GpuParamCoreBase(const SPtr<GpuParamsCore>& parent)
		:mParent(parent)
	{ }

	void GpuParamCoreBase::markCoreDirty()
	{
		mParent->_markCoreDirty();
	}

	/************************************************************************/
	/* 						GPU DATA PARAMS CORE BASE                      	*/
	/************************************************************************/

	GpuDataParamCoreBase::GpuDataParamCoreBase()
		: GpuParamCoreBase(), mParamDesc(nullptr)
	{ }

	GpuDataParamCoreBase::GpuDataParamCoreBase(GpuParamDataDesc* paramDesc, const SPtr<GpuParamsCore>& parent)
		: GpuParamCoreBase(parent), mParamDesc(paramDesc)
	{ }

	bool GpuDataParamCoreBase::getTransposeMatrices() const
	{
		return mParent->getTransposeMatrices();
	}

	bool GpuDataParamCoreBase::write(UINT32 offset, const void* data, UINT32 size)
	{
		SPtr<GpuParamBlockBufferCore> paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return false;

		paramBlock->write(offset, data, size);
		return true;
	}

	bool GpuDataParamCoreBase::read(UINT32 offset, void* data, UINT32 size)
	{
		SPtr<GpuParamBlockBufferCore> paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return false;

		paramBlock->read(offset, data, size);
		return true;
	}

	void GpuDataParamCoreBase::zeroOut(UINT32 offset, UINT32 size)
	{
		SPtr<GpuParamBlockBufferCore> paramBlock = mParent->getParamBlockBuffer(mParamDesc->paramBlockSlot);
		if (paramBlock == nullptr)
			return;

		paramBlock->zeroOut(offset, size);
	}

	/************************************************************************/
	/* 						GPU OBJECT PARAMS CORE BASE                    	*/
	/************************************************************************/

	GpuObjectParamCoreBase::GpuObjectParamCoreBase()
		: GpuParamCoreBase(), mParamDesc(nullptr)
	{ }

	GpuObjectParamCoreBase::GpuObjectParamCoreBase(GpuParamObjectDesc* paramDesc, const SPtr<GpuParamsCore>& parent)
		: GpuParamCoreBase(parent), mParamDesc(paramDesc)
	{ }

	SPtr<TextureCore> GpuObjectParamCoreBase::getTexture(UINT32 slot)
	{
		return mParent->getTexture(slot);
	}

	SPtr<SamplerStateCore> GpuObjectParamCoreBase::getSamplerState(UINT32 slot)
	{
		return mParent->getSamplerState(slot);
	}

	void GpuObjectParamCoreBase::setTexture(UINT32 slot, const SPtr<TextureCore>& texture)
	{
		mParent->setTexture(slot, texture);
	}

	void GpuObjectParamCoreBase::setSamplerState(UINT32 slot, const SPtr<SamplerStateCore>& sampler)
	{
		mParent->setSamplerState(slot, sampler);
	}

	void GpuObjectParamCoreBase::setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const
	{
		mParent->setLoadStoreSurface(slot, surface);
	}

	void GpuObjectParamCoreBase::setIsLoadStoreTexture(UINT32 slot, bool isLoadStore)
	{
		mParent->setIsLoadStoreTexture(slot, isLoadStore);
	}
}