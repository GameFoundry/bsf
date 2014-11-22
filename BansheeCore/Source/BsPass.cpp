#include "BsPass.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsPassRTTI.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsFrameAlloc.h"
#include "BsException.h"

namespace BansheeEngine
{
	bool propertiesHaveBlending(const BlendProperties& bsProps)
	{
		bool transparent = false;

		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			// Transparent if destination color is taken into account
			if (bsProps.getDstBlend(i) != BF_ZERO ||
				bsProps.getSrcBlend(i) == BF_DEST_COLOR ||
				bsProps.getSrcBlend(i) == BF_INV_DEST_COLOR ||
				bsProps.getSrcBlend(i) == BF_DEST_ALPHA ||
				bsProps.getSrcBlend(i) == BF_INV_DEST_ALPHA)
			{
				transparent = true;
			}
		}

		return transparent;
	}

	bool PassBase::hasBlending(const HBlendState& blendState)
	{
		if (blendState != nullptr)
			return propertiesHaveBlending(blendState->getProperties());

		return false;
	}

	bool PassBase::hasBlending(const SPtr<BlendStateCore>& blendState)
	{
		if (blendState != nullptr)
			return propertiesHaveBlending(blendState->getProperties());

		return false;
	}

	void PassCore::syncToCore(const CoreSyncData& data)
	{
		UINT8* buffer = data.getBuffer();

		SPtr<BlendStateCore>* blendState = (SPtr<BlendStateCore>*)buffer;
		mData.mBlendState = *blendState;
		blendState->~SPtr<BlendStateCore>();
		buffer += sizeof(SPtr<BlendStateCore>);

		SPtr<RasterizerStateCore>* rasterizerState = (SPtr<RasterizerStateCore>*)buffer;
		mData.mRasterizerState = *rasterizerState;
		rasterizerState->~SPtr<RasterizerStateCore>();
		buffer += sizeof(SPtr<RasterizerStateCore>);

		SPtr<DepthStencilStateCore>* depthStencilState = (SPtr<DepthStencilStateCore>*)buffer;
		mData.mDepthStencilState = *depthStencilState;
		depthStencilState->~SPtr<DepthStencilStateCore>();
		buffer += sizeof(SPtr<DepthStencilStateCore>);

		memcpy(&mData.mStencilRefValue, buffer, sizeof(UINT32));
		buffer += sizeof(UINT32);

		SPtr<GpuProgramCore>* allPrograms[6] = { &mData.mVertexProgram, &mData.mFragmentProgram, &mData.mGeometryProgram,
			&mData.mHullProgram, &mData.mDomainProgram, &mData.mComputeProgram };

		UINT32 numPrograms = sizeof(allPrograms) / sizeof(HGpuProgram*);
		for (UINT32 i = 0; i < numPrograms; i++)
		{
			SPtr<GpuProgramCore>* gpuProgram = (SPtr<GpuProgramCore>*)buffer;
			*allPrograms[i] = *gpuProgram;
			gpuProgram->~SPtr<GpuProgramCore>();
			buffer += sizeof(SPtr<GpuProgramCore>);
		}
	}

	SPtr<PassCore> PassCore::create()
	{
		PassCore* newPass = new (bs_alloc<PassCore>()) PassCore();
		SPtr<PassCore> newPassPtr = bs_shared_ptr<PassCore, GenAlloc>(newPass);
		newPassPtr->_setThisPtr(newPassPtr);
		newPassPtr->initialize();

		return newPassPtr;
	}

	SPtr<PassCore> Pass::getCore() const
	{
		return std::static_pointer_cast<PassCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Pass::createCore() const
	{
		PassCore* pass = new (bs_alloc<PassCore>()) PassCore();
		SPtr<PassCore> passPtr = bs_shared_ptr<PassCore, GenAlloc>(pass);
		passPtr->_setThisPtr(passPtr);

		return passPtr;
	}

	void Pass::_markCoreDirty()
	{
		markCoreDirty();
	}

	CoreSyncData Pass::syncToCore(FrameAlloc* alloc)
	{
		UINT32 size = sizeof(SPtr<BlendStateCore>) + sizeof(SPtr<RasterizerStateCore>) + 
			sizeof(SPtr<DepthStencilStateCore>) + sizeof(SPtr<GpuProgramCore>) * 6 + sizeof(UINT32);

		UINT8* data = alloc->alloc(size);
		UINT8* dataPtr = data;

		SPtr<BlendStateCore>* blendState = new (dataPtr)SPtr<BlendStateCore>();
		dataPtr += sizeof(SPtr<BlendStateCore>);

		if (mData.mBlendState != nullptr)
			*blendState = mData.mBlendState->getCore();
		else
			*blendState = nullptr;

		SPtr<RasterizerStateCore>* rasterizerState = new (dataPtr)SPtr<RasterizerStateCore>();
		dataPtr += sizeof(SPtr<RasterizerStateCore>);

		if (mData.mRasterizerState != nullptr)
			*rasterizerState = mData.mRasterizerState->getCore();
		else
			*rasterizerState = nullptr;

		SPtr<DepthStencilStateCore>* depthStencilState = new (dataPtr)SPtr<DepthStencilStateCore>();
		dataPtr += sizeof(SPtr<DepthStencilStateCore>);

		if (mData.mDepthStencilState != nullptr)
			*depthStencilState = mData.mDepthStencilState->getCore();
		else
			*depthStencilState = nullptr;

		memcpy(dataPtr, &mData.mStencilRefValue, sizeof(UINT32));
		dataPtr += sizeof(UINT32);

		HGpuProgram* allPrograms[6] = { &mData.mVertexProgram, &mData.mFragmentProgram, &mData.mGeometryProgram,
			&mData.mHullProgram, &mData.mDomainProgram, &mData.mComputeProgram };

		UINT32 numPrograms = sizeof(allPrograms) / sizeof(HGpuProgram*);
		for (UINT32 i = 0; i < numPrograms; i++)
		{
			SPtr<GpuProgramCore>* gpuProgram = new (dataPtr)SPtr<GpuProgramCore>();
			dataPtr += sizeof(SPtr<GpuProgramCore>);

			if (*allPrograms[i] != nullptr)
				*gpuProgram = (*allPrograms[i])->getCore();
			else
				*gpuProgram = nullptr;
		}

		return CoreSyncData(data, size);
	}

	PassPtr Pass::create()
	{
		PassPtr newPass = createEmpty();
		newPass->initialize();

		return newPass;
	}

	PassPtr Pass::createEmpty()
	{
		Pass* newPass = new (bs_alloc<Pass>()) Pass();
		PassPtr newPassPtr = bs_core_ptr<Pass, GenAlloc>(newPass);
		newPassPtr->_setThisPtr(newPassPtr);

		return newPassPtr;
	}

	RTTITypeBase* Pass::getRTTIStatic()
	{
		return PassRTTI::instance();
	}

	RTTITypeBase* Pass::getRTTI() const
	{
		return Pass::getRTTIStatic();
	}
}