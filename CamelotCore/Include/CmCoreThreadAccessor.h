#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmRenderSystem.h"
#include "CmCommandQueue.h"
#include "CmSamplerState.h"
#include "CmGpuProgram.h"
#include "CmCoreThread.h"
#include "CmAsyncOp.h"
#include "CmColor.h"

namespace CamelotFramework
{
	/**
	 * @brief	Core thread accessor allows you to schedule core commands outside of the core thread.
	 * 			
	 * @note	All commands are queued and only executed after the call to submitToCoreThread, in the order they were called.
	 */
	template <class CommandQueueSyncPolicy = CommandQueueNoSync>
	class CM_EXPORT CoreThreadAccessor
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId		Identifier for the thread that created the accessor.
		 */
		CoreThreadAccessor(CM_THREAD_ID_TYPE threadId)
		{
			mCommandQueue = cm_new<CommandQueue<CommandQueueSyncPolicy>>(threadId);
		}

		~CoreThreadAccessor()
		{
			cm_delete(mCommandQueue);
		}

		/** @copydoc RenderSystem::disableTextureUnit() */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::disableTextureUnit, RenderSystem::instancePtr(), gptype, texUnit));
		}

		/** @copydoc RenderSystem::setPointParameters() */
		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

		/** @copydoc RenderSystem::setTexture() */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setTexture, RenderSystem::instancePtr(), gptype, unit, enabled, texPtr));
		}

		/** @copydoc RenderSystem::setSamplerState() */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setSamplerState, RenderSystem::instancePtr(), gptype, texUnit, samplerState));
		}

		/** @copydoc RenderSystem::setBlendState() */
		void setBlendState(const BlendStatePtr& blendState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setBlendState, RenderSystem::instancePtr(), blendState));
		}

		/** @copydoc RenderSystem::setRasterizerState() */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setRasterizerState, RenderSystem::instancePtr(), rasterizerState));
		}

		/** @copydoc RenderSystem::setRasterizerState() */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setDepthStencilState, RenderSystem::instancePtr(), depthStencilState, stencilRefValue));
		}

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(const ViewportPtr& vp)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setViewport, RenderSystem::instancePtr(), vp));
		}

		/** @copydoc RenderSystem::setDrawOperation() */
		void setDrawOperation(DrawOperationType op)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setDrawOperation, RenderSystem::instancePtr(), op));
		}


		/** @copydoc RenderSystem::setClipPlanes() */
		void setClipPlanes(const PlaneList& clipPlanes)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setClipPlanes, RenderSystem::instancePtr(), clipPlanes));
		}

		/** @copydoc RenderSystem::addClipPlane(const Plane&) */
		void addClipPlane(const Plane& p)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, RenderSystem::instancePtr(), p));
		}

		/** @copydoc RenderSystem::addClipPlane(float, float, float, float) */
		void addClipPlane(float A, float B, float C, float D)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::addClipPlane, RenderSystem::instancePtr(), A, B, C, D));
		}

		/** @copydoc RenderSystem::resetClipPlanes() */
		void resetClipPlanes()
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::resetClipPlanes, RenderSystem::instancePtr()));
		}

		/** @copydoc RenderSystem::setScissorTest() */
		void setScissorTest(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setScissorRect, RenderSystem::instancePtr(), left, top, right, bottom));
		}


		/** @copydoc RenderSystem::setRenderTarget() */
		void setRenderTarget(RenderTargetPtr target)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::setRenderTarget, RenderSystem::instancePtr(), target));
		}

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(HGpuProgram prg)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuProgram, RenderSystem::instancePtr(), prg));
		}

		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::unbindGpuProgram, RenderSystem::instancePtr(), gptype));
		}

		/** @copydoc RenderSystem::bindGpuParams() */
		void bindGpuParams(GpuProgramType gptype, BindableGpuParams& params)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::bindGpuParams, RenderSystem::instancePtr(), gptype, params));
		}


		/** @copydoc RenderSystem::beginFrame() */
		void beginFrame(void)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::beginFrame, RenderSystem::instancePtr()));
		}

		/** @copydoc RenderSystem::endFrame() */
		void endFrame(void)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::endFrame, RenderSystem::instancePtr()));
		}

		/**
		 * @copydoc RenderSystem::clearRenderTarget()
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::clearRenderTarget, RenderSystem::instancePtr(), buffers, color, depth, stencil));
		}

		/**
		 * @copydoc RenderSystem::clearViewport()
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::clearViewport, RenderSystem::instancePtr(), buffers, color, depth, stencil));
		}

		/** @copydoc RenderSystem::swapBuffers() */
		void swapBuffers(RenderTargetPtr target)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::swapBuffers, RenderSystem::instancePtr(), target));
		}


		/** @copydoc RenderSystem::render() */
		void render(const MeshBasePtr& mesh, UINT32 indexOffset = 0, UINT32 indexCount = 0, bool useIndices = true, DrawOperationType drawOp = DOT_TRIANGLE_LIST)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::render, RenderSystem::instancePtr(), mesh, indexOffset, indexCount, useIndices, drawOp));
		}

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexOffset, UINT32 vertexCount)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::draw, RenderSystem::instancePtr(), vertexOffset, vertexCount));
		}

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount)
		{
			mCommandQueue->queue(boost::bind(&RenderSystem::drawIndexed, RenderSystem::instancePtr(), startIndex, indexCount, vertexOffset, vertexCount));
		}

		/**
		 * @copydoc RenderSystem::writeSubresource()
		 *
		 * @param discardEntireBuffer When true the existing contents of the resource you are updating will be discarded. This can make the
		 * 							  operation faster. Resources with certain buffer types might require this flag to be in a specific state
		 * 							  otherwise the operation will fail. 
		 *
		 * @note Resource is updated with data from "data" parameter when the async operation completes. 
		 * 		 Until the async operation completes "data" is owned by the core thread and you won't
		 * 		 be able to access it. 
		 * 		 
		 *		Normally dynamic buffers will require you to enable "discardEntireBuffer" flag, while static buffers require it disabled.
		 */
		AsyncOp writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data, bool discardEntireBuffer = false)
		{
			data->lock();

			return mCommandQueue->queueReturn(boost::bind(&RenderSystem::writeSubresource, RenderSystem::instancePtr(), resource, subresourceIdx, data, discardEntireBuffer, _1));
		}

		/**
		 * @copydoc RenderSystem::writeSubresource()
		 *
		 * @note "data" parameter is populated with subresource data when the async operation completes. 
		 * 		 Until the async operation completes "data" is owned by the core thread and you won't
		 * 		 be able to access it.
		 */
		AsyncOp readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data)
		{
			data->lock();

			return mCommandQueue->queueReturn(boost::bind(&RenderSystem::readSubresource, RenderSystem::instancePtr(), resource, subresourceIdx, data, _1));
		}

		void resizeWindow(RenderWindowPtr& renderWindow, UINT32 width, UINT32 height)
		{
			mCommandQueue->queue(boost::bind(&RenderWindow::resize, renderWindow.get(), width, height));
		}

		void moveWindow(RenderWindowPtr& renderWindow, INT32 left, INT32 top)
		{
			mCommandQueue->queue(boost::bind(&RenderWindow::move, renderWindow.get(), left, top));
		}

		void hideWindow(RenderWindowPtr& renderWindow)
		{
			mCommandQueue->queue(boost::bind(&RenderWindow::setHidden, renderWindow.get(), true));
		}

		void showWindow(RenderWindowPtr& renderWindow)
		{
			mCommandQueue->queue(boost::bind(&RenderWindow::setHidden, renderWindow.get(), false));
		}

		/**
		 * @brief	Makes all the currently queued commands available to the core thread. They will be executed
		 * 			as soon as the core thread is ready.
		 */
		void submitToCoreThread(bool blockUntilComplete = false)
		{
			Queue<QueuedCommand>::type* commands = mCommandQueue->flush();

			gCoreThread().queueCommand(boost::bind(&CommandQueueBase::playback, mCommandQueue, commands), blockUntilComplete);
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
	};
}