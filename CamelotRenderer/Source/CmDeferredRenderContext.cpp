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
		mCommandQueue->queue(boost::bind(&RenderSystem::swapAllRenderTargetBuffers, mRenderSystem, waitForVSync));
	}

	void DeferredRenderContext::setViewport(const Viewport& vp)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setViewport, mRenderSystem, vp));
	}

	void DeferredRenderContext::setTextureUnitSettings(UINT16 texUnit, const TexturePtr& tex, const SamplerState& samplerState)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureUnitSettings, mRenderSystem, texUnit, tex, samplerState));
	}

	void DeferredRenderContext::setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTexture, mRenderSystem, unit, enabled, texPtr));
	}

	void DeferredRenderContext::setVertexTexture(UINT16 unit, const TexturePtr& tex)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setVertexTexture, mRenderSystem, unit, tex));
	}

	void DeferredRenderContext::disableTextureUnit(UINT16 texUnit)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnit, mRenderSystem, texUnit));
	}

	void DeferredRenderContext::disableTextureUnitsFrom(UINT16 texUnit)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnitsFrom, mRenderSystem, texUnit));
	}

	void DeferredRenderContext::setTextureFiltering(UINT16 unit, FilterOptions minFilter,
		FilterOptions magFilter, FilterOptions mipFilter)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureFiltering, mRenderSystem, unit, minFilter, magFilter, mipFilter));
	}

	void DeferredRenderContext::setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureAnisotropy, mRenderSystem, unit, maxAnisotropy));
	}

	void DeferredRenderContext::setTextureAddressingMode(UINT16 unit, const SamplerState::UVWAddressingMode& uvw)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureAddressingMode, mRenderSystem, unit, uvw));
	}

	void DeferredRenderContext::setTextureBorderColor(UINT16 unit, const Color& color)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureBorderColor, mRenderSystem, unit, color));
	}

	void DeferredRenderContext::setTextureMipmapBias(UINT16 unit, float bias)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setTextureMipmapBias, mRenderSystem, unit, bias));
	}

	void DeferredRenderContext::setPointParameters(float size, bool attenuationEnabled, 
		float constant, float linear, float quadratic, float minSize, float maxSize)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setPointParameters, mRenderSystem, size, attenuationEnabled, constant, linear, quadratic, minSize, maxSize));
	}

	void DeferredRenderContext::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setSceneBlending, mRenderSystem, sourceFactor, destFactor, op));
	}

	void DeferredRenderContext::setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
		SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setSeparateSceneBlending, mRenderSystem, sourceFactor, destFactor, sourceFactorAlpha, destFactorAlpha, op, alphaOp));
	}

	void DeferredRenderContext::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setAlphaRejectSettings, mRenderSystem, func, value, alphaToCoverage));
	}

	void DeferredRenderContext::setScissorTest(bool enabled, UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setScissorTest, mRenderSystem, enabled, left, top, right, bottom));
	}

	bool DeferredRenderContext::getWaitForVerticalBlank(void) const
	{
		return mWaitForVerticalBlank;
	}

	void DeferredRenderContext::setWaitForVerticalBlank(bool enabled)
	{
		mWaitForVerticalBlank = enabled;
		mCommandQueue->queue(boost::bind(&RenderSystem::setWaitForVerticalBlank, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setInvertVertexWinding(bool invert)
	{
		mInvertVertexWinding = invert;
		mCommandQueue->queue(boost::bind(&RenderSystem::setInvertVertexWinding_, mRenderSystem, invert));
	}

	bool DeferredRenderContext::getInvertVertexWinding(void) const
	{
		return mInvertVertexWinding;
	}

	void DeferredRenderContext::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferParams, mRenderSystem, depthTest, depthWrite, depthFunction));
	}

	void DeferredRenderContext::setDepthBufferCheckEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferCheckEnabled, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setDepthBufferWriteEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferWriteEnabled, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setDepthBufferFunction(CompareFunction func)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBufferFunction, mRenderSystem, func));
	}

	void DeferredRenderContext::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setColorBufferWriteEnabled, mRenderSystem, red, green, blue, alpha));
	}

	void DeferredRenderContext::setDepthBias(float constantBias, float slopeScaleBias)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setDepthBias, mRenderSystem, constantBias, slopeScaleBias));
	}

	void DeferredRenderContext::setPolygonMode(PolygonMode level)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setPolygonMode, mRenderSystem, level));
	}

	void DeferredRenderContext::setStencilCheckEnabled(bool enabled)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setStencilCheckEnabled, mRenderSystem, enabled));
	}

	void DeferredRenderContext::setStencilBufferParams(CompareFunction func, UINT32 refValue, UINT32 mask, StencilOperation stencilFailOp, 
		StencilOperation depthFailOp, StencilOperation passOp, bool twoSidedOperation)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setStencilBufferParams, mRenderSystem, func, refValue, mask, stencilFailOp, depthFailOp, passOp, twoSidedOperation));
	}

	void DeferredRenderContext::setCullingMode(CullingMode mode)
	{
		mCullingMode = mode;
		mCommandQueue->queue(boost::bind(&RenderSystem::setCullingMode, mRenderSystem, mode));
	}

	CullingMode DeferredRenderContext::getCullingMode(void) const
	{
		return mCullingMode;
	}

	void DeferredRenderContext::addClipPlane(const Plane &p)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, mRenderSystem, p));
	}

	void DeferredRenderContext::addClipPlane(float A, float B, float C, float D)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, mRenderSystem, A, B, C, D));
	}

	void DeferredRenderContext::setClipPlanes(const PlaneList& clipPlanes)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setClipPlanes, mRenderSystem, clipPlanes));
	}

	void DeferredRenderContext::resetClipPlanes()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::resetClipPlanes, mRenderSystem));
	}

	void DeferredRenderContext::bindGpuProgram(GpuProgramHandle prg)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgram, mRenderSystem, prg));
	}

	void DeferredRenderContext::unbindGpuProgram(GpuProgramType gptype)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::unbindGpuProgram, mRenderSystem, gptype));
	}

	void DeferredRenderContext::bindGpuProgramParameters(GpuProgramType gptype, 
		GpuProgramParametersSharedPtr params, UINT16 variabilityMask)
	{
		GpuProgramParametersSharedPtr paramCopy = GpuProgramParametersSharedPtr(new GpuProgramParameters(*params));
		mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgramParameters, mRenderSystem, gptype, paramCopy, variabilityMask));
	}

	void DeferredRenderContext::setRenderTarget(RenderTarget *target)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::setRenderTarget, mRenderSystem, target));
	}

	void DeferredRenderContext::clearFrameBuffer(unsigned int buffers, const Color& color, float depth, unsigned short stencil)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::clearFrameBuffer, mRenderSystem, buffers, color, depth, stencil));
	}

	void DeferredRenderContext::beginFrame()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::beginFrame, mRenderSystem));
	}

	void DeferredRenderContext::endFrame()
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::endFrame, mRenderSystem));
	}

	void DeferredRenderContext::render(const RenderOperation& op)
	{
		mCommandQueue->queue(boost::bind(&RenderSystem::render, mRenderSystem, op));
	}

	void DeferredRenderContext::submitToGpu()
	{
		vector<CommandQueue::Command>::type* commands = mCommandQueue->flush();

		RenderSystem* rs = RenderSystemManager::getActive();
		rs->queueCommand(boost::bind(&CommandQueue::playback, mCommandQueue, commands));
	}
}