#include "CmPass.h"
#include "CmPassRTTI.h"
#include "CmException.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
    //-----------------------------------------------------------------------------
	Pass::Pass()
		: mDepthCheck(true)
		, mDepthWrite(true)
		, mDepthFunc(CMPF_LESS_EQUAL)
		, mDepthBiasConstant(0.0f)
		, mDepthBiasSlopeScale(0.0f)
		, mDepthBiasPerIteration(0.0f)
		, mCullMode(CULL_CLOCKWISE)
		, mPolygonMode(PM_SOLID)
    {

   }

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

	    mDepthCheck = oth.mDepthCheck;
	    mDepthWrite = oth.mDepthWrite;
	    mDepthFunc = oth.mDepthFunc;
        mDepthBiasConstant = oth.mDepthBiasConstant;
		mDepthBiasSlopeScale = oth.mDepthBiasSlopeScale;
		mDepthBiasPerIteration = oth.mDepthBiasPerIteration;
	    mCullMode = oth.mCullMode;
		mPolygonMode = oth.mPolygonMode;

		mVertexProgram = oth.mVertexProgram;
		mFragmentProgram = oth.mFragmentProgram;
		mGeometryProgram = oth.mGeometryProgram;

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
				if (mBlendState->getDstBlend(i) != SBF_ZERO ||
					mBlendState->getSrcBlend(i) == SBF_DEST_COLOUR ||
					mBlendState->getSrcBlend(i) == SBF_ONE_MINUS_DEST_COLOUR ||
					mBlendState->getSrcBlend(i) == SBF_DEST_ALPHA ||
					mBlendState->getSrcBlend(i) == SBF_ONE_MINUS_DEST_ALPHA)
				{
					transparent = true;
				}
			}
		}

		return transparent;
    }
	//----------------------------------------------------------------------
	void Pass::setBlendState(BlendStatePtr blendState)
	{
		mBlendState = blendState;
	}
	//----------------------------------------------------------------------
	BlendStatePtr Pass::getBlendState() const
	{
		return mBlendState;
	}
	//----------------------------------------------------------------------
	void Pass::setRasterizerState(RasterizerStatePtr rasterizerState)
	{
		mRasterizerState = rasterizerState;
	}
	//----------------------------------------------------------------------
	RasterizerStatePtr Pass::getRasterizerState() const
	{
		return mRasterizerState;
	}
    //-----------------------------------------------------------------------
    void Pass::setDepthCheckEnabled(bool enabled)
    {
	    mDepthCheck = enabled;
    }
    //-----------------------------------------------------------------------
    bool Pass::getDepthCheckEnabled(void) const
    {
	    return mDepthCheck;
    }
    //-----------------------------------------------------------------------
    void Pass::setDepthWriteEnabled(bool enabled)
    {
	    mDepthWrite = enabled;
    }
    //-----------------------------------------------------------------------
    bool Pass::getDepthWriteEnabled(void) const
    {
	    return mDepthWrite;
    }
    //-----------------------------------------------------------------------
    void Pass::setDepthFunction( CompareFunction func)
    {
	    mDepthFunc = func;
    }
    //-----------------------------------------------------------------------
    CompareFunction Pass::getDepthFunction(void) const
    {
	    return mDepthFunc;
    }
    //-----------------------------------------------------------------------
	void Pass::setVertexProgram(GpuProgramHandle gpuProgram)
	{
		mVertexProgram = gpuProgram;
	}
    //-----------------------------------------------------------------------
	void Pass::setFragmentProgram(GpuProgramHandle gpuProgram)
	{
		mFragmentProgram = gpuProgram;
	}
	//-----------------------------------------------------------------------
	void Pass::setGeometryProgram(GpuProgramHandle gpuProgram)
	{
		mGeometryProgram = gpuProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramHandle& Pass::getVertexProgram(void) const
	{
		return mVertexProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramHandle& Pass::getFragmentProgram(void) const
	{
		return mFragmentProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramHandle& Pass::getGeometryProgram(void) const
	{
		return mGeometryProgram;
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