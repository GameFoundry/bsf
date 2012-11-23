#include "CmDeferredRenderSystem.h"
#include "CmDeferredGpuCommands.h"
#include "CmRenderSystemManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	DeferredRenderSystem::DeferredRenderSystem(CM_THREAD_ID_TYPE threadId)
		:mMyThreadId(threadId)
	{
		mActiveRenderCommandBuffer = new vector<DeferredGpuCommand*>::type();
	}

	void DeferredRenderSystem::render(const RenderOperation& op)
	{
		throwIfInvalidThread();

		RenderOperation opCopy = op;

		DfrdRenderGpuCommand* newCommand = new DfrdRenderGpuCommand(opCopy);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::bindGpuProgram(GpuProgramPtr prg)
	{
		throwIfInvalidThread();

		DfrdBindGpuProgramCommand* newCommand = new DfrdBindGpuProgramCommand(prg);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params)
	{
		throwIfInvalidThread();

		// We need to copy the constant buffers in order to prevent the user from modifying the parameters before rendering is done.
		// Because of the way how GpuProgramParameters copy constructor works, only constants are copied and rest of the data is shared.
		// Shared data is data that is only modified on shader change, and that should only occur on render thread, so it won't interfere
		// with rendering.
		GpuProgramParametersSharedPtr paramCopy = GpuProgramParametersSharedPtr(new GpuProgramParameters(*params));

		DfrdBindGpuParamsCommand* newCommand = new DfrdBindGpuParamsCommand(gptype, paramCopy);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::setInvertVertexWinding(bool invert)
	{
		throwIfInvalidThread();

		DfrdInvVertWindingCommand* newCommand = new DfrdInvVertWindingCommand(invert);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setScissorTest(bool enabled, size_t left, size_t top, size_t right, size_t bottom)
	{
		throwIfInvalidThread();

		DfrdScissorTestCommand* newCommand = new DfrdScissorTestCommand(enabled, left, top, right, bottom);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setStencilBufferParams(CompareFunction func, UINT32 refValue, UINT32 mask, 
		StencilOperation stencilFailOp, StencilOperation depthFailOp, StencilOperation passOp, bool twoSidedOperation)
	{
		throwIfInvalidThread();

		DfrdStencilBufferParamsCommand* newCommand = new DfrdStencilBufferParamsCommand(func, refValue, mask, stencilFailOp, depthFailOp, passOp, twoSidedOperation);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::setVertexDeclaration(VertexDeclarationPtr decl)
	{
		throwIfInvalidThread();

		DfrdVertexDeclCommand* newCommand = new DfrdVertexDeclCommand();
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setVertexBufferBinding(VertexBufferBinding* binding)
	{
		throwIfInvalidThread();

		DfrdVertBufferBindCommand* newCommand = new DfrdVertBufferBindCommand();
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setPolygonMode(PolygonMode mode)
	{
		throwIfInvalidThread();

		DfrdPolygonModeCommand* newCommand = new DfrdPolygonModeCommand(mode);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setStencilCheckEnabled(bool enabled)
	{
		throwIfInvalidThread();

		DfrdStencilCheckCommand* newCommand = new DfrdStencilCheckCommand(enabled);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setWaitForVerticalBlank(bool enabled)
	{
		throwIfInvalidThread();

		DfrdWaitForVerticalBlankCommand* newCommand = new DfrdWaitForVerticalBlankCommand(enabled);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureUnitSettings(size_t texUnit, const TexturePtr& texture, const SamplerState& samplerState)
	{
		throwIfInvalidThread();

		DfrdTextureUnitSettingsCommand* newCommand = new DfrdTextureUnitSettingsCommand(texUnit, texture, samplerState);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setPointParameters(float size, bool attenuationEnabled, 
		float constant, float linear, float quadratic, float minSize, float maxSize)
	{
		throwIfInvalidThread();

		DfrdPointParamsCommand* newCommand = new DfrdPointParamsCommand(size, attenuationEnabled, constant, linear, quadratic, minSize, maxSize);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTexture(size_t unit, bool enabled, const TexturePtr &texPtr)
	{
		throwIfInvalidThread();

		DfrdTextureCommand* newCommand = new DfrdTextureCommand(unit, enabled, texPtr);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setVertexTexture(size_t unit, const TexturePtr& tex)
	{
		throwIfInvalidThread();

		DfrdVertexTextureCommand* newCommand = new DfrdVertexTextureCommand(unit, tex);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureFiltering(size_t unit, FilterOptions minFilter,
		FilterOptions magFilter, FilterOptions mipFilter)
	{
		throwIfInvalidThread();

		setTextureFiltering(unit, FT_MIN, minFilter);
		setTextureFiltering(unit, FT_MAG, magFilter);
		setTextureFiltering(unit, FT_MIP, mipFilter);
	}
		
	void DeferredRenderSystem::setTextureFiltering(size_t unit, FilterType ftype, FilterOptions filter)
	{
		throwIfInvalidThread();

		DfrdTextureFilteringCommand* newCommand = new DfrdTextureFilteringCommand(unit, ftype, filter);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureAnisotropy(size_t unit, unsigned int maxAnisotropy)
	{
		throwIfInvalidThread();

		DfrdTextureAnisotropyCommand* newCommand = new DfrdTextureAnisotropyCommand(unit, maxAnisotropy);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureAddressingMode(size_t unit, const SamplerState::UVWAddressingMode& uvw)
	{
		throwIfInvalidThread();

		DfrdTextureAddrModeCommand* newCommand = new DfrdTextureAddrModeCommand(unit, uvw);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureBorderColor(size_t unit, const Color& color)
	{
		throwIfInvalidThread();

		DfrdTextureBorderColorCommand* newCommand = new DfrdTextureBorderColorCommand(unit, color);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setTextureMipmapBias(size_t unit, float bias)
	{
		throwIfInvalidThread();

		DfrdTextureMipBiasCommand* newCommand = new DfrdTextureMipBiasCommand(unit, bias);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		throwIfInvalidThread();

		DfrdSceneBlendingCommand* newCommand = new DfrdSceneBlendingCommand(sourceFactor, destFactor, op);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
		SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp)
	{
		throwIfInvalidThread();

		DfrdSeparateSceneBlendingCommand* newCommand = new DfrdSeparateSceneBlendingCommand(sourceFactor, destFactor, sourceFactorAlpha, destFactorAlpha, op, alphaOp);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		throwIfInvalidThread();

		DfrdAlphaRejectParamsCommand* newCommand = new DfrdAlphaRejectParamsCommand(func, value, alphaToCoverage);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setViewport(Viewport *vp)
	{
		throwIfInvalidThread();

		DfrdViewportCommand* newCommand = new DfrdViewportCommand();
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setCullingMode(CullingMode mode)
	{
		throwIfInvalidThread();

		DfrdCullingCommand* newCommand = new DfrdCullingCommand(mode);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		throwIfInvalidThread();

		DfrdDepthBufferParamsCommand* newCommand = new DfrdDepthBufferParamsCommand(depthTest, depthWrite, depthFunction);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setDepthBufferCheckEnabled(bool enabled)
	{
		throwIfInvalidThread();

		DfrdDepthBufferCheckCommand* newCommand = new DfrdDepthBufferCheckCommand(enabled);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		throwIfInvalidThread();

		DfrdDepthBufferWriteCommand* newCommand = new DfrdDepthBufferWriteCommand(enabled);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setDepthBufferFunction(CompareFunction func)
	{
		throwIfInvalidThread();

		DfrdDepthBufferFuncCommand* newCommand = new DfrdDepthBufferFuncCommand(func);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::setDepthBias(float constantBias, float slopeScaleBias)
	{
		throwIfInvalidThread();

		DfrdDepthBiasCommand* newCommand = new DfrdDepthBiasCommand(constantBias, slopeScaleBias);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		throwIfInvalidThread();

		DfrdColorBufferWriteCommand* newCommand = new DfrdColorBufferWriteCommand(red, green, blue, alpha);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::disableTextureUnit(size_t texUnit)
	{
		throwIfInvalidThread();

		DfrdDisableTextureUnitCommand* newCommand = new DfrdDisableTextureUnitCommand(texUnit);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}
		
	void DeferredRenderSystem::disableTextureUnitsFrom(size_t texUnit)
	{
		throwIfInvalidThread();

		DfrdDisableTextureUnitsFromCommand* newCommand = new DfrdDisableTextureUnitsFromCommand(texUnit);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::beginFrame(void)
	{
		throwIfInvalidThread();

		DfrdBeginFrameCommand* newCommand = new DfrdBeginFrameCommand();
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::endFrame(void)
	{
		throwIfInvalidThread();

		DfrdEndFrameCommand* newCommand = new DfrdEndFrameCommand();
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::clearFrameBuffer(unsigned int buffers, 
		const Color& color, float depth, unsigned short stencil)
	{
		throwIfInvalidThread();

		DfrdClearFrameBufferCommand* newCommand = new DfrdClearFrameBufferCommand(buffers, color, depth, stencil);
		mActiveRenderCommandBuffer->push_back(newCommand);
	}

	void DeferredRenderSystem::submitToGpu()
	{
		throwIfInvalidThread();

		{
			CM_LOCK_MUTEX(mCommandBufferMutex)

			if(mReadyRenderCommandBuffer != nullptr)
			{
				for(auto iter = mReadyRenderCommandBuffer->begin(); iter != mReadyRenderCommandBuffer->end(); ++iter)
					delete *iter;

				delete mReadyRenderCommandBuffer;
				mReadyRenderCommandBuffer = nullptr;
			}

			mReadyRenderCommandBuffer = mActiveRenderCommandBuffer;
			mActiveRenderCommandBuffer = new vector<DeferredGpuCommand*>::type();
		}
	}

	void DeferredRenderSystem::playbackCommands()
	{
		// TODO - Throw exception if this is not called from render thread
		
		vector<DeferredGpuCommand*>::type* currentCommands = nullptr;
		{
			CM_LOCK_MUTEX(mCommandBufferMutex)

			currentCommands = mReadyRenderCommandBuffer;
			mReadyRenderCommandBuffer = nullptr;
		}

		if(currentCommands == nullptr)
			return;

		RenderSystem* rs = RenderSystemManager::getActive();

		for(auto iter = currentCommands->begin(); iter != currentCommands->end(); ++iter)
		{
			(*iter)->submitCommand(rs);
			delete *iter;
		}

		delete currentCommands;
	}

	void DeferredRenderSystem::throwIfInvalidThread()
	{
#if CM_THREAD_SUPPORT != 0
		if(CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Deferred render system method called on an invalid thread.");
		}
#endif
	}
}