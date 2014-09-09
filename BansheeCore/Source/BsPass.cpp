#include "BsPass.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsPassRTTI.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsException.h"

namespace BansheeEngine
{
	Pass::Pass()
		:mStencilRefValue(0)
    { }

	Pass::Pass(const Pass& oth)
    {
        *this = oth;
    }

    Pass::~Pass()
    {

    }

    Pass& Pass::operator=(const Pass& oth)
    {
	    // Default blending (overwrite)
	    mBlendState = oth.mBlendState;
		mRasterizerState = oth.mRasterizerState;
		mDepthStencilState = oth.mDepthStencilState;
		mStencilRefValue = oth.mStencilRefValue;

		mVertexProgram = oth.mVertexProgram;
		mFragmentProgram = oth.mFragmentProgram;
		mGeometryProgram = oth.mGeometryProgram;
		mHullProgram = oth.mHullProgram;
		mDomainProgram = oth.mDomainProgram;
		mComputeProgram = oth.mComputeProgram;

		return *this;
    }

    bool Pass::hasBlending() const
    {
		bool transparent = false;

		if(mBlendState != nullptr)
		{
			for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
			{
				// Transparent if destination color is taken into account
				if (mBlendState->getDstBlend(i) != BF_ZERO ||
					mBlendState->getSrcBlend(i) == BF_DEST_COLOR ||
					mBlendState->getSrcBlend(i) == BF_INV_DEST_COLOR ||
					mBlendState->getSrcBlend(i) == BF_DEST_ALPHA ||
					mBlendState->getSrcBlend(i) == BF_INV_DEST_ALPHA)
				{
					transparent = true;
				}
			}
		}

		return transparent;
    }

	void Pass::setBlendState(HBlendState& blendState)
	{
		mBlendState = blendState;
	}

	HBlendState Pass::getBlendState() const
	{
		return mBlendState;
	}

	void Pass::setRasterizerState(HRasterizerState& rasterizerState)
	{
		mRasterizerState = rasterizerState;
	}

	HRasterizerState Pass::getRasterizerState() const
	{
		return mRasterizerState;
	}

	void Pass::setDepthStencilState(HDepthStencilState& depthStencilState)
	{
		mDepthStencilState = depthStencilState;
	}

	HDepthStencilState Pass::getDepthStencilState() const
	{
		return mDepthStencilState;
	}

	void Pass::setStencilRefValue(UINT32 refValue)
	{
		mStencilRefValue = refValue;
	}

	UINT32 Pass::getStencilRefValue() const
	{
		return mStencilRefValue;
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