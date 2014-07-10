//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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

		/** @copydoc RenderSystem::setSamplerState() */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState);

		/** @copydoc RenderSystem::setBlendState() */
		void setBlendState(const BlendStatePtr& blendState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setRasterizerState(const RasterizerStatePtr& rasterizerState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(Viewport vp);

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
		void bindGpuProgram(HGpuProgram prg);

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

		/** @copydoc RenderSystem::render() */
		void render(const MeshBasePtr& mesh, UINT32 indexOffset = 0, UINT32 indexCount = 0, bool useIndices = true, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

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
		AsyncOp writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data, bool discardEntireBuffer = false);

		/**
		 * @copydoc RenderSystem::readSubresource()
		 *
		 * @note "data" parameter is populated with subresource data when the async operation completes. 
		 * 		 Until the async operation completes "data" is owned by the core thread and you won't
		 * 		 be able to access it.
		 */
		AsyncOp readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data);

		/**
		 * @brief	Resize the provided window to specified width and height in pixels.
		 */
		void resizeWindow(RenderWindowPtr& renderWindow, UINT32 width, UINT32 height);

		/**
		 * @brief	Move the provided window to specified screen coordinates.
		 */
		void moveWindow(RenderWindowPtr& renderWindow, INT32 left, INT32 top);

		/**
		 * @brief	Hide the provided window. (Does not destroy it, just hides it).
		 */
		void hideWindow(RenderWindowPtr& renderWindow);

		/**
		 * @brief	Shows a previously hidden window.
		 */
		void showWindow(RenderWindowPtr& renderWindow);

		/**
		 * @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32)
		 */
		void setFullscreen(RenderWindowPtr& renderWindow, UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0);

		/**
		 * @copydoc RenderWindow::setFullscreen(const VideoMode&)
		 */
		void setFullscreen(RenderWindowPtr& renderWindow, const VideoMode& mode);

		/**
		 * @copydoc RenderWindow::setWindowed
		 */
		void setWindowed(RenderWindowPtr& renderWindow, UINT32 width, UINT32 height);

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