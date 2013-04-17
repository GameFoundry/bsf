#include "CmPass.h"
#include "CmRasterizerState.h"
#include "CmBlendState.h"
#include "CmDepthStencilState.h"
#include "CmPassRTTI.h"
#include "CmDeferredRenderContext.h"
#include "CmMaterial.h"
#include "CmException.h"

namespace CamelotFramework
{
    //-----------------------------------------------------------------------------
	Pass::Pass()
		:mStencilRefValue(0)
    { }
    //-----------------------------------------------------------------------------
	Pass::Pass(const Pass& oth)
    {
        *this = oth;
    }
    //-----------------------------------------------------------------------------
    Pass::~Pass()
    {
	
    }
    //-----------------------------------------------------------------------------
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
    //-----------------------------------------------------------------------
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
	//----------------------------------------------------------------------
	void Pass::setBlendState(HBlendState& blendState)
	{
		mBlendState = blendState;
	}
	//----------------------------------------------------------------------
	HBlendState Pass::getBlendState() const
	{
		return mBlendState;
	}
	//----------------------------------------------------------------------
	void Pass::setRasterizerState(HRasterizerState& rasterizerState)
	{
		mRasterizerState = rasterizerState;
	}
	//----------------------------------------------------------------------
	HRasterizerState Pass::getRasterizerState() const
	{
		return mRasterizerState;
	}
	//-----------------------------------------------------------------------
	void Pass::setDepthStencilState(HDepthStencilState& depthStencilState)
	{
		mDepthStencilState = depthStencilState;
	}
	//-----------------------------------------------------------------------
	HDepthStencilState Pass::getDepthStencilState() const
	{
		return mDepthStencilState;
	}
	//----------------------------------------------------------------------
	void Pass::setStencilRefValue(UINT32 refValue)
	{
		mStencilRefValue = refValue;
	}
	//----------------------------------------------------------------------
	UINT32 Pass::getStencilRefValue() const
	{
		return mStencilRefValue;
	}
	//----------------------------------------------------------------------
	void Pass::activate(DeferredRenderContextPtr& renderContext) const
	{
		HGpuProgram vertProgram = getVertexProgram();
		if(vertProgram)
			renderContext->bindGpuProgram(vertProgram);
		else
			renderContext->unbindGpuProgram(GPT_VERTEX_PROGRAM);

		HGpuProgram fragProgram = getFragmentProgram();
		if(fragProgram)
			renderContext->bindGpuProgram(fragProgram);
		else
			renderContext->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		HGpuProgram geomProgram = getGeometryProgram();
		if(geomProgram)
			renderContext->bindGpuProgram(geomProgram);
		else
			renderContext->unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		HGpuProgram hullProgram = getHullProgram();
		if(hullProgram)
			renderContext->bindGpuProgram(hullProgram);
		else
			renderContext->unbindGpuProgram(GPT_HULL_PROGRAM);

		HGpuProgram domainProgram = getDomainProgram();
		if(domainProgram)
			renderContext->bindGpuProgram(domainProgram);
		else
			renderContext->unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		// TODO - Try to limit amount of state changes, if previous state is already the same (especially with textures)

		// TODO: Disable remaining texture units
		//renderSystem->_disableTextureUnitsFrom(pass->getNumTextures());

		// Set up non-texture related pass settings
		HBlendState blendState = getBlendState();
		if(blendState != nullptr)
			renderContext->setBlendState(blendState.getInternalPtr());
		else
			renderContext->setBlendState(BlendState::getDefault());

		HDepthStencilState depthStancilState = getDepthStencilState();
		if(depthStancilState != nullptr)
			renderContext->setDepthStencilState(depthStancilState.getInternalPtr(), getStencilRefValue());
		else
			renderContext->setDepthStencilState(DepthStencilState::getDefault(), getStencilRefValue());

		HRasterizerState rasterizerState = getRasterizerState();
		if(rasterizerState != nullptr)
			renderContext->setRasterizerState(rasterizerState.getInternalPtr());
		else
			renderContext->setRasterizerState(RasterizerState::getDefault());
	}
	//----------------------------------------------------------------------
	void Pass::bindParameters(DeferredRenderContextPtr& renderContext, const PassParametersPtr& params) const
	{
		HGpuProgram vertProgram = getVertexProgram();
		if(vertProgram)
			renderContext->bindGpuParams(GPT_VERTEX_PROGRAM, params->mVertParams);

		HGpuProgram fragProgram = getFragmentProgram();
		if(fragProgram)
			renderContext->bindGpuParams(GPT_FRAGMENT_PROGRAM, params->mFragParams);

		HGpuProgram geomProgram = getGeometryProgram();
		if(geomProgram)
			renderContext->bindGpuParams(GPT_GEOMETRY_PROGRAM, params->mGeomParams);

		HGpuProgram hullProgram = getHullProgram();
		if(hullProgram)
			renderContext->bindGpuParams(GPT_HULL_PROGRAM, params->mHullParams);

		HGpuProgram domainProgram = getDomainProgram();
		if(domainProgram)
			renderContext->bindGpuParams(GPT_DOMAIN_PROGRAM, params->mDomainParams);

		HGpuProgram computeProgram = getComputeProgram();
		if(computeProgram)
			renderContext->bindGpuParams(GPT_COMPUTE_PROGRAM, params->mComputeParams);
	}
	//----------------------------------------------------------------------
	RTTITypeBase* Pass::getRTTIStatic()
	{
		return PassRTTI::instance();
	}
	//----------------------------------------------------------------------
	RTTITypeBase* Pass::getRTTI() const
	{
		return Pass::getRTTIStatic();
	}
}