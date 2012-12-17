#include "CmDeferredRenderContext.h"
#include "CmCommandQueue.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	DeferredRenderContext::DeferredRenderContext(RenderSystem* rs, CM_THREAD_ID_TYPE threadId)
		:mCommandQueue(new CommandQueue(threadId))
		, mWaitForVerticalBlank(true)
		, mInvertVertexWinding(false)
		, mCullingMode(CULL_CLOCKWISE)
		, mRenderSystem(rs)
	{
		assert(mRenderSystem != nullptr);
	}

	DeferredRenderContext::~DeferredRenderContext()
	{
		delete mCommandQueue;
	}

	void DeferredRenderContext::swapAllRenderTargetBuffers(bool waitForVSync)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::swapAllRenderTargetBuffers_internal, mRenderSystem, waitForVSync));
	}

	void DeferredRenderContext::setViewport(const Viewport& vp)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setViewport_internal, mRenderSystem, vp));
	}

	void DeferredRenderContext::setTextureUnitSettings(UINT16 texUnit, const TexturePtr& tex, const SamplerState& samplerState)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureUnitSettings_internal, mRenderSystem, texUnit, tex, samplerState));
	}

	void DeferredRenderContext::setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTexture_internal, mRenderSystem, unit, enabled, texPtr));
	}

	void DeferredRenderContext::setVertexTexture(UINT16 unit, const TexturePtr& tex)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setVertexTexture_internal, mRenderSystem, unit, tex));
	}

	void DeferredRenderContext::disableTextureUnit(UINT16 texUnit)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnit_internal, mRenderSystem, texUnit));
	}

	void DeferredRenderContext::disableTextureUnitsFrom(UINT16 texUnit)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnitsFrom_internal, mRenderSystem, texUnit));
	}

	void DeferredRenderContext::setTextureFiltering(UINT16 unit, FilterOptions minFilter,
		FilterOptions magFilter, FilterOptions mipFilter)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureFiltering_internal, mRenderSystem, unit, minFilter, magFilter, mipFilter));
	}

	void DeferredRenderContext::setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureAnisotropy_internal, mRenderSystem, unit, maxAnisotropy));
	}

	void DeferredRenderContext::setTextureAddressingMode(UINT16 unit, const SamplerState::UVWAddressingMode& uvw)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureAddressingMode_internal, mRenderSystem, unit, uvw));
	}

	void DeferredRenderContext::setTextureBorderColor(UINT16 unit, const Color& color)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureBorderColor_internal, mRenderSystem, unit, color));
	}

	void DeferredRenderContext::setTextureMipmapBias(UINT16 unit, float bias)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureMipmapBias_internal, mRenderSystem, unit, bias));
	}

	void DeferredRenderContext::setPointParameters(float size, bool attenuationEnabled, 
		float constant, float linear, float quadratic, float minSize, float maxSize)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setPointParameters_internal, mRenderSystem, size, attenuationEnabled, constant, linear, quadratic, minSize, maxSize));
	}

	void DeferredRenderContext::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setSceneBlending_internal, mRenderSystem, sourceFactor, destFactor, op));
	}

	void DeferredRenderContext::setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
		SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setSeparateSceneBlending_internal, mRenderSystem, sourceFactor, destFactor, sourceFactorAlpha, destFactorAlpha, op, alphaOp));
	}

	void DeferredRenderContext::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setAlphaRejectSettings_internal, mRenderSystem, func, value, alphaToCoverage));
	}

	void DeferredRenderContext::setScissorTest(bool enabled, UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setScissorTest_internal, mRenderSystem, enabled, left, top, right, bottom));
	}

	bool DeferredRenderContext::getWaitForVerticalBlank(void) const
	{
		return mWaitForVerticalBlank;
	}

	void DeferredRenderContext::setWaitForVerticalBlank(bool enabled)
	{
		mWaitForVerticalBlank = enabled;
		mCommandQueue->queue(boost::bind(&RenderSystem::setWaitForVerticalBlank_internal, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setInvertVertexWinding(bool invert)
	{
		mInvertVertexWinding = invert;
		mCommandQueue->queue(boost::bind(&RenderSystem::setInvertVertexWinding_internal, mRenderSystem, invert));
	}

	bool DeferredRenderContext::getInvertVertexWinding(void) const
	{
		return mInvertVertexWinding;
	}

	void DeferredRenderContext::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferParams_internal, mRenderSystem, depthTest, depthWrite, depthFunction));
	}

	void DeferredRenderContext::setDepthBufferCheckEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferCheckEnabled_internal, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setDepthBufferWriteEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferWriteEnabled_internal, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setDepthBufferFunction(CompareFunction func)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferFunction_internal, mRenderSystem, func));
	}

	void DeferredRenderContext::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setColorBufferWriteEnabled_internal, mRenderSystem, red, green, blue, alpha));
	}

	void DeferredRenderContext::setDepthBias(float constantBias, float slopeScaleBias)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBias_internal, mRenderSystem, constantBias, slopeScaleBias));
	}

	void DeferredRenderContext::setPolygonMode(PolygonMode level)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setPolygonMode_internal, mRenderSystem, level));
	}

	void DeferredRenderContext::setStencilCheckEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setStencilCheckEnabled_internal, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setStencilBufferParams(CompareFunction func, UINT32 refValue, UINT32 mask, StencilOperation stencilFailOp, 
		StencilOperation depthFailOp, StencilOperation passOp, bool twoSidedOperation)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setStencilBufferParams_internal, mRenderSystem, func, refValue, mask, stencilFailOp, depthFailOp, passOp, twoSidedOperation));
	}

	void DeferredRenderContext::setCullingMode(CullingMode mode)
	{
		mCullingMode = mode;
		mCommandQueue->queue(boost::bind(&RenderSystem::setCullingMode_internal, mRenderSystem, mode));
	}

	CullingMode DeferredRenderContext::getCullingMode(void) const
	{
		return mCullingMode;
	}

	void DeferredRenderContext::addClipPlane(const Plane &p)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane_internal, mRenderSystem, p));
	}

	void DeferredRenderContext::addClipPlane(float A, float B, float C, float D)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane_internal, mRenderSystem, A, B, C, D));
	}

	void DeferredRenderContext::setClipPlanes(const PlaneList& clipPlanes)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setClipPlanes_internal, mRenderSystem, clipPlanes));
	}

	void DeferredRenderContext::resetClipPlanes()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::resetClipPlanes_internal, mRenderSystem));
	}

	void DeferredRenderContext::bindGpuProgram(GpuProgramHandle prg)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgram_internal, mRenderSystem, prg));
	}

	void DeferredRenderContext::unbindGpuProgram(GpuProgramType gptype)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::unbindGpuProgram_internal, mRenderSystem, gptype));
	}

	void DeferredRenderContext::bindGpuProgramParameters(GpuProgramType gptype, 
		GpuProgramParametersSharedPtr params, UINT16 variabilityMask)
	{
		GpuProgramParametersSharedPtr paramCopy = GpuProgramParametersSharedPtr(new GpuProgramParameters(*params));
		mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgramParameters_internal, mRenderSystem, gptype, paramCopy, variabilityMask));
	}

	void DeferredRenderContext::setRenderTarget(RenderTarget *target)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setRenderTarget_internal, mRenderSystem, target));
	}

	void DeferredRenderContext::clearFrameBuffer(unsigned int buffers, const Color& color, float depth, unsigned short stencil)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::clearFrameBuffer_internal, mRenderSystem, buffers, color, depth, stencil));
	}

	void DeferredRenderContext::beginFrame()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::beginFrame_internal, mRenderSystem));
	}

	void DeferredRenderContext::endFrame()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::endFrame_internal, mRenderSystem));
	}

	void DeferredRenderContext::render(const RenderOperation& op)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::render_internal, mRenderSystem, op));
	}

	void DeferredRenderContext::submitToGpu()
	{
		vector<CommandQueue::Command>::type* commands = mCommandQueue->flush();

		RenderSystem* rs = RenderSystemManager::getActive();
		rs->queueCommand(boost::bind(&CommandQueue::playback, mCommandQueue, commands));
	}
}