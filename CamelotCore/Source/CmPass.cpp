#include "CmPass.h"
#include "CmRasterizerState.h"
#include "CmBlendState.h"
#include "CmDepthStencilState.h"
#include "CmPassRTTI.h"
#include "CmMaterial.h"
#include "CmGpuParams.h"
#include "CmException.h"

namespace CamelotFramework
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

    bool Pass::isTransparent(void) const
    {
		bool transparent = false;

		if(mBlendState != nullptr)
		{
			for(UINT32 i = 0; i < CM_MAX_MULTIPLE_RENDER_TARGETS; i++)
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

	void Pass::activate(CoreAccessor& coreAccessor) const
	{
		HGpuProgram vertProgram = getVertexProgram();
		if(vertProgram)
			coreAccessor.bindGpuProgram(vertProgram);
		else
			coreAccessor.unbindGpuProgram(GPT_VERTEX_PROGRAM);

		HGpuProgram fragProgram = getFragmentProgram();
		if(fragProgram)
			coreAccessor.bindGpuProgram(fragProgram);
		else
			coreAccessor.unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		HGpuProgram geomProgram = getGeometryProgram();
		if(geomProgram)
			coreAccessor.bindGpuProgram(geomProgram);
		else
			coreAccessor.unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		HGpuProgram hullProgram = getHullProgram();
		if(hullProgram)
			coreAccessor.bindGpuProgram(hullProgram);
		else
			coreAccessor.unbindGpuProgram(GPT_HULL_PROGRAM);

		HGpuProgram domainProgram = getDomainProgram();
		if(domainProgram)
			coreAccessor.bindGpuProgram(domainProgram);
		else
			coreAccessor.unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		// TODO - Try to limit amount of state changes, if previous state is already the same (especially with textures)

		// TODO: Disable remaining texture units
		//renderSystem->_disableTextureUnitsFrom(pass->getNumTextures());

		// Set up non-texture related pass settings
		HBlendState blendState = getBlendState();
		if(blendState != nullptr)
			coreAccessor.setBlendState(blendState.getInternalPtr());
		else
			coreAccessor.setBlendState(BlendState::getDefault());

		HDepthStencilState depthStancilState = getDepthStencilState();
		if(depthStancilState != nullptr)
			coreAccessor.setDepthStencilState(depthStancilState.getInternalPtr(), getStencilRefValue());
		else
			coreAccessor.setDepthStencilState(DepthStencilState::getDefault(), getStencilRefValue());

		HRasterizerState rasterizerState = getRasterizerState();
		if(rasterizerState != nullptr)
			coreAccessor.setRasterizerState(rasterizerState.getInternalPtr());
		else
			coreAccessor.setRasterizerState(RasterizerState::getDefault());
	}

	void Pass::bindParameters(CoreAccessor& coreAccessor, const PassParametersPtr& params) const
	{
		HGpuProgram vertProgram = getVertexProgram();
		if(vertProgram)
			coreAccessor.bindGpuParams(GPT_VERTEX_PROGRAM, params->mVertParams);

		HGpuProgram fragProgram = getFragmentProgram();
		if(fragProgram)
			coreAccessor.bindGpuParams(GPT_FRAGMENT_PROGRAM, params->mFragParams);

		HGpuProgram geomProgram = getGeometryProgram();
		if(geomProgram)
			coreAccessor.bindGpuParams(GPT_GEOMETRY_PROGRAM, params->mGeomParams);

		HGpuProgram hullProgram = getHullProgram();
		if(hullProgram)
			coreAccessor.bindGpuParams(GPT_HULL_PROGRAM, params->mHullParams);

		HGpuProgram domainProgram = getDomainProgram();
		if(domainProgram)
			coreAccessor.bindGpuParams(GPT_DOMAIN_PROGRAM, params->mDomainParams);

		HGpuProgram computeProgram = getComputeProgram();
		if(computeProgram)
			coreAccessor.bindGpuParams(GPT_COMPUTE_PROGRAM, params->mComputeParams);
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