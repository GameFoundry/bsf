#pragma once

#include "CmPrerequisites.h"
#include "CmCommon.h"
#include "CmSamplerState.h"
#include "CmGpuProgram.h"
#include "CmColor.h"

namespace CamelotEngine
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

		/** @copydoc RenderSystem::setWaitForVerticalBlank() */
		void setWaitForVerticalBlank(bool enabled);
		/** @copydoc RenderSystem::getWaitForVerticalBlank() */
		bool getWaitForVerticalBlank(void) const;

		/** @copydoc RenderSystem::disableTextureUnit() */
		void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/** @copydoc RenderSystem::setPointParameters() */
		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

		/** @copydoc RenderSystem::setTexture() */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);

		/** @copydoc RenderSystem::setSamplerState() */
		void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerState& samplerState);

		/** @copydoc RenderSystem::setBlendState() */
		void setBlendState(const BlendState& blendState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setRasterizerState(const RasterizerState& rasterizerState);

		/** @copydoc RenderSystem::setRasterizerState() */
		void setDepthStencilState(const DepthStencilState& depthStencilState);

		/** @copydoc RenderSystem::setStencilRefValue() */
		void setStencilRefValue(UINT32 refValue);

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(const Viewport& vp);

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
		void setRenderTarget(RenderTarget *target);

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(GpuProgramHandle prg);
		/** @copydoc RenderSystem::bindGpuProgramParameters() */
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);
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

		/** @copydoc RenderSystem::swapAllRenderTargetBuffers() */
		void swapAllRenderTargetBuffers(bool waitForVsync = true);
		/** @copydoc RenderSystem::clearFrameBuffer() */
		void clearFrameBuffer(unsigned int buffers, const Color& color = Color::Black, float depth = 1.0f, unsigned short stencil = 0);

		/**
		 * @brief	Makes all the currently queued commands available to the GPU. They will be executed
		 * 			as soon as the render thread is ready.
		 */
		void submitToGpu();

	private:
		CommandQueue* mCommandQueue;
		RenderSystem* mRenderSystem;

		bool mWaitForVerticalBlank;
	};
}