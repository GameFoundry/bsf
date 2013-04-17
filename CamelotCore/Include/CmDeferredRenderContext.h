#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmSamplerState.h"
#include "CmGpuProgram.h"
#include "CmColor.h"

namespace CamelotFramework
{
	/**
	 * @brief	Deferred render context allows you to execute RenderSystem commands outside of the render thread.
	 * 			DeferredRenderContext cannot be shared between threads. It must be created and used on the threat that created it.
	 * 			
	 * @note	All commands are queued and only executed after the call to submitToGpu, in the order they were called.
	 */
	class CM_EXPORT DeferredRenderContext
	{
	public:
		DeferredRenderContext(RenderSystem* rs, CM_THREAD_ID_TYPE threadId);
		~DeferredRenderContext();

		/** @copydoc RenderSystem::disableTextureUnit() */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/** @copydoc RenderSystem::setPointParameters() */
		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

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
		void setViewport(ViewportPtr& vp);

		/** @copydoc RenderSystem::setVertexBuffer() */
		void setVertexBuffer(UINT32 index, const VertexBufferPtr& buffer);

		/** @copydoc RenderSystem::setIndexBuffer() */
		void setIndexBuffer(const IndexBufferPtr& buffer);

		/** @copydoc RenderSystem::setVertexDeclaration() */
		void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration);

		/** @copydoc RenderSystem::setDrawOperation() */
		void setDrawOperation(DrawOperationType op);

		/** @copydoc RenderSystem::setClipPlanes() */
		void setClipPlanes(const PlaneList& clipPlanes);
		/** @copydoc RenderSystem::addClipPlane(const Plane&) */
		void addClipPlane(const Plane& p);
		/** @copydoc RenderSystem::addClipPlane(float, float, float, float) */
		void addClipPlane(float A, float B, float C, float D);
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
		void bindGpuParams(GpuProgramType gptype, GpuParamsPtr params);

		/** @copydoc RenderSystem::beginFrame() */
		void beginFrame(void);
		/** @copydoc RenderSystem::endFrame() */
		void endFrame(void);

		/** @copydoc RenderSystem::render() */
		void render(const RenderOperation& op);

		/** @copydoc RenderSystem::draw() */
		void draw(UINT32 vertexCount);

		/** @copydoc RenderSystem::drawIndexed() */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexCount);

		/** @copydoc RenderSystem::clear() */
		void clear(RenderTargetPtr target, unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		/**
		 * @brief	Makes all the currently queued commands available to the GPU. They will be executed
		 * 			as soon as the render thread is ready.
		 */
		void submitToGpu();

		/**
		 * @brief	Cancels all commands in the queue.
		 */
		void cancelAll();

	private:
		CommandQueue* mCommandQueue;
		RenderSystem* mRenderSystem;
	};
}