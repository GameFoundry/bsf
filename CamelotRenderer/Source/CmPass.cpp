#include "CmPass.h"
#include "CmPassRTTI.h"
#include "CmException.h"

namespace CamelotEngine
{
    //-----------------------------------------------------------------------------
	Pass::Pass()
		: mSourceBlendFactor(SBF_ONE)
		, mDestBlendFactor(SBF_ZERO)
		, mSourceBlendFactorAlpha(SBF_ONE)
		, mDestBlendFactorAlpha(SBF_ZERO)
		, mSeparateBlend(false)
		, mBlendOperation(SBO_ADD)
		, mAlphaBlendOperation(SBO_ADD)
		, mSeparateBlendOperation(false)
		, mDepthCheck(true)
		, mDepthWrite(true)
		, mDepthFunc(CMPF_LESS_EQUAL)
		, mDepthBiasConstant(0.0f)
		, mDepthBiasSlopeScale(0.0f)
		, mDepthBiasPerIteration(0.0f)
		, mColourWrite(true)
		, mAlphaRejectFunc(CMPF_ALWAYS_PASS)
		, mAlphaRejectVal(0)
		, mAlphaToCoverageEnabled(false)
		, mCullMode(CULL_CLOCKWISE)
		, mPolygonMode(PM_SOLID)
		, mPassIterationCount(1)
		, mPointSize(1.0f)
		, mPointMinSize(0.0f)
		, mPointMaxSize(0.0f)	
    {

   }

    //-----------------------------------------------------------------------------
	Pass::Pass(const Pass& oth)
        :mPassIterationCount(1)
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
	    mSourceBlendFactor = oth.mSourceBlendFactor;
	    mDestBlendFactor = oth.mDestBlendFactor;
		mSourceBlendFactorAlpha = oth.mSourceBlendFactorAlpha;
		mDestBlendFactorAlpha = oth.mDestBlendFactorAlpha;
		mSeparateBlend = oth.mSeparateBlend;

		mBlendOperation = oth.mBlendOperation;
		mAlphaBlendOperation = oth.mAlphaBlendOperation;
		mSeparateBlendOperation = oth.mSeparateBlendOperation;

	    mDepthCheck = oth.mDepthCheck;
	    mDepthWrite = oth.mDepthWrite;
		mAlphaRejectFunc = oth.mAlphaRejectFunc;
		mAlphaRejectVal = oth.mAlphaRejectVal;
		mAlphaToCoverageEnabled = oth.mAlphaToCoverageEnabled;
        mColourWrite = oth.mColourWrite;
	    mDepthFunc = oth.mDepthFunc;
        mDepthBiasConstant = oth.mDepthBiasConstant;
		mDepthBiasSlopeScale = oth.mDepthBiasSlopeScale;
		mDepthBiasPerIteration = oth.mDepthBiasPerIteration;
	    mCullMode = oth.mCullMode;
		mPolygonMode = oth.mPolygonMode;
        mPassIterationCount = oth.mPassIterationCount;
		mPointSize = oth.mPointSize;
		mPointMinSize = oth.mPointMinSize;
		mPointMaxSize = oth.mPointMaxSize;

		mVertexProgram = oth.mVertexProgram;
		mFragmentProgram = oth.mFragmentProgram;
		mGeometryProgram = oth.mGeometryProgram;

		return *this;
    }
    //-----------------------------------------------------------------------
    void Pass::setPointSize(float ps)
    {
	    mPointSize = ps;
    }
    //-----------------------------------------------------------------------
	void Pass::setPointMinSize(float min)
	{
		mPointMinSize = min;
	}
    //-----------------------------------------------------------------------
	float Pass::getPointMinSize(void) const
	{
		return mPointMinSize;
	}
    //-----------------------------------------------------------------------
	void Pass::setPointMaxSize(float max)
	{
		mPointMaxSize = max;
	}
    //-----------------------------------------------------------------------
	float Pass::getPointMaxSize(void) const
	{
		return mPointMaxSize;
	}
    //-----------------------------------------------------------------------
    float Pass::getPointSize(void) const
    {
	    return mPointSize;
    }
    //-----------------------------------------------------------------------
    void Pass::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor)
    {
	    mSourceBlendFactor = sourceFactor;
	    mDestBlendFactor = destFactor;

		mSeparateBlend = false;
    }
	//-----------------------------------------------------------------------
	void Pass::setSeparateSceneBlending( const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor, const SceneBlendFactor sourceFactorAlpha, const SceneBlendFactor destFactorAlpha )
	{
		mSourceBlendFactor = sourceFactor;
		mDestBlendFactor = destFactor;
		mSourceBlendFactorAlpha = sourceFactorAlpha;
		mDestBlendFactorAlpha = destFactorAlpha;

		mSeparateBlend = true;
	}
    //-----------------------------------------------------------------------
    SceneBlendFactor Pass::getSourceBlendFactor(void) const
    {
	    return mSourceBlendFactor;
    }
    //-----------------------------------------------------------------------
    SceneBlendFactor Pass::getDestBlendFactor(void) const
    {
	    return mDestBlendFactor;
    }
    //-----------------------------------------------------------------------
    SceneBlendFactor Pass::getSourceBlendFactorAlpha(void) const
    {
	    return mSourceBlendFactorAlpha;
    }
    //-----------------------------------------------------------------------
    SceneBlendFactor Pass::getDestBlendFactorAlpha(void) const
    {
	    return mDestBlendFactorAlpha;
    }
	//-----------------------------------------------------------------------
	bool Pass::hasSeparateSceneBlending() const
	{
		return mSeparateBlend;
	}
	//-----------------------------------------------------------------------
	void Pass::setSceneBlendingOperation(SceneBlendOperation op)
	{
		mBlendOperation = op;
		mSeparateBlendOperation = false;
	}
	//-----------------------------------------------------------------------
	void Pass::setSeparateSceneBlendingOperation(SceneBlendOperation op, SceneBlendOperation alphaOp)
	{
		mBlendOperation = op;
		mAlphaBlendOperation = alphaOp;
		mSeparateBlendOperation = true;
	}
	//-----------------------------------------------------------------------
	SceneBlendOperation Pass::getSceneBlendingOperation() const
	{
		return mBlendOperation;
	}
	//-----------------------------------------------------------------------
	SceneBlendOperation Pass::getSceneBlendingOperationAlpha() const
	{
		return mAlphaBlendOperation;
	}
	//-----------------------------------------------------------------------
	bool Pass::hasSeparateSceneBlendingOperations() const
	{
		return mSeparateBlendOperation;
	}
    //-----------------------------------------------------------------------
    bool Pass::isTransparent(void) const
    {
		// Transparent if any of the destination colour is taken into account
		if (mDestBlendFactor == SBF_ZERO &&
			mSourceBlendFactor != SBF_DEST_COLOUR &&
			mSourceBlendFactor != SBF_ONE_MINUS_DEST_COLOUR &&
			mSourceBlendFactor != SBF_DEST_ALPHA &&
			mSourceBlendFactor != SBF_ONE_MINUS_DEST_ALPHA)
		{
		    return false;
		}
	    else
		{
		    return true;
		}
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
	void Pass::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		mAlphaRejectFunc = func;
		mAlphaRejectVal = value;
		mAlphaToCoverageEnabled = alphaToCoverage;
	}
	//-----------------------------------------------------------------------
	void Pass::setAlphaRejectFunction(CompareFunction func)
	{
		mAlphaRejectFunc = func;
	}
	//-----------------------------------------------------------------------
	void Pass::setAlphaRejectValue(unsigned char val)
	{
		mAlphaRejectVal = val;
	}
	//---------------------------------------------------------------------
	void Pass::setAlphaToCoverageEnabled(bool enabled)
	{
		mAlphaToCoverageEnabled = enabled;
	}
    //-----------------------------------------------------------------------
	void Pass::setColourWriteEnabled(bool enabled)
	{
		mColourWrite = enabled;
	}
    //-----------------------------------------------------------------------
	bool Pass::getColourWriteEnabled(void) const
	{
		return mColourWrite;
	}
    //-----------------------------------------------------------------------
    void Pass::setCullingMode( CullingMode mode)
    {
	    mCullMode = mode;
    }
    //-----------------------------------------------------------------------
    CullingMode Pass::getCullingMode(void) const
    {
	    return mCullMode;
    }
	//-----------------------------------------------------------------------
	void Pass::setPolygonMode(PolygonMode mode)
	{
		mPolygonMode = mode;
	}
	//-----------------------------------------------------------------------
	PolygonMode Pass::getPolygonMode(void) const
	{
		return mPolygonMode;
	}
    //-----------------------------------------------------------------------
    void Pass::setDepthBias(float constantBias, float slopeScaleBias)
    {
       mDepthBiasConstant = constantBias;
	   mDepthBiasSlopeScale = slopeScaleBias;
    }
    //-----------------------------------------------------------------------
    float Pass::getDepthBiasConstant(void) const
    {
        return mDepthBiasConstant;
    }
	//-----------------------------------------------------------------------
	float Pass::getDepthBiasSlopeScale(void) const
	{
		return mDepthBiasSlopeScale;
	}
	//---------------------------------------------------------------------
	void Pass::setIterationDepthBias(float biasPerIteration)
	{
		mDepthBiasPerIteration = biasPerIteration;
	}
	//---------------------------------------------------------------------
	float Pass::getIterationDepthBias() const
	{
		return mDepthBiasPerIteration;
	}
    //-----------------------------------------------------------------------
	void Pass::setVertexProgram(GpuProgramRef gpuProgram)
	{
		mVertexProgram = gpuProgram;
	}
    //-----------------------------------------------------------------------
	void Pass::setFragmentProgram(GpuProgramRef gpuProgram)
	{
		mFragmentProgram = gpuProgram;
	}
	//-----------------------------------------------------------------------
	void Pass::setGeometryProgram(GpuProgramRef gpuProgram)
	{
		mGeometryProgram = gpuProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramRef& Pass::getVertexProgram(void) const
	{
		return mVertexProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramRef& Pass::getFragmentProgram(void) const
	{
		return mFragmentProgram;
	}
	//-----------------------------------------------------------------------
	const GpuProgramRef& Pass::getGeometryProgram(void) const
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