#pragma once

#include "CmPrerequisites.h"
#include "CmDeferredRenderContextFwd.h"
#include "CmCommonEnums.h"
#include "CmRenderSystem.h"
#include "CmCommandQueue.h"
#include "CmSamplerState.h"
#include "CmGpuProgram.h"
#include "CmColor.h"

namespace CamelotFramework
{
	/**
	 * @brief	Deferred render context allows you to execute RenderSystem commands outside of the render thread.
	 * 			
	 * @note	All commands are queued and only executed after the call to submitToGpu, in the order they were called.
	 */
	template <class CommandQueueSyncPolicy = CommandQueueNoSync>
	class CM_EXPORT DeferredRenderContext
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	rs  			Render system to be used by the context.
		 * @param	threadId		Identifier for the thread that created the context.
		 * @param	syncedAccess	If false, the deferred render context can only be safely accessed from the thread that created it.
		 * 							If true, deferred render context can be accessed from any thread safely, however there will be a slight performance
		 * 							hit due to synchronization. In most cases you will want not to use synced access and instead create a separate context
		 * 							for specific threads.
		 */
		DeferredRenderContext(RenderSystem* rs, CM_THREAD_ID_TYPE threadId)
			:mRenderSystem(rs)
		{
			assert(mRenderSystem != nullptr);

			mCommandQueue = cm_new<CommandQueue<CommandQueueSyncPolicy>>(threadId);
		}

		~DeferredRenderContext()
		{
			cm_delete(mCommandQueue);
		}

		/** @copydoc RenderSystem::disableTextureUnit() */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnit, mRenderSystem, gptype, texUnit));
		}

		/** @copydoc RenderSystem::setPointParameters() */
		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

		/** @copydoc RenderSystem::setTexture() */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setTexture, mRenderSystem, gptype, unit, enabled, texPtr));
		}

		/** @copydoc RenderSystem::setSamplerState() */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setSamplerState, mRenderSystem, gptype, texUnit, samplerState));
		}

		/** @copydoc RenderSystem::setBlendState() */
		void setBlendState(const BlendStatePtr& blendState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setBlendState, mRenderSystem, blendState));
		}

		/** @copydoc RenderSystem::setRasterizerState() */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setRasterizerState, mRenderSystem, rasterizerState));
		}

		/** @copydoc RenderSystem::setRasterizerState() */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setDepthStencilState, mRenderSystem, depthStencilState, stencilRefValue));
		}

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(ViewportPtr& vp)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setViewport, mRenderSystem, vp));
		}

		/** @copydoc RenderSystem::setVertexBuffer() */
		void setVertexBuffer(UINT32 index, const VertexBufferPtr& buffer)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setVertexBuffer, mRenderSystem, index, buffer));
		}

		/** @copydoc RenderSystem::setIndexBuffer() */
		void setIndexBuffer(const IndexBufferPtr& buffer)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setIndexBuffer, mRenderSystem, buffer));
		}

		/** @copydoc RenderSystem::setVertexDeclaration() */
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setVertexDeclaration, mRenderSystem, vertexDeclaration));
		}

		/** @copydoc RenderSystem::setDrawOperation() */
		void setDrawOperation(DrawOperationType op)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setDrawOperation, mRenderSystem, op));
		}


		/** @copydoc RenderSystem::setClipPlanes() */
		void setClipPlanes(const PlaneList& clipPlanes)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setClipPlanes, mRenderSystem, clipPlanes));
		}

		/** @copydoc RenderSystem::addClipPlane(const Plane&) */
		void addClipPlane(const Plane& p)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, mRenderSystem, p));
		}

		/** @copydoc RenderSystem::addClipPlane(float, float, float, float) */
		void addClipPlane(float A, float B, float C, float D)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, mRenderSystem, A, B, C, D));
		}

		/** @copydoc RenderSystem::resetClipPlanes() */
		void resetClipPlanes()
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::resetClipPlanes, mRenderSystem));
		}

		/** @copydoc RenderSystem::setScissorTest() */
		void setScissorTest(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setScissorRect, mRenderSystem, left, top, right, bottom));
		}


		/** @copydoc RenderSystem::setRenderTarget() */
		void setRenderTarget(RenderTargetPtr target)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setRenderTarget, mRenderSystem, target));
		}

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(HGpuProgram prg)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgram, mRenderSystem, prg));
		}

		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::unbindGpuProgram, mRenderSystem, gptype));
		}

		/** @copydoc RenderSystem::bindGpuParams() */
		void bindGpuParams(GpuProgramType gptype, BindableGpuParams& params)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuParams, mRenderSystem, gptype, params));
		}


		/** @copydoc RenderSystem::beginFrame() */
		void beginFrame(void)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::beginFrame, mRenderSystem));
		}

		/** @copydoc RenderSystem::endFrame() */
		void endFrame(void)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::endFrame, mRenderSystem));
		}

		/** @copydoc RenderSystem::clear() */
		void clear(RenderTargetPtr target, unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::clear, mRenderSystem, target, buffers, color, depth, stencil));
		}

		/** @copydoc RenderSystem::swapBuffers() */
		void swapBuffers(RenderTargetPtr target)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::swapBuffers, mRenderSystem, target));
		}


		/** @copydoc RenderSystem::render() */
		void render(const RenderOperation& op)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::render, mRenderSystem, op));
		}

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexCount)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::draw, mRenderSystem, vertexCount));
		}

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexCount)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::drawIndexed, mRenderSystem, startIndex, indexCount, vertexCount));
		}


		/**
		 * @copydoc RenderSystem::writeSubresource()
		 *
		 * @note Resource is updated with data from "data" parameter when the async operation completes. 
		 * 		 Until the async operation completes "data" is owned by the render thread and you won't
		 * 		 be able to access it. 
		 */
		AsyncOp writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceData& data)
		{
			data.lock();

			return mCommandQueue->queueReturn(boost::bind(&RenderSystem::writeSubresource, mRenderSystem, resource, subresourceIdx, boost::cref(data), _1));
		}

		/**
		 * @copydoc RenderSystem::writeSubresource()
		 *
		 * @note "data" parameter is populated with subresource data when the async operation completes. 
		 * 		 Until the async operation completes "data" is owned by the render thread and you won't
		 * 		 be able to access it.
		 */
		AsyncOp readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, GpuResourceData& data)
		{
			data.lock();

			return mCommandQueue->queueReturn(boost::bind(&RenderSystem::readSubresource, mRenderSystem, resource, subresourceIdx, boost::ref(data), _1));
		}

		/**
		 * @brief	Makes all the currently queued commands available to the GPU. They will be executed
		 * 			as soon as the render thread is ready.
		 */
		void submitToGpu(bool blockUntilComplete = false)
		{
			Queue<QueuedCommand>::type* commands = mCommandQueue->flush();

			RenderSystem* rs = RenderSystem::instancePtr();
			rs->queueCommand(boost::bind(&CommandQueueBase::playback, mCommandQueue, commands), blockUntilComplete);
		}

		/**
		 * @brief	Cancels all commands in the queue.
		 */
		void cancelAll()
		{
			mCommandQueue->cancelAll();
		}

	private:
		CommandQueue<CommandQueueSyncPolicy>* mCommandQueue;
		RenderSystem* mRenderSystem;
	};
}