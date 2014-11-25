#include "BsPass.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsPassRTTI.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsFrameAlloc.h"
#include "BsGpuProgram.h"
#include "BsException.h"

namespace BansheeEngine
{
	template<bool Core>
	TPass<Core>::TPass()
	{
		mData.stencilRefValue = 0;
	}

	template<bool Core>
	TPass<Core>::TPass(const PassDescType& data)
		:mData(data)
	{

	}

	template<bool Core>
	bool TPass<Core>::hasBlending() const 
	{ 
		if (!mData.blendState)
			return false;

		bool transparent = false;

		const BlendProperties& bsProps = mData.blendState->getProperties();
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

	template class TPass < false > ;
	template class TPass < true >;

	PassCore::PassCore(const PASS_DESC_CORE& desc)
		:TPass(desc)
	{ }

	SPtr<PassCore> PassCore::create(const PASS_DESC_CORE& desc)
	{
		PassCore* newPass = new (bs_alloc<PassCore>()) PassCore(desc);
		SPtr<PassCore> newPassPtr = bs_shared_ptr<PassCore, GenAlloc>(newPass);
		newPassPtr->_setThisPtr(newPassPtr);
		newPassPtr->initialize();

		return newPassPtr;
	}

	Pass::Pass(const PASS_DESC& desc)
		:TPass(desc)
	{ }

	SPtr<PassCore> Pass::getCore() const
	{
		return std::static_pointer_cast<PassCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Pass::createCore() const
	{
		PASS_DESC_CORE desc;
		desc.blendState = mData.blendState.isLoaded() ? mData.blendState->getCore() : nullptr;
		desc.rasterizerState = mData.rasterizerState.isLoaded() ? mData.rasterizerState->getCore() : nullptr;
		desc.depthStencilState = mData.depthStencilState.isLoaded() ? mData.depthStencilState->getCore() : nullptr;
		desc.stencilRefValue = mData.stencilRefValue;
		desc.vertexProgram = mData.vertexProgram != nullptr ? mData.vertexProgram->getCore() : nullptr;
		desc.fragmentProgram = mData.fragmentProgram != nullptr ? mData.fragmentProgram->getCore() : nullptr;
		desc.geometryProgram = mData.geometryProgram != nullptr ? mData.geometryProgram->getCore() : nullptr;
		desc.hullProgram = mData.hullProgram != nullptr ? mData.hullProgram->getCore() : nullptr;
		desc.domainProgram = mData.domainProgram != nullptr ? mData.domainProgram->getCore() : nullptr;
		desc.hullProgram = mData.hullProgram != nullptr ? mData.hullProgram->getCore() : nullptr;

		PassCore* pass = new (bs_alloc<PassCore>()) PassCore(desc);
		SPtr<PassCore> passPtr = bs_shared_ptr<PassCore, GenAlloc>(pass);
		passPtr->_setThisPtr(passPtr);

		return passPtr;
	}

	PassPtr Pass::create(const PASS_DESC& desc)
	{
		Pass* newPass = new (bs_alloc<Pass>()) Pass(desc);
		PassPtr newPassPtr = bs_core_ptr<Pass, GenAlloc>(newPass);
		newPassPtr->_setThisPtr(newPassPtr);
		newPassPtr->initialize();

		return newPassPtr;
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