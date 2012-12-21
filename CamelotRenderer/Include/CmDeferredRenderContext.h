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
		void disableTextureUnit(UINT16 texUnit);
		/** @copydoc RenderSystem::disableTextureUnitsFrom() */
		void disableTextureUnitsFrom(UINT16 texUnit);

		/** @copydoc RenderSystem::setPointParameters() */
		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

		/** @copydoc RenderSystem::setTexture() */
		void setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr);

		/** @copydoc RenderSystem::setTextureUnitSettings() */
		void setSamplerState(UINT16 texUnit, const SamplerState& samplerState);

		/** @copydoc RenderSystem::setSceneBlending() */
		void setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op = SBO_ADD);
		/** @copydoc RenderSystem::setSeparateSceneBlending() */
		void setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
			SceneBlendFactor destFactorAlpha, SceneBlendOperation op = SBO_ADD, SceneBlendOperation alphaOp = SBO_ADD);
		/** @copydoc RenderSystem::setAlphaRejectSettings() */
		void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage);

		/** @copydoc RenderSystem::setViewport() */
		void setViewport(const Viewport& vp);

		/** @copydoc RenderSystem::setCullingMode() */
		void setCullingMode(CullingMode mode);
		/** @copydoc RenderSystem::getCullingMode() */
		CullingMode getCullingMode(void) const;

		/** @copydoc RenderSystem::setPolygonMode() */
		void setPolygonMode(PolygonMode level);

		/** @copydoc RenderSystem::setInvertVertexWinding() */
		void setInvertVertexWinding(bool invert);
		/** @copydoc RenderSystem::getInvertVertexWinding() */
		bool getInvertVertexWinding(void) const;

		/** @copydoc RenderSystem::setDepthBufferParams() */
		void setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);
		/** @copydoc RenderSystem::setDepthBufferCheckEnabled() */
		void setDepthBufferCheckEnabled(bool enabled = true);
		/** @copydoc RenderSystem::setDepthBufferWriteEnabled() */
		void setDepthBufferWriteEnabled(bool enabled = true);
		/** @copydoc RenderSystem::setDepthBufferFunction() */
		void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);
		/** @copydoc RenderSystem::setDepthBias() */
		void setDepthBias(float constantBias, float slopeScaleBias = 0.0f);

		/** @copydoc RenderSystem::setColorBufferWriteEnabled() */
		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		/** @copydoc RenderSystem::setStencilCheckEnabled() */
		void setStencilCheckEnabled(bool enabled);
		/** @copydoc RenderSystem::setStencilBufferParams() */
		void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
			StencilOperation stencilFailOp = SOP_KEEP, StencilOperation depthFailOp = SOP_KEEP, StencilOperation passOp = SOP_KEEP, 
			bool twoSidedOperation = false);

		/** @copydoc RenderSystem::setClipPlanes() */
		void setClipPlanes(const PlaneList& clipPlanes);
		/** @copydoc RenderSystem::addClipPlane(const Plane&) */
		void addClipPlane(const Plane& p);
		/** @copydoc RenderSystem::addClipPlane(float, float, float, float) */
		void addClipPlane(float A, float B, float C, float D);
		/** @copydoc RenderSystem::resetClipPlanes() */
		void resetClipPlanes();

		/** @copydoc RenderSystem::setScissorTest() */
		void setScissorTest(bool enabled, UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		/** @copydoc RenderSystem::setRenderTarget() */
		void setRenderTarget(RenderTarget *target);

		/** @copydoc RenderSystem::bindGpuProgram() */
		void bindGpuProgram(GpuProgramHandle prg);
		/** @copydoc RenderSystem::bindGpuProgramParameters() */
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);
		/** @copydoc RenderSystem::unbindGpuProgram() */
		void unbindGpuProgram(GpuProgramType gptype);

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
		bool mInvertVertexWinding;
		CullingMode mCullingMode;
	};
}