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

		// TODO - Add @copydoc from RenderSystem

		void setWaitForVerticalBlank(bool enabled);
		bool getWaitForVerticalBlank(void) const;

		void setTextureUnitSettings(UINT16 texUnit, const TexturePtr& texture, const SamplerState& samplerState);
		void disableTextureUnit(UINT16 texUnit);
		void disableTextureUnitsFrom(UINT16 texUnit);

		void setPointParameters(float size, bool attenuationEnabled, float constant, float linear, float quadratic, float minSize, float maxSize);

		void setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr);
		void setVertexTexture(UINT16 unit, const TexturePtr& tex);

		void setTextureFiltering(UINT16 unit, FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter);
		void setTextureFiltering(UINT16 unit, FilterType ftype, FilterOptions filter);
		void setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy);
		void setTextureAddressingMode(UINT16 unit, const SamplerState::UVWAddressingMode& uvw);
		void setTextureBorderColor(UINT16 unit, const Color& color);
		void setTextureMipmapBias(UINT16 unit, float bias);

		void setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op = SBO_ADD);
		void setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
			SceneBlendFactor destFactorAlpha, SceneBlendOperation op = SBO_ADD, SceneBlendOperation alphaOp = SBO_ADD);
		void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage);

		void setViewport(const Viewport& vp);

		void setCullingMode(CullingMode mode);
		CullingMode getCullingMode(void) const;

		void setPolygonMode(PolygonMode level);

		void setInvertVertexWinding(bool invert);
		bool getInvertVertexWinding(void) const;

		void setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);
		void setDepthBufferCheckEnabled(bool enabled = true);
		void setDepthBufferWriteEnabled(bool enabled = true);
		void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);
		void setDepthBias(float constantBias, float slopeScaleBias = 0.0f);

		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		void setStencilCheckEnabled(bool enabled);
		void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
			StencilOperation stencilFailOp = SOP_KEEP, StencilOperation depthFailOp = SOP_KEEP, StencilOperation passOp = SOP_KEEP, 
			bool twoSidedOperation = false);

		void setClipPlanes(const PlaneList& clipPlanes);
		void addClipPlane(const Plane &p);
		void addClipPlane(float A, float B, float C, float D);
		void resetClipPlanes();

		void setScissorTest(bool enabled, UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		void setRenderTarget(RenderTarget *target);

		void bindGpuProgram(GpuProgramHandle prg);
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 variabilityMask);
		void unbindGpuProgram(GpuProgramType gptype);

		void beginFrame(void);
		void endFrame(void);

		void render(const RenderOperation& op);

		void swapAllRenderTargetBuffers(bool waitForVsync = true);
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