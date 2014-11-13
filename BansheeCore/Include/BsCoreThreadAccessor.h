#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderSystem.h"
#include "BsCommandQueue.h"
#include "BsAsyncOp.h"
#include "BsViewport.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains some base functionality used for CoreThreadAccessor.
	 * 			
	 * @see		CoreThreadAccesor
	 */
	class BS_CORE_EXPORT CoreThreadAccessorBase
	{
	public:
		CoreThreadAccessorBase(CommandQueueBase* commandQueue);
		virtual ~CoreThreadAccessorBase();

		/** @copydoc RenderSystem::disableTextureUnit() */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/** @copydoc RenderSystem::setTexture() */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);

		/** @copydoc RenderSystem::setLoadStoreTexture() */
		void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr& texPtr,
			const TextureSurface& surface);

		/** @copydoc RenderSystem::setSamplerState() */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState);

		/** @copydoc RenderSystem::setBlendState() */
		void setBlendState(const BlendStatePtr& blendState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		/** @copydoc RenderSystem::setVertexBuffers() */
		void setVertexBuffers(UINT32 index, const Vector<VertexBufferPtr>& buffers);

		/** @copydoc RenderSystem::setIndexBuffer() */
		void setIndexBuffer(const IndexBufferPtr& buffer);

		/** @copydoc RenderSystem::setVertexDeclaration() */
		void setVertexDeclaration(const VertexDeclarationPtr& vertexDeclaration);

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(const Rect2& vp);

		/** @copydoc RenderSystem::setDrawOperation() */
		void setDrawOperation(DrawOperationType op);

		/** @copydoc RenderSystem::setClipPlanes() */
		void setClipPlanes(const PlaneList& clipPlanes);

		/** @copydoc RenderSystem::addClipPlane(const Plane&) */
		void addClipPlane(const Plane& p);

		/** @copydoc RenderSystem::resetClipPlanes() */
		void resetClipPlanes();

		/** @copydoc RenderSystem::setScissorTest() */
		void setScissorTest(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		/** @copydoc RenderSystem::setRenderTarget() */
		void setRenderTarget(RenderTargetPtr target);

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(const GpuProgramPtr& prg);

		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype);

		/** @copydoc RenderSystem::bindGpuParams() */
		void bindGpuParams(GpuProgramType gptype, const GpuParamsPtr& params);

		/** @copydoc RenderSystem::beginFrame() */
		void beginRender();

		/** @copydoc RenderSystem::endFrame() */
		void endRender();

		/** @copydoc RenderSystem::clearRenderTarget() */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/** @copydoc RenderSystem::clearViewport() */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0);

		/** @copydoc RenderSystem::swapBuffers() */
		void swapBuffers(RenderTargetPtr target);

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/**
		* @brief	Queues a new generic command that will be added to the command queue.
		*/
		AsyncOp queueReturnCommand(std::function<void(AsyncOp&)> commandCallback);

		/**
		* @brief	Queues a new generic command that will be added to the command queue.
		*/
		void queueCommand(std::function<void()> commandCallback);

		/**
		 * @brief	Makes all the currently queued commands available to the core thread. They will be executed
		 * 			as soon as the core thread is ready. All queued commands are removed from the accessor.
		 */
		void submitToCoreThread(bool blockUntilComplete = false);

		/**
		 * @brief	Cancels all commands in the queue.
		 */
		void cancelAll();

	private:
		CommandQueueBase* mCommandQueue;
	};

	/**
	 * @brief	Core thread accessor allows you to schedule core commands outside of the core thread. Provides a set of common
	 * 			methods you may want to execute on the core thread, as well as a general command queuing methods.
	 * 			
	 * @note	Queued commands are only executed after the call to submitToCoreThread, in the order they were submitted.
	 */
	template <class CommandQueueSyncPolicy = CommandQueueNoSync>
	class BS_CORE_EXPORT CoreThreadAccessor : public CoreThreadAccessorBase
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId		Identifier for the thread that created the accessor.
		 */
		CoreThreadAccessor(BS_THREAD_ID_TYPE threadId)
			:CoreThreadAccessorBase(bs_new<CommandQueue<CommandQueueSyncPolicy>>(threadId))
		{

		}
	};
}