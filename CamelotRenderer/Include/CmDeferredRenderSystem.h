#pragma once

#include "CmPrerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	// TODO Low priority - We have too many allocations for RenderCommands in this class. Maybe pool the commands somewhere?

	class CM_EXPORT DeferredRenderSystem
	{
	public:
		DeferredRenderSystem(CM_THREAD_ID_TYPE threadId);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::render()
		 */
		void render(const RenderOperation& op);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::bindGpuProgram()
		 */
		virtual void bindGpuProgram(GpuProgramPtr prg);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::bindGpuProgramParameters()
		 */
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params);
				
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::bindGpuProgramParameters()
		 */
		void setInvertVertexWinding(bool invert);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setScissorTest()
		 */
		void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, 
			size_t right = 1280, size_t bottom = 720);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setStencilBufferParams()
		 */
		void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, 
			UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
			StencilOperation stencilFailOp = SOP_KEEP, 
			StencilOperation depthFailOp = SOP_KEEP,
			StencilOperation passOp = SOP_KEEP, 
			bool twoSidedOperation = false);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setVertexDeclaration()
		 */
		void setVertexDeclaration(VertexDeclarationPtr decl);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setVertexBufferBinding()
		 */
		void setVertexBufferBinding(VertexBufferBinding* binding);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setPolygonMode()
		 */
		void setPolygonMode(PolygonMode mode);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setStencilCheckEnabled()
		 */
		void setStencilCheckEnabled(bool enabled);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setWaitForVerticalBlank()
		 */
		void setWaitForVerticalBlank(bool enabled);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureUnitSettings()
		 */
		void setTextureUnitSettings(size_t texUnit, const TexturePtr& texture, const SamplerState& samplerState);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setPointParameters()
		 */
		void setPointParameters(float size, bool attenuationEnabled, 
			float constant, float linear, float quadratic, float minSize, float maxSize);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTexture()
		 */
		void setTexture(size_t unit, bool enabled, const TexturePtr &texPtr);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setVertexTexture()
		 */
		void setVertexTexture(size_t unit, const TexturePtr& tex);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureUnitFiltering()
		 */
		void setTextureFiltering(size_t unit, FilterOptions minFilter,
			FilterOptions magFilter, FilterOptions mipFilter);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureFiltering()
		 */
		void setTextureFiltering(size_t unit, FilterType ftype, FilterOptions filter);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureAnisotropy()
		 */
		void setTextureAnisotropy(size_t unit, unsigned int maxAnisotropy);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureAddressingMode()
		 */
		void setTextureAddressingMode(size_t unit, const SamplerState::UVWAddressingMode& uvw);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureBorderColor()
		 */
		void setTextureBorderColor(size_t unit, const Color& color);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setTextureMipmapBias()
		 */
		void setTextureMipmapBias(size_t unit, float bias);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setSceneBlending()
		 */
		void setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op = SBO_ADD);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setSeparateSceneBlending()
		 */
		void setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
			SceneBlendFactor destFactorAlpha, SceneBlendOperation op = SBO_ADD, SceneBlendOperation alphaOp = SBO_ADD);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setAlphaRejectSettings()
		 */
		void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setViewport()
		 */
		void setViewport(Viewport *vp);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setCullingMode()
		 */
		void setCullingMode(CullingMode mode);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setDepthBufferParams()
		 */
		void setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setDepthBufferCheckEnabled()
		 */
		void setDepthBufferCheckEnabled(bool enabled = true);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setDepthBufferWriteEnabled()
		 */
		void setDepthBufferWriteEnabled(bool enabled = true);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setDepthBufferFunction()
		 */
		void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setDepthBias()
		 */
		void setDepthBias(float constantBias, float slopeScaleBias = 0.0f);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::setColorBufferWriteEnabled()
		 */
		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::disableTextureUnit()
		 */
		void disableTextureUnit(size_t texUnit);
		
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::disableTextureUnitsFrom()
		 */
		void disableTextureUnitsFrom(size_t texUnit);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::beginFrame()
		 */
		void beginFrame(void);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::endFrame()
		 */
		void endFrame(void);

		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::clearFrameBuffer()
		 */
		void clearFrameBuffer(unsigned int buffers, 
			const Color& colour = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0);

		/**
		 * @brief	Makes all the currently queued commands available to the GPU. They will be executed
		 * 			as soon as the render thread is ready.
		 * 			
		 * @note	This is expected to be called once per frame. If the previous set of commands hasn't even started rendering
		 * 			yet, it will be discarded. This is to prevent lag if the simulation executes faster than the render thread.
		 */
		void submitToGpu();

	private:
		// Actively being filled up
		vector<DeferredGpuCommand*>::type* mActiveRenderCommandBuffer;

		// Finalized and ready for rendering
		vector<DeferredGpuCommand*>::type* mReadyRenderCommandBuffer;

		CM_THREAD_ID_TYPE mMyThreadId;
		CM_MUTEX(mCommandBufferMutex)

		/**
		 * @brief	Plays all queued commands
		 */
		void playbackCommands();

		/**
		 * @brief	Throw an exception if the current thread is not the original
		 * 			thread the DeferredRenderSystem was started on.
		 */
		void throwIfInvalidThread();
	};
}