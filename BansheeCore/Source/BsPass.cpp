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
	/**
	 * @brief	Converts a sim thread pass descriptor to a core thread one.
	 */
	void convertPassDesc(const PASS_DESC& input, PASS_DESC_CORE& output)
	{
		output.blendState = input.blendState != nullptr ? input.blendState->getCore() : nullptr;
		output.rasterizerState = input.rasterizerState != nullptr ? input.rasterizerState->getCore() : nullptr;
		output.depthStencilState = input.depthStencilState != nullptr ? input.depthStencilState->getCore() : nullptr;
		output.stencilRefValue = input.stencilRefValue;
		output.vertexProgram = input.vertexProgram != nullptr ? input.vertexProgram->getCore() : nullptr;
		output.fragmentProgram = input.fragmentProgram != nullptr ? input.fragmentProgram->getCore() : nullptr;
		output.geometryProgram = input.geometryProgram != nullptr ? input.geometryProgram->getCore() : nullptr;
		output.hullProgram = input.hullProgram != nullptr ? input.hullProgram->getCore() : nullptr;
		output.domainProgram = input.domainProgram != nullptr ? input.domainProgram->getCore() : nullptr;
		output.hullProgram = input.hullProgram != nullptr ? input.hullProgram->getCore() : nullptr;
	}

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

	void PassCore::syncToCore(const CoreSyncData& data)
	{
		UINT8* dataPtr = data.getBuffer();
		PASS_DESC_CORE* desc = (PASS_DESC_CORE*)dataPtr;

		mData = *desc;
		desc->~PASS_DESC_CORE();
	}

	SPtr<PassCore> PassCore::create(const PASS_DESC_CORE& desc)
	{
		PassCore* newPass = new (bs_alloc<PassCore>()) PassCore(desc);
		SPtr<PassCore> newPassPtr = bs_shared_ptr<PassCore>(newPass);
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
		convertPassDesc(mData, desc);

		PassCore* pass = new (bs_alloc<PassCore>()) PassCore(desc);
		SPtr<PassCore> passPtr = bs_shared_ptr<PassCore>(pass);
		passPtr->_setThisPtr(passPtr);

		return passPtr;
	}

	CoreSyncData Pass::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(PASS_DESC_CORE);

		UINT8* data = allocator->alloc(size);
		PASS_DESC_CORE* passDesc = new (data) PASS_DESC_CORE();
		convertPassDesc(mData, *passDesc);

		return CoreSyncData(data, size);
	}

	void Pass::getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies)
	{
		if (mData.blendState != nullptr)
			dependencies.push_back(mData.blendState);

		if (mData.rasterizerState != nullptr)
			dependencies.push_back(mData.rasterizerState);

		if (mData.depthStencilState != nullptr)
			dependencies.push_back(mData.depthStencilState);

		if (mData.vertexProgram != nullptr)
			dependencies.push_back(mData.vertexProgram);

		if (mData.fragmentProgram != nullptr)
			dependencies.push_back(mData.fragmentProgram);

		if (mData.geometryProgram != nullptr)
			dependencies.push_back(mData.geometryProgram);

		if (mData.hullProgram != nullptr)
			dependencies.push_back(mData.hullProgram);

		if (mData.domainProgram != nullptr)
			dependencies.push_back(mData.domainProgram);

		if (mData.computeProgram != nullptr)
			dependencies.push_back(mData.computeProgram);
	}

	PassPtr Pass::create(const PASS_DESC& desc)
	{
		Pass* newPass = new (bs_alloc<Pass>()) Pass(desc);
		PassPtr newPassPtr = bs_core_ptr<Pass>(newPass);
		newPassPtr->_setThisPtr(newPassPtr);
		newPassPtr->initialize();

		return newPassPtr;
	}

	PassPtr Pass::createEmpty()
	{
		Pass* newPass = new (bs_alloc<Pass>()) Pass();
		PassPtr newPassPtr = bs_core_ptr<Pass>(newPass);
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