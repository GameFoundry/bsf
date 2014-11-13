#include "BsCoreThreadAccessor.h"
#include "BsCommandQueue.h"
#include "BsRenderSystem.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsGpuResourceData.h"
#include "BsIndexBuffer.h"
#include "BsVertexBuffer.h"
#include "BsVideoModeInfo.h"
#include "BsGpuParams.h"
#include "BsPass.h"
#include "BsMaterial.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	CoreThreadAccessorBase::CoreThreadAccessorBase(CommandQueueBase* commandQueue)
		:mCommandQueue(commandQueue)
	{

	}

	CoreThreadAccessorBase::~CoreThreadAccessorBase()
	{
		bs_delete(mCommandQueue);
	}

	void CoreThreadAccessorBase::disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::disableTextureUnit, RenderSystem::instancePtr(), gptype, texUnit));
	}

	void CoreThreadAccessorBase::setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setTexture, RenderSystem::instancePtr(), gptype, unit, enabled, texPtr->getCore()));
	}

	void CoreThreadAccessorBase::setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr& texPtr,
		const TextureSurface& surface)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setLoadStoreTexture, RenderSystem::instancePtr(), gptype, unit, enabled, texPtr->getCore(), 
			surface));
	}

	void CoreThreadAccessorBase::setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setSamplerState, RenderSystem::instancePtr(), gptype, texUnit, samplerState->getCore()));
	}

	void CoreThreadAccessorBase::setBlendState(const BlendStatePtr& blendState)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setBlendState, RenderSystem::instancePtr(), blendState->getCore()));
	}

	void CoreThreadAccessorBase::setRasterizerState(const RasterizerStatePtr& rasterizerState)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setRasterizerState, RenderSystem::instancePtr(), rasterizerState->getCore()));
	}

	void CoreThreadAccessorBase::setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setDepthStencilState, RenderSystem::instancePtr(), depthStencilState->getCore(), stencilRefValue));
	}

	void CoreThreadAccessorBase::setVertexBuffers(UINT32 index, const Vector<VertexBufferPtr>& buffers)
	{
		Vector<SPtr<VertexBufferCore>> coreBuffers(buffers.size());
		for (UINT32 i = 0; i < (UINT32)buffers.size(); i++)
			coreBuffers[i] = buffers[i] != nullptr ? buffers[i]->getCore() : nullptr;

		std::function<void(RenderSystem*, UINT32, const Vector<SPtr<VertexBufferCore>>&)> resizeFunc =
			[](RenderSystem* rs, UINT32 idx, const Vector<SPtr<VertexBufferCore>>& _buffers)
		{
			rs->setVertexBuffers(idx, (SPtr<VertexBufferCore>*)_buffers.data(), (UINT32)_buffers.size());
		};

		mCommandQueue->queue(std::bind(resizeFunc, RenderSystem::instancePtr(), index, coreBuffers));
	}

	void CoreThreadAccessorBase::setIndexBuffer(const IndexBufferPtr& buffer)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setIndexBuffer, RenderSystem::instancePtr(), buffer->getCore()));
	}

	void CoreThreadAccessorBase::setVertexDeclaration(const VertexDeclarationPtr& vertexDeclaration)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setVertexDeclaration, RenderSystem::instancePtr(), vertexDeclaration));
	}

	void CoreThreadAccessorBase::setViewport(const Rect2& vp)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setViewport, RenderSystem::instancePtr(), vp));
	}

	void CoreThreadAccessorBase::setDrawOperation(DrawOperationType op)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setDrawOperation, RenderSystem::instancePtr(), op));
	}

	void CoreThreadAccessorBase::setClipPlanes(const PlaneList& clipPlanes)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setClipPlanes, RenderSystem::instancePtr(), clipPlanes));
	}

	void CoreThreadAccessorBase::addClipPlane(const Plane& p)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::addClipPlane, RenderSystem::instancePtr(), p));
	}

	void CoreThreadAccessorBase::resetClipPlanes()
	{
		mCommandQueue->queue(std::bind(&RenderSystem::resetClipPlanes, RenderSystem::instancePtr()));
	}

	void CoreThreadAccessorBase::setScissorTest(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setScissorRect, RenderSystem::instancePtr(), left, top, right, bottom));
	}

	void CoreThreadAccessorBase::setRenderTarget(RenderTargetPtr target)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::setRenderTarget, RenderSystem::instancePtr(), target->getCore()));
	}

	void CoreThreadAccessorBase::bindGpuProgram(const GpuProgramPtr& prg)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::bindGpuProgram, RenderSystem::instancePtr(), prg->getCore()));
	}

	void CoreThreadAccessorBase::unbindGpuProgram(GpuProgramType gptype)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::unbindGpuProgram, RenderSystem::instancePtr(), gptype));
	}

	void CoreThreadAccessorBase::bindGpuParams(GpuProgramType gptype, const GpuParamsPtr& params)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::bindGpuParams, RenderSystem::instancePtr(), gptype, params->_clone(gCoreThread().getFrameAlloc(), true)));
	}

	void CoreThreadAccessorBase::beginRender()
	{
		mCommandQueue->queue(std::bind(&RenderSystem::beginFrame, RenderSystem::instancePtr()));
	}

	void CoreThreadAccessorBase::endRender()
	{
		mCommandQueue->queue(std::bind(&RenderSystem::endFrame, RenderSystem::instancePtr()));
	}

	void CoreThreadAccessorBase::clearRenderTarget(UINT32 buffers, const Color& color, float depth, UINT16 stencil)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::clearRenderTarget, RenderSystem::instancePtr(), buffers, color, depth, stencil));
	}

	void CoreThreadAccessorBase::clearViewport(UINT32 buffers, const Color& color, float depth, UINT16 stencil)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::clearViewport, RenderSystem::instancePtr(), buffers, color, depth, stencil));
	}

	void CoreThreadAccessorBase::swapBuffers(RenderTargetPtr target)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::swapBuffers, RenderSystem::instancePtr(), target->getCore()));
	}

	void CoreThreadAccessorBase::draw(UINT32 vertexOffset, UINT32 vertexCount)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::draw, RenderSystem::instancePtr(), vertexOffset, vertexCount));
	}

	void CoreThreadAccessorBase::drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount)
	{
		mCommandQueue->queue(std::bind(&RenderSystem::drawIndexed, RenderSystem::instancePtr(), startIndex, indexCount, vertexOffset, vertexCount));
	}

	AsyncOp CoreThreadAccessorBase::queueReturnCommand(std::function<void(AsyncOp&)> commandCallback)
	{
		return mCommandQueue->queueReturn(commandCallback);
	}

	void CoreThreadAccessorBase::queueCommand(std::function<void()> commandCallback)
	{
		mCommandQueue->queue(commandCallback);
	}

	void CoreThreadAccessorBase::submitToCoreThread(bool blockUntilComplete)
	{
		Queue<QueuedCommand>* commands = mCommandQueue->flush();

		gCoreThread().queueCommand(std::bind(&CommandQueueBase::playback, mCommandQueue, commands), blockUntilComplete);
	}

	void CoreThreadAccessorBase::cancelAll()
	{
		// Note that this won't free any Frame data allocated for all the canceled commands since
		// frame data will only get cleared at frame start
		mCommandQueue->cancelAll();
	}
}