#include "BsGLRenderSystem.h"
#include "BsRenderSystem.h"
#include "BsGLTextureManager.h"
#include "BsGLVertexBuffer.h"
#include "BsGLIndexBuffer.h"
#include "BsGLUtil.h"
#include "BsGLSLGpuProgram.h"
#include "BsException.h"
#include "BsGLOcclusionQuery.h"
#include "BsGLContext.h"
#include "BsGLSupport.h"
#include "BsAsyncOp.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsGLRenderTexture.h"
#include "BsGLRenderWindowManager.h"
#include "BsGLSLProgramPipelineManager.h"
#include "BsGLVertexArrayObjectManager.h"
#include "BsRenderStateManager.h"
#include "BsGpuParams.h"
#include "BsGLGpuParamBlockBuffer.h"
#include "BsCoreThread.h"
#include "BsGLQueryManager.h"
#include "BsDebug.h"
#include "BsRenderStats.h"

namespace BansheeEngine 
{
	String MODULE_NAME = "BansheeGLRenderSystem.dll";

	void __stdcall openGlErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam);

	/************************************************************************/
	/* 								PUBLIC INTERFACE                   		*/
	/************************************************************************/

	GLRenderSystem::GLRenderSystem()
		: mDepthWrite(true),
		mGLSLProgramFactory(nullptr),
		mProgramPipelineManager(nullptr),
		mActivePipeline(nullptr),
		mActiveTextureUnit(0),
		mScissorTop(0), mScissorBottom(720), mScissorLeft(0), mScissorRight(1280),
		mViewportLeft(0), mViewportTop(0), mViewportWidth(0), mViewportHeight(0),
		mStencilReadMask(0xFFFFFFFF),
		mStencilWriteMask(0xFFFFFFFF),
		mStencilCompareFront(CMPF_ALWAYS_PASS),
		mStencilCompareBack(CMPF_ALWAYS_PASS),
		mStencilRefValue(0),
		mFragmentTexOffset(0),
		mVertexTexOffset(0),
		mGeometryTexOffset(0),
		mTextureTypes(nullptr),
		mNumTextureTypes(0),
		mFragmentUBOffset(0),
		mVertexUBOffset(0),
		mGeometryUBOffset(0),
		mHullUBOffset(0),
		mDomainUBOffset(0),
		mComputeUBOffset(0),
		mDrawCallInProgress(false),
		mCurrentDrawOperation(DOT_TRIANGLE_LIST),
		mViewportNorm(0.0f, 0.0f, 1.0f, 1.0f)
	{
		// Get our GLSupport
		mGLSupport = BansheeEngine::getGLSupport();

		mViewMatrix = Matrix4::IDENTITY;

		mColorWrite[0] = mColorWrite[1] = mColorWrite[2] = mColorWrite[3] = true;

		mCurrentContext = 0;
		mMainContext = 0;

		mGLInitialised = false;

		mMinFilter = FO_LINEAR;
		mMipFilter = FO_POINT;

		mProgramPipelineManager = bs_new<GLSLProgramPipelineManager>();
	}

	GLRenderSystem::~GLRenderSystem()
	{

	}

	const String& GLRenderSystem::getName() const
	{
		static String strName("GLRenderSystem");
		return strName;
	}


	const String& GLRenderSystem::getShadingLanguageName() const
	{
		static String strName("glsl");
		return strName;
	}

	void GLRenderSystem::initializePrepare()
	{
		THROW_IF_NOT_CORE_THREAD;

		mGLSupport->start();
		mVideoModeInfo = mGLSupport->getVideoModeInfo();

		RenderWindowManager::startUp<GLRenderWindowManager>(this);
		RenderWindowCoreManager::startUp<GLRenderWindowCoreManager>(this);

		RenderStateManager::startUp();
		RenderStateCoreManager::startUp();

		QueryManager::startUp<GLQueryManager>();

		RenderSystem::initializePrepare();
	}

	void GLRenderSystem::initializeFinalize(const SPtr<RenderWindowCore>& primaryWindow)
	{
		// Get the context from the window and finish initialization
		GLContext *context = nullptr;
		primaryWindow->getCustomAttribute("GLCONTEXT", &context);

		// Set main and current context
		mMainContext = context;
		mCurrentContext = mMainContext;

		// Set primary context as active
		if (mCurrentContext)
			mCurrentContext->setCurrent();

		checkForErrors();

		// Setup GLSupport
		mGLSupport->initializeExtensions();
		checkForErrors();

		Vector<BansheeEngine::String> tokens = StringUtil::split(mGLSupport->getGLVersion(), ".");

		if (!tokens.empty())
		{
			mDriverVersion.major = parseInt(tokens[0]);
			if (tokens.size() > 1)
				mDriverVersion.minor = parseInt(tokens[1]);
			if (tokens.size() > 2)
				mDriverVersion.release = parseInt(tokens[2]);
		}
		mDriverVersion.build = 0;

		mCurrentCapabilities = createRenderSystemCapabilities();
		initFromCaps(mCurrentCapabilities);
		GLVertexArrayObjectManager::startUp();

		mGLInitialised = true;

		RenderSystem::initializeFinalize(primaryWindow);
	}

	void GLRenderSystem::destroy_internal()
	{
		RenderSystem::destroy_internal();

		// Deleting the GLSL program factory
		if (mGLSLProgramFactory)
		{
			// Remove from manager safely
			GpuProgramCoreManager::instance().removeFactory(mGLSLProgramFactory);
			bs_delete(mGLSLProgramFactory);
			mGLSLProgramFactory = nullptr;
		}

		// Deleting the hardware buffer manager.  Has to be done before the mGLSupport->stop().
		HardwareBufferCoreManager::shutDown();
		HardwareBufferManager::shutDown();
		GLRTTManager::shutDown();

		mBoundVertexBuffers.clear();
		mBoundVertexDeclaration = nullptr;
		mBoundIndexBuffer = nullptr;

		mCurrentVertexProgram = nullptr;
		mCurrentFragmentProgram = nullptr;
		mCurrentGeometryProgram = nullptr;
		mCurrentHullProgram = nullptr;
		mCurrentDomainProgram = nullptr;

		mGLSupport->stop();

		TextureCoreManager::shutDown();
		TextureManager::shutDown();
		QueryManager::shutDown();
		RenderWindowCoreManager::shutDown();
		RenderWindowManager::shutDown();
		RenderStateCoreManager::shutDown();
		RenderStateManager::shutDown();
		GLVertexArrayObjectManager::shutDown(); // Note: Needs to be after QueryManager shutdown as some resources might be waiting for queries to complete

		mGLInitialised = false;

		if(mProgramPipelineManager != nullptr)
			bs_delete(mProgramPipelineManager);

		if(mGLSupport)
			bs_delete(mGLSupport);

		if(mTextureTypes != nullptr)
			bs_deleteN(mTextureTypes, mNumTextureTypes);
	}

	void GLRenderSystem::bindGpuProgram(const SPtr<GpuProgramCore>& prg)
	{
		THROW_IF_NOT_CORE_THREAD;

		SPtr<GLSLGpuProgramCore> glprg = std::static_pointer_cast<GLSLGpuProgramCore>(prg);

		switch (glprg->getProperties().getType())
		{
		case GPT_VERTEX_PROGRAM:
			mCurrentVertexProgram = glprg;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mCurrentFragmentProgram = glprg;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mCurrentGeometryProgram = glprg;
			break;
		case GPT_DOMAIN_PROGRAM:
			mCurrentDomainProgram = glprg;
			break;
		case GPT_HULL_PROGRAM:
			mCurrentHullProgram = glprg;
			break;
		}

		RenderSystem::bindGpuProgram(prg);
	}

	void GLRenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		THROW_IF_NOT_CORE_THREAD;

		setActiveProgram(gptype, nullptr);

		RenderSystem::unbindGpuProgram(gptype);
	}

	void GLRenderSystem::bindGpuParams(GpuProgramType gptype, const SPtr<GpuParamsCore>& bindableParams)
	{
		THROW_IF_NOT_CORE_THREAD;

		bindableParams->updateHardwareBuffers();
		const GpuParamDesc& paramDesc = bindableParams->getParamDesc();
		SPtr<GLSLGpuProgramCore> activeProgram = getActiveProgram(gptype);
		GLuint glProgram = activeProgram->getGLHandle();

		for(auto iter = paramDesc.textures.begin(); iter != paramDesc.textures.end(); ++iter)
		{
			SPtr<TextureCore> texture = bindableParams->getTexture(iter->second.slot);

			if (!bindableParams->isLoadStoreTexture(iter->second.slot))
			{
				if (texture == nullptr)
					setTexture(gptype, iter->second.slot, false, nullptr);
				else
					setTexture(gptype, iter->second.slot, true, texture);
			}
			else
			{
				const TextureSurface& surface = bindableParams->getLoadStoreSurface(iter->second.slot);

				if (texture == nullptr)
					setLoadStoreTexture(gptype, iter->second.slot, false, nullptr, surface);
				else
					setLoadStoreTexture(gptype, iter->second.slot, true, texture, surface);
			}
		}

		for(auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
		{
			SPtr<SamplerStateCore>& samplerState = bindableParams->getSamplerState(iter->second.slot);

			if(samplerState == nullptr)
				setSamplerState(gptype, iter->second.slot, SamplerState::getDefault()->getCore());
			else
				setSamplerState(gptype, iter->second.slot, samplerState);

			glProgramUniform1i(glProgram, iter->second.slot, getGLTextureUnit(gptype, iter->second.slot));
		}

		UINT8* uniformBufferData = nullptr;

		UINT32 blockBinding = 0;
		for(auto iter = paramDesc.paramBlocks.begin(); iter != paramDesc.paramBlocks.end(); ++iter)
		{
			SPtr<GpuParamBlockBufferCore> paramBlockBuffer = bindableParams->getParamBlockBuffer(iter->second.slot);
			if(paramBlockBuffer == nullptr)
				continue;

			if(iter->second.slot == 0)
			{
				// 0 means uniforms are not in block, in which case we handle it specially
				if (uniformBufferData == nullptr && paramBlockBuffer->getSize() > 0)
				{
					uniformBufferData = (UINT8*)bs_alloc<ScratchAlloc>(paramBlockBuffer->getSize());
					paramBlockBuffer->readFromGPU(uniformBufferData);
				}

				continue;
			}

			const GLGpuParamBlockBufferCore* glParamBlockBuffer = static_cast<const GLGpuParamBlockBufferCore*>(paramBlockBuffer.get());

			UINT32 globalBlockBinding = getGLUniformBlockBinding(gptype, blockBinding);
			glUniformBlockBinding(glProgram, iter->second.slot - 1, globalBlockBinding);
			glBindBufferRange(GL_UNIFORM_BUFFER, globalBlockBinding, glParamBlockBuffer->getGLHandle(), 0, glParamBlockBuffer->getSize());

			blockBinding++;

			BS_INC_RENDER_STAT(NumGpuParamBufferBinds);
		}

		bool hasBoundAtLeastOne = false;
		for(auto iter = paramDesc.params.begin(); iter != paramDesc.params.end(); ++iter)
		{
			const GpuParamDataDesc& paramDesc = iter->second;

			if(paramDesc.paramBlockSlot != 0) // 0 means uniforms are not in a block
				continue;

			const UINT8* ptrData = uniformBufferData + paramDesc.cpuMemOffset * sizeof(UINT32);
			hasBoundAtLeastOne = true;

			// Note: We don't transpose matrices here even though we don't use column major format
			// because they are assumed to be pre-transposed in the GpuParams buffer
			switch(paramDesc.type)
			{
			case GPDT_FLOAT1:
				glProgramUniform1fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GPDT_FLOAT2:
				glProgramUniform2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GPDT_FLOAT3:
				glProgramUniform3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GPDT_FLOAT4:
				glProgramUniform4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_2X2:
				glProgramUniformMatrix2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_2X3:
				glProgramUniformMatrix3x2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_2X4:
				glProgramUniformMatrix4x2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_3X2:
				glProgramUniformMatrix2x3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_3X3:
				glProgramUniformMatrix3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_3X4:
				glProgramUniformMatrix4x3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_4X2:
				glProgramUniformMatrix2x4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_4X3:
				glProgramUniformMatrix3x4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_MATRIX_4X4:
				glProgramUniformMatrix4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GPDT_INT1:
				glProgramUniform1iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GPDT_INT2:
				glProgramUniform2iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GPDT_INT3:
				glProgramUniform3iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GPDT_INT4:
				glProgramUniform4iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GPDT_BOOL:
				glProgramUniform1uiv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLuint*)ptrData);
				break;
			case GPDT_UNKNOWN:
				break;
			}
		}

		if (hasBoundAtLeastOne)
			BS_INC_RENDER_STAT(NumGpuParamBufferBinds);

		if(uniformBufferData != nullptr)
		{
			bs_free<ScratchAlloc>(uniformBufferData);
		}
	}

	void GLRenderSystem::setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr)
	{
		THROW_IF_NOT_CORE_THREAD;

		unit = getGLTextureUnit(gptype, unit);

		SPtr<GLTextureCore> tex = std::static_pointer_cast<GLTextureCore>(texPtr);

		GLenum lastTextureType = mTextureTypes[unit];

		if (!activateGLTextureUnit(unit))
			return;

		if (enabled && tex)
		{
			mTextureTypes[unit] = tex->getGLTextureTarget();
			glBindTexture(mTextureTypes[unit], tex->getGLID());
		}
		else
		{
			// TODO - This doesn't actually disable all textures set on this image unit, only the 2D ones
			//      - If a non-2D sampler is used, the texture will still be displayed

			glBindTexture(GL_TEXTURE_2D, 0); 
		}

		activateGLTextureUnit(0);

		BS_INC_RENDER_STAT(NumTextureBinds);
	}

	void GLRenderSystem::setSamplerState(GpuProgramType gptype, UINT16 unit, const SPtr<SamplerStateCore>& state)
	{
		THROW_IF_NOT_CORE_THREAD;

		const SamplerProperties& stateProps = state->getProperties();

		unit = getGLTextureUnit(gptype, unit);

		// Set texture layer filtering
		setTextureFiltering(unit, FT_MIN, stateProps.getTextureFiltering(FT_MIN));
		setTextureFiltering(unit, FT_MAG, stateProps.getTextureFiltering(FT_MAG));
		setTextureFiltering(unit, FT_MIP, stateProps.getTextureFiltering(FT_MIP));

		// Set texture anisotropy
		setTextureAnisotropy(unit, stateProps.getTextureAnisotropy());

		// Set mipmap biasing
		setTextureMipmapBias(unit, stateProps.getTextureMipmapBias());

		// Texture addressing mode
		const UVWAddressingMode& uvw = stateProps.getTextureAddressingMode();
		setTextureAddressingMode(unit, uvw);

		// Set border color
		setTextureBorderColor(unit, stateProps.getBorderColor());

		BS_INC_RENDER_STAT(NumSamplerBinds);
	}

	void GLRenderSystem::setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr,
		const TextureSurface& surface)
	{
		THROW_IF_NOT_CORE_THREAD;

		// TODO - OpenGL can't bind a certain subset of faces like DX11, only zero, one or all, so I'm ignoring numSlices parameter
		if (texPtr != nullptr)
		{
			SPtr<GLTextureCore> tex = std::static_pointer_cast<GLTextureCore>(texPtr);
			glBindImageTexture(unit, tex->getGLID(), surface.mipLevel, surface.numArraySlices > 1, 
				surface.arraySlice, tex->getGLFormat(), GL_READ_WRITE);
		}
		else
			glBindImageTexture(unit, 0, 0, false, 0, 0, GL_READ_WRITE);

		BS_INC_RENDER_STAT(NumTextureBinds);
	}

	void GLRenderSystem::setBlendState(const SPtr<BlendStateCore>& blendState)
	{
		THROW_IF_NOT_CORE_THREAD;

		const BlendProperties& stateProps = blendState->getProperties();

		// Alpha to coverage
		setAlphaToCoverage(stateProps.getAlphaToCoverageEnabled());

		// Blend states
		// OpenGL doesn't allow us to specify blend state per render target, so we just use the first one.
		if (stateProps.getBlendEnabled(0))
		{
			setSceneBlending(stateProps.getSrcBlend(0), stateProps.getDstBlend(0), stateProps.getAlphaSrcBlend(0),
				stateProps.getAlphaDstBlend(0), stateProps.getBlendOperation(0), stateProps.getAlphaBlendOperation(0));
		}
		else
		{
			setSceneBlending(BF_ONE, BF_ZERO, BO_ADD);
		}

		// Color write mask
		UINT8 writeMask = stateProps.getRenderTargetWriteMask(0);
		setColorBufferWriteEnabled((writeMask & 0x1) != 0, (writeMask & 0x2) != 0, (writeMask & 0x4) != 0, (writeMask & 0x8) != 0);

		BS_INC_RENDER_STAT(NumBlendStateChanges);
	}

	void GLRenderSystem::setRasterizerState(const SPtr<RasterizerStateCore>& rasterizerState)
	{
		THROW_IF_NOT_CORE_THREAD;

		const RasterizerProperties& stateProps = rasterizerState->getProperties();

		setDepthBias((float)stateProps.getDepthBias(), stateProps.getSlopeScaledDepthBias());

		setCullingMode(stateProps.getCullMode());

		setPolygonMode(stateProps.getPolygonMode());

		setScissorTestEnable(stateProps.getScissorEnable());

		setMultisamplingEnable(stateProps.getMultisampleEnable());

		setDepthClipEnable(stateProps.getDepthClipEnable());

		setAntialiasedLineEnable(stateProps.getAntialiasedLineEnable());

		BS_INC_RENDER_STAT(NumRasterizerStateChanges);
	}

	void GLRenderSystem::setDepthStencilState(const SPtr<DepthStencilStateCore>& depthStencilState, UINT32 stencilRefValue)
	{
		THROW_IF_NOT_CORE_THREAD;

		const DepthStencilProperties& stateProps = depthStencilState->getProperties();

		// Set stencil buffer options
		setStencilCheckEnabled(stateProps.getStencilEnable());

		setStencilBufferOperations(stateProps.getStencilFrontFailOp(), stateProps.getStencilFrontZFailOp(), stateProps.getStencilFrontPassOp(), true);
		setStencilBufferFunc(stateProps.getStencilFrontCompFunc(), stateProps.getStencilReadMask(), true);

		setStencilBufferOperations(stateProps.getStencilBackFailOp(), stateProps.getStencilBackZFailOp(), stateProps.getStencilBackPassOp(), false);
		setStencilBufferFunc(stateProps.getStencilBackCompFunc(), stateProps.getStencilReadMask(), false);

		setStencilBufferWriteMask(stateProps.getStencilWriteMask());

		// Set depth buffer options
		setDepthBufferCheckEnabled(stateProps.getDepthReadEnable());
		setDepthBufferWriteEnabled(stateProps.getDepthWriteEnable());
		setDepthBufferFunction(stateProps.getDepthComparisonFunc());

		// Set stencil ref value
		setStencilRefValue(stencilRefValue);

		BS_INC_RENDER_STAT(NumDepthStencilStateChanges);
	}

	void GLRenderSystem::setViewport(const Rect2& area)
	{
		THROW_IF_NOT_CORE_THREAD;

		mViewportNorm = area;
		applyViewport();
	}

	void GLRenderSystem::setRenderTarget(const SPtr<RenderTargetCore>& target)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Switch context if different from current one
		GLContext *newContext = 0;
		target->getCustomAttribute("GLCONTEXT", &newContext);
		if(newContext && mCurrentContext != newContext) 
		{
			switchContext(newContext);
		}

		// This must happen after context switch to ensure previous context is still alive
		mActiveRenderTarget = target;

		GLFrameBufferObject *fbo = 0;
		target->getCustomAttribute("FBO", &fbo);
		if(fbo)
			fbo->bind();
		else
			// Old style context (window/pbuffer) or copying render texture
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		if (GLEW_EXT_framebuffer_sRGB)
		{
			// Enable / disable sRGB states
			if (target->getProperties().isHwGammaEnabled())
			{
				glEnable(GL_FRAMEBUFFER_SRGB_EXT);

				// Note: could test GL_FRAMEBUFFER_SRGB_CAPABLE_EXT here before
				// enabling, but GL spec says incapable surfaces ignore the setting
				// anyway. We test the capability to enable isHardwareGammaEnabled.
			}
			else
			{
				glDisable(GL_FRAMEBUFFER_SRGB_EXT);
			}
		}

		applyViewport();

		BS_INC_RENDER_STAT(NumRenderTargetChanges);
	}

	void GLRenderSystem::beginFrame()
	{
		THROW_IF_NOT_CORE_THREAD;

		// Activate the viewport clipping
		glEnable(GL_SCISSOR_TEST);
	}

	void GLRenderSystem::endFrame()
	{
		THROW_IF_NOT_CORE_THREAD;

		// Deactivate the viewport clipping.
		glDisable(GL_SCISSOR_TEST);
	}

	void GLRenderSystem::setVertexBuffers(UINT32 index, SPtr<VertexBufferCore>* buffers, UINT32 numBuffers)
	{
		THROW_IF_NOT_CORE_THREAD;

		if ((index + numBuffers) > (UINT32)mBoundVertexBuffers.size())
			mBoundVertexBuffers.resize(index + numBuffers);

		for(UINT32 i = 0; i < numBuffers; i++)
		{
			mBoundVertexBuffers[index + i] = buffers[i];
		}
	}

	void GLRenderSystem::setVertexDeclaration(const SPtr<VertexDeclarationCore>& vertexDeclaration)
	{
		THROW_IF_NOT_CORE_THREAD;

		mBoundVertexDeclaration = vertexDeclaration;
	}

	void GLRenderSystem::setDrawOperation(DrawOperationType op)
	{
		THROW_IF_NOT_CORE_THREAD;

		mCurrentDrawOperation = op;
	}

	void GLRenderSystem::setIndexBuffer(const SPtr<IndexBufferCore>& buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		mBoundIndexBuffer = buffer;
	}

	void GLRenderSystem::draw(UINT32 vertexOffset, UINT32 vertexCount)
	{
		// Find the correct type to render
		GLint primType = getGLDrawMode();
		beginDraw();

		glDrawArrays(primType, vertexOffset, vertexCount);

		endDraw();

		UINT32 primCount = vertexCountToPrimCount(mCurrentDrawOperation, vertexCount);

		BS_INC_RENDER_STAT(NumDrawCalls);
		BS_ADD_RENDER_STAT(NumVertices, vertexCount);
		BS_ADD_RENDER_STAT(NumPrimitives, primCount);
	}

	void GLRenderSystem::drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount)
	{
		if(mBoundIndexBuffer == nullptr)
		{
			LOGWRN("Cannot draw indexed because index buffer is not set.");
			return;
		}

		// Find the correct type to render
		GLint primType = getGLDrawMode();

		beginDraw();

		SPtr<GLIndexBufferCore> indexBuffer = std::static_pointer_cast<GLIndexBufferCore>(mBoundIndexBuffer);
		const IndexBufferProperties& ibProps = indexBuffer->getProperties();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getGLBufferId());

		GLenum indexType = (ibProps.getType() == IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		glDrawElementsBaseVertex(primType, indexCount, indexType, (GLvoid*)(ibProps.getIndexSize() * startIndex), vertexOffset);

		endDraw();
		UINT32 primCount = vertexCountToPrimCount(mCurrentDrawOperation, vertexCount);

		BS_INC_RENDER_STAT(NumDrawCalls);
		BS_ADD_RENDER_STAT(NumVertices, vertexCount);
		BS_ADD_RENDER_STAT(NumPrimitives, primCount);

		BS_INC_RENDER_STAT(NumIndexBufferBinds);
	}

	void GLRenderSystem::setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		THROW_IF_NOT_CORE_THREAD;

		mScissorTop = top;
		mScissorBottom = bottom;
		mScissorLeft = left;
		mScissorRight = right;
	}

	void GLRenderSystem::clearRenderTarget(UINT32 buffers, const Color& color, float depth, UINT16 stencil)
	{
		if(mActiveRenderTarget == nullptr)
			return;

		const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();
		Rect2I clearRect(0, 0, rtProps.getWidth(), rtProps.getHeight());

		clearArea(buffers, color, depth, stencil, clearRect);
	}

	void GLRenderSystem::clearViewport(UINT32 buffers, const Color& color, float depth, UINT16 stencil)
	{
		Rect2I clearRect(mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight);

		clearArea(buffers, color, depth, stencil, clearRect);
	}

	void GLRenderSystem::clearArea(UINT32 buffers, const Color& color, float depth, UINT16 stencil, const Rect2I& clearRect)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mActiveRenderTarget == nullptr)
			return;

		bool colorMask = !mColorWrite[0] || !mColorWrite[1] 
		|| !mColorWrite[2] || !mColorWrite[3]; 

		GLbitfield flags = 0;
		if (buffers & FBT_COLOR)
		{
			flags |= GL_COLOR_BUFFER_BIT;

			// Enable buffer for writing if it isn't
			if (colorMask)
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glClearColor(color.r, color.g, color.b, color.a);
		}
		if (buffers & FBT_DEPTH)
		{
			flags |= GL_DEPTH_BUFFER_BIT;

			// Enable buffer for writing if it isn't
			if (!mDepthWrite)
				glDepthMask(GL_TRUE);

			glClearDepth(depth);
		}
		if (buffers & FBT_STENCIL)
		{
			flags |= GL_STENCIL_BUFFER_BIT;

			// Enable buffer for writing if it isn't
			glStencilMask(0xFFFFFFFF);

			glClearStencil(stencil);
		}

		// Disable scissor test as we want to clear the entire render surface
		GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
		UINT32 oldScissorTop = mScissorTop;
		UINT32 oldScissorBottom = mScissorBottom;
		UINT32 oldScissorLeft = mScissorLeft;
		UINT32 oldScissorRight = mScissorRight;

		if (scissorTestEnabled)
		{
			glDisable(GL_SCISSOR_TEST);
		}

		const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();

		bool clearEntireTarget = clearRect.width == 0 || clearRect.height == 0;
		clearEntireTarget |= (clearRect.x == 0 && clearRect.y == 0 && clearRect.width == rtProps.getWidth() && clearRect.height == rtProps.getHeight());

		if(!clearEntireTarget)
		{
			setScissorRect(clearRect.x, clearRect.y, clearRect.x + clearRect.width, clearRect.y + clearRect.height);
			setScissorTestEnable(true);			
		}

		// Clear buffers
		glClear(flags);

		if(!clearEntireTarget)
		{
			setScissorTestEnable(false);	
		}

		// Restore scissor test
		if (scissorTestEnabled)
		{
			glEnable(GL_SCISSOR_TEST);

			mScissorTop = oldScissorTop;
			mScissorBottom = oldScissorBottom;
			mScissorLeft = oldScissorLeft;
			mScissorRight = oldScissorRight;
		}

		// Reset buffer write state
		if (!mDepthWrite && (buffers & FBT_DEPTH))
		{
			glDepthMask(GL_FALSE);
		}
		if (colorMask && (buffers & FBT_COLOR))
		{
			glColorMask(mColorWrite[0], mColorWrite[1], mColorWrite[2], mColorWrite[3]);
		}
		if (buffers & FBT_STENCIL)
		{
			glStencilMask(mStencilWriteMask);
		}

		BS_INC_RENDER_STAT(NumClears);
	}

	/************************************************************************/
	/* 								PRIVATE		                     		*/
	/************************************************************************/

	void GLRenderSystem::setTextureAddressingMode(UINT16 stage, const UVWAddressingMode& uvw)
	{
		if (!activateGLTextureUnit(stage))
			return;

		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_S, 
			getTextureAddressingMode(uvw.u));
		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_T, 
			getTextureAddressingMode(uvw.v));
		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_R, 
			getTextureAddressingMode(uvw.w));
		activateGLTextureUnit(0);
	}

	void GLRenderSystem::setTextureBorderColor(UINT16 stage, const Color& colour)
	{
		GLfloat border[4] = { colour.r, colour.g, colour.b, colour.a };
		if (activateGLTextureUnit(stage))
		{
			glTexParameterfv(mTextureTypes[stage], GL_TEXTURE_BORDER_COLOR, border);
			activateGLTextureUnit(0);
		}
	}

	void GLRenderSystem::setTextureMipmapBias(UINT16 stage, float bias)
	{
		if (mCurrentCapabilities->hasCapability(RSC_MIPMAP_LOD_BIAS))
		{
			if (activateGLTextureUnit(stage))
			{
				glTexParameterf(mTextureTypes[stage], GL_TEXTURE_LOD_BIAS, bias);
				activateGLTextureUnit(0);
			}
		}

	}

	void GLRenderSystem::setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, BlendOperation op )
	{
		GLint sourceBlend = getBlendMode(sourceFactor);
		GLint destBlend = getBlendMode(destFactor);
		if(sourceFactor == BF_ONE && destFactor == BF_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFunc(sourceBlend, destBlend);
		}

		GLint func = GL_FUNC_ADD;
		switch(op)
		{
		case BO_ADD:
			func = GL_FUNC_ADD;
			break;
		case BO_SUBTRACT:
			func = GL_FUNC_SUBTRACT;
			break;
		case BO_REVERSE_SUBTRACT:
			func = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case BO_MIN:
			func = GL_MIN;
			break;
		case BO_MAX:
			func = GL_MAX;
			break;
		}

		if(GLEW_VERSION_1_4 || GLEW_ARB_imaging)
		{
			glBlendEquation(func);
		}
		else if(GLEW_EXT_blend_minmax && (func == GL_MIN || func == GL_MAX))
		{
			glBlendEquationEXT(func);
		}
	}


	void GLRenderSystem::setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, 
		BlendFactor sourceFactorAlpha, BlendFactor destFactorAlpha, BlendOperation op, BlendOperation alphaOp)
	{
		GLint sourceBlend = getBlendMode(sourceFactor);
		GLint destBlend = getBlendMode(destFactor);
		GLint sourceBlendAlpha = getBlendMode(sourceFactorAlpha);
		GLint destBlendAlpha = getBlendMode(destFactorAlpha);

		if(sourceFactor == BF_ONE && destFactor == BF_ZERO && 
			sourceFactorAlpha == BF_ONE && destFactorAlpha == BF_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFuncSeparate(sourceBlend, destBlend, sourceBlendAlpha, destBlendAlpha);
		}

		GLint func = GL_FUNC_ADD, alphaFunc = GL_FUNC_ADD;

		switch(op)
		{
		case BO_ADD:
			func = GL_FUNC_ADD;
			break;
		case BO_SUBTRACT:
			func = GL_FUNC_SUBTRACT;
			break;
		case BO_REVERSE_SUBTRACT:
			func = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case BO_MIN:
			func = GL_MIN;
			break;
		case BO_MAX:
			func = GL_MAX;
			break;
		}

		switch(alphaOp)
		{
		case BO_ADD:
			alphaFunc = GL_FUNC_ADD;
			break;
		case BO_SUBTRACT:
			alphaFunc = GL_FUNC_SUBTRACT;
			break;
		case BO_REVERSE_SUBTRACT:
			alphaFunc = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case BO_MIN:
			alphaFunc = GL_MIN;
			break;
		case BO_MAX:
			alphaFunc = GL_MAX;
			break;
		}

		if(GLEW_VERSION_2_0) {
			glBlendEquationSeparate(func, alphaFunc);
		}
		else if(GLEW_EXT_blend_equation_separate) {
			glBlendEquationSeparateEXT(func, alphaFunc);
		}
	}

	void GLRenderSystem::setAlphaTest(CompareFunction func, unsigned char value)
	{
		if(func == CMPF_ALWAYS_PASS)
		{
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(convertCompareFunction(func), value / 255.0f);
		}
	}

	void GLRenderSystem::setAlphaToCoverage(bool enable)
	{
		static bool lasta2c = false;

		if (enable != lasta2c && getCapabilities()->hasCapability(RSC_ALPHA_TO_COVERAGE))
		{
			if (enable)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			lasta2c = enable;
		}
	}

	void GLRenderSystem::setScissorTestEnable(bool enable)
	{
		const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();

		// If request texture flipping, use "upper-left", otherwise use "lower-left"
		bool flipping = rtProps.requiresTextureFlipping();
		//  GL measures from the bottom, not the top
		UINT32 targetHeight = rtProps.getHeight();
		// Calculate the "lower-left" corner of the viewport
		GLsizei x = 0, y = 0, w = 0, h = 0;

		if (enable)
		{
			glEnable(GL_SCISSOR_TEST);
			// GL uses width / height rather than right / bottom
			x = mScissorLeft;

			if (flipping)
				y = targetHeight - mScissorBottom - 1;
			else
				y = mScissorTop;

			w = mScissorRight - mScissorLeft;
			h = mScissorBottom - mScissorTop;

			glScissor(x, y, w, h);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);

			// GL requires you to reset the scissor when disabling
			w = mViewportWidth;
			h = mViewportHeight;
			x = mViewportLeft;
			y = mViewportTop; 

			glScissor(x, y, w, h);
		}
	}

	void GLRenderSystem::setMultisamplingEnable(bool enable)
	{
		if (enable)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
	}

	void GLRenderSystem::setDepthClipEnable(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_CLAMP);
		else
			glDisable(GL_DEPTH_CLAMP);
	}

	void GLRenderSystem::setAntialiasedLineEnable(bool enable)
	{
		if (enable)
			glEnable(GL_LINE_SMOOTH);
		else
			glDisable(GL_LINE_SMOOTH);
	}


	void GLRenderSystem::setCullingMode(CullingMode mode)
	{
		mCullingMode = mode;
		GLenum cullMode;

		switch( mode )
		{
		case CULL_NONE:
			glDisable(GL_CULL_FACE);
			return;
		default:
		case CULL_CLOCKWISE:
			cullMode = GL_BACK;
			break;
		case CULL_COUNTERCLOCKWISE:
			cullMode = GL_FRONT;
			break;
		}

		glEnable(GL_CULL_FACE);
		glCullFace(cullMode);
	}

	void GLRenderSystem::setDepthBufferCheckEnabled(bool enabled)
	{
		if (enabled)
		{
			glClearDepth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void GLRenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		GLboolean flag = enabled ? GL_TRUE : GL_FALSE;
		glDepthMask(flag);  

		mDepthWrite = enabled;
	}

	void GLRenderSystem::setDepthBufferFunction(CompareFunction func)
	{
		glDepthFunc(convertCompareFunction(func));
	}

	void GLRenderSystem::setDepthBias(float constantBias, float slopeScaleBias)
	{
		if (constantBias != 0 || slopeScaleBias != 0)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_POLYGON_OFFSET_POINT);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-slopeScaleBias, -constantBias);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_POLYGON_OFFSET_POINT);
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
	}

	void GLRenderSystem::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		glColorMask(red, green, blue, alpha);
		// record this
		mColorWrite[0] = red;
		mColorWrite[1] = blue;
		mColorWrite[2] = green;
		mColorWrite[3] = alpha;
	}

	void GLRenderSystem::setPolygonMode(PolygonMode level)
	{
		GLenum glmode;
		switch(level)
		{
		case PM_WIREFRAME:
			glmode = GL_LINE;
			break;
		default:
		case PM_SOLID:
			glmode = GL_FILL;
			break;
		}
		glPolygonMode(GL_FRONT_AND_BACK, glmode);
	}

	void GLRenderSystem::setStencilCheckEnabled(bool enabled)
	{
		if (enabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}

	void GLRenderSystem::setStencilBufferOperations(StencilOperation stencilFailOp,
		StencilOperation depthFailOp, StencilOperation passOp, bool front)
	{
		if (front)
		{
			glStencilOpSeparate(GL_FRONT, 
				convertStencilOp(stencilFailOp),
				convertStencilOp(depthFailOp), 
				convertStencilOp(passOp));
		}
		else
		{
			glStencilOpSeparate(GL_BACK, 
				convertStencilOp(stencilFailOp, true), 
				convertStencilOp(depthFailOp, true), 
				convertStencilOp(passOp, true));
		}
	}

	void GLRenderSystem::setStencilBufferFunc(CompareFunction func, UINT32 mask, bool front)
	{
		mStencilReadMask = mask;

		if(front)
		{
			mStencilCompareFront = func;
			glStencilFuncSeparate(GL_FRONT, convertCompareFunction(mStencilCompareFront), mStencilRefValue, mStencilReadMask);
		}
		else
		{
			mStencilCompareBack = func;
			glStencilFuncSeparate(GL_BACK, convertCompareFunction(mStencilCompareBack), mStencilRefValue, mStencilReadMask);
		}
	}

	void GLRenderSystem::setStencilBufferWriteMask(UINT32 mask)
	{
		mStencilWriteMask = mask;
		glStencilMask(mask);
	}

	void GLRenderSystem::setStencilRefValue(UINT32 refValue)
	{
		THROW_IF_NOT_CORE_THREAD;

		mStencilRefValue = refValue;

		glStencilFuncSeparate(GL_FRONT, convertCompareFunction(mStencilCompareFront), mStencilRefValue, mStencilReadMask);
		glStencilFuncSeparate(GL_BACK, convertCompareFunction(mStencilCompareBack), mStencilRefValue, mStencilReadMask);
	}

	void GLRenderSystem::setTextureFiltering(UINT16 unit, FilterType ftype, FilterOptions fo)
	{
		if (!activateGLTextureUnit(unit))
			return;

		switch(ftype)
		{
		case FT_MIN:
			mMinFilter = fo;
			// Combine with existing mip filter
			glTexParameteri(mTextureTypes[unit], GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter());
			break;
		case FT_MAG:
			switch (fo)
			{
			case FO_ANISOTROPIC: // GL treats linear and aniso the same
			case FO_LINEAR:
				glTexParameteri(mTextureTypes[unit], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FO_POINT:
			case FO_NONE:
				glTexParameteri(mTextureTypes[unit], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			}
			break;
		case FT_MIP:
			mMipFilter = fo;
			// Combine with existing min filter
			glTexParameteri(mTextureTypes[unit], GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter());
			break;
		}

		activateGLTextureUnit(0);
	}

	void GLRenderSystem::setTextureAnisotropy(UINT16 unit, UINT32 maxAnisotropy)
	{
		if (!mCurrentCapabilities->hasCapability(RSC_ANISOTROPY))
			return;

		if (!activateGLTextureUnit(unit))
			return;

		GLfloat largest_supported_anisotropy = 0;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
		if (maxAnisotropy > largest_supported_anisotropy)
			maxAnisotropy = largest_supported_anisotropy ? 
			static_cast<UINT32>(largest_supported_anisotropy) : 1;

		if(maxAnisotropy < 1)
			maxAnisotropy = 1;

		if (getCurrentAnisotropy(unit) != maxAnisotropy)
			glTexParameterf(mTextureTypes[unit], GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)maxAnisotropy);

		activateGLTextureUnit(0);
	}

	void GLRenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
	{
		size_t i = 0;
		size_t numClipPlanes;
		GLdouble clipPlane[4];

		numClipPlanes = clipPlanes.size();
		for (i = 0; i < numClipPlanes; ++i)
		{
			GLenum clipPlaneId = static_cast<GLenum>(GL_CLIP_PLANE0 + i);
			const Plane& plane = clipPlanes[i];

			if (i >= 6)
			{
				BS_EXCEPT(RenderingAPIException, "Unable to set clip plane");
			}

			clipPlane[0] = plane.normal.x;
			clipPlane[1] = plane.normal.y;
			clipPlane[2] = plane.normal.z;
			clipPlane[3] = plane.d;

			glClipPlane(clipPlaneId, clipPlane);
			glEnable(clipPlaneId);
		}

		// Disable remaining clip planes
		for (; i < 6; ++i)
		{
			glDisable(static_cast<GLenum>(GL_CLIP_PLANE0 + i));
		}
	}

	bool GLRenderSystem::activateGLTextureUnit(UINT16 unit)
	{
		if (mActiveTextureUnit != unit)
		{
			if (unit < getCapabilities()->getNumCombinedTextureUnits())
			{
				glActiveTexture(GL_TEXTURE0 + unit);
				mActiveTextureUnit = unit;
				return true;
			}
			else if (!unit)
			{
				// always ok to use the first unit
				return true;
			}
			else
			{
				LOGWRN("Provided texture unit index is higher than OpenGL supports. Provided: " + toString(unit) + 
					". Supported range: 0 .. " + toString(getCapabilities()->getNumCombinedTextureUnits() - 1));
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	void GLRenderSystem::beginDraw()
	{
		if(mDrawCallInProgress)
			BS_EXCEPT(InternalErrorException, "Calling beginDraw without finishing previous draw call. Please call endDraw().");

		mDrawCallInProgress = true;

		if(mCurrentVertexProgram == nullptr)
		{
			LOGWRN("Cannot render without a set vertex shader.");
			return;
		}

		if(mBoundVertexDeclaration == nullptr)
		{
			LOGWRN("Cannot render without a set vertex declaration.");
			return;
		}

		const GLSLProgramPipeline* pipeline = mProgramPipelineManager->getPipeline(mCurrentVertexProgram.get(), 
			mCurrentFragmentProgram.get(), mCurrentGeometryProgram.get(), mCurrentHullProgram.get(), mCurrentDomainProgram.get());

		if(mActivePipeline != pipeline)
		{
			glBindProgramPipeline(pipeline->glHandle);
			mActivePipeline = pipeline;
		}

		const GLVertexArrayObject& vao = GLVertexArrayObjectManager::instance().getVAO(mCurrentVertexProgram, mBoundVertexDeclaration, mBoundVertexBuffers);
		glBindVertexArray(vao.getGLHandle()); 

		BS_INC_RENDER_STAT(NumVertexBufferBinds);
		BS_INC_RENDER_STAT(NumGpuProgramBinds);
	}

	void GLRenderSystem::endDraw()
	{
		if(!mDrawCallInProgress)
			return;

		mDrawCallInProgress = false;
	}

	GLfloat GLRenderSystem::getCurrentAnisotropy(UINT16 unit)
	{
		GLfloat curAniso = 0;
		glGetTexParameterfv(mTextureTypes[unit], GL_TEXTURE_MAX_ANISOTROPY_EXT, &curAniso);

		return curAniso ? curAniso : 1;
	}

	GLint GLRenderSystem::convertStencilOp(StencilOperation op, bool invert) const
	{
		switch (op)
		{
		case SOP_KEEP:
			return GL_KEEP;
		case SOP_ZERO:
			return GL_ZERO;
		case SOP_REPLACE:
			return GL_REPLACE;
		case SOP_INCREMENT:
			return invert ? GL_DECR : GL_INCR;
		case SOP_DECREMENT:
			return invert ? GL_INCR : GL_DECR;
		case SOP_INCREMENT_WRAP:
			return invert ? GL_DECR_WRAP_EXT : GL_INCR_WRAP_EXT;
		case SOP_DECREMENT_WRAP:
			return invert ? GL_INCR_WRAP_EXT : GL_DECR_WRAP_EXT;
		case SOP_INVERT:
			return GL_INVERT;
		};
		// to keep compiler happy
		return SOP_KEEP;
	}

	GLint GLRenderSystem::convertCompareFunction(CompareFunction func) const
	{
		switch (func)
		{
		case CMPF_ALWAYS_FAIL:
			return GL_NEVER;
		case CMPF_ALWAYS_PASS:
			return GL_ALWAYS;
		case CMPF_LESS:
			return GL_LESS;
		case CMPF_LESS_EQUAL:
			return GL_LEQUAL;
		case CMPF_EQUAL:
			return GL_EQUAL;
		case CMPF_NOT_EQUAL:
			return GL_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return GL_GEQUAL;
		case CMPF_GREATER:
			return GL_GREATER;
		};

		return GL_ALWAYS;
	}

	GLuint GLRenderSystem::getCombinedMinMipFilter() const
	{
		switch (mMinFilter)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			switch (mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// Linear min, linear mip
				return GL_LINEAR_MIPMAP_LINEAR;
			case FO_POINT:
				// Linear min, point mip
				return GL_LINEAR_MIPMAP_NEAREST;
			case FO_NONE:
				// Linear min, no mip
				return GL_LINEAR;
			}
			break;
		case FO_POINT:
		case FO_NONE:
			switch (mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// Nearest min, linear mip
				return GL_NEAREST_MIPMAP_LINEAR;
			case FO_POINT:
				// Nearest min, point mip
				return GL_NEAREST_MIPMAP_NEAREST;
			case FO_NONE:
				// Nearest min, no mip
				return GL_NEAREST;
			}
			break;
		}

		// Should never get here
		return 0;
	}

	GLint GLRenderSystem::getBlendMode(BlendFactor blendMode) const
	{
		switch (blendMode)
		{
		case BF_ONE:
			return GL_ONE;
		case BF_ZERO:
			return GL_ZERO;
		case BF_DEST_COLOR:
			return GL_DST_COLOR;
		case BF_SOURCE_COLOR:
			return GL_SRC_COLOR;
		case BF_INV_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case BF_INV_SOURCE_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case BF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case BF_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case BF_INV_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BF_INV_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		};

		return GL_ONE;
	}

	GLint GLRenderSystem::getTextureAddressingMode(TextureAddressingMode tam) const
	{
		switch (tam)
		{
		default:
		case TAM_WRAP:
			return GL_REPEAT;
		case TAM_MIRROR:
			return GL_MIRRORED_REPEAT;
		case TAM_CLAMP:
			return GL_CLAMP_TO_EDGE;
		case TAM_BORDER:
			return GL_CLAMP_TO_BORDER;
		}
	}

	GLint GLRenderSystem::getGLDrawMode() const
	{
		GLint primType;

		// Use adjacency if there is a geometry program and it requested adjacency info
		bool useAdjacency = (mGeometryProgramBound && mCurrentGeometryProgram->isAdjacencyInfoRequired());
		switch (mCurrentDrawOperation)
		{
		case DOT_POINT_LIST:
			primType = GL_POINTS;
			break;
		case DOT_LINE_LIST:
			primType = useAdjacency ? GL_LINES_ADJACENCY_EXT : GL_LINES;
			break;
		case DOT_LINE_STRIP:
			primType = useAdjacency ? GL_LINE_STRIP_ADJACENCY_EXT : GL_LINE_STRIP;
			break;
		default:
		case DOT_TRIANGLE_LIST:
			primType = useAdjacency ? GL_TRIANGLES_ADJACENCY_EXT : GL_TRIANGLES;
			break;
		case DOT_TRIANGLE_STRIP:
			primType = useAdjacency ? GL_TRIANGLE_STRIP_ADJACENCY_EXT : GL_TRIANGLE_STRIP;
			break;
		case DOT_TRIANGLE_FAN:
			primType = GL_TRIANGLE_FAN;
			break;
		}

		return primType;
	}

	UINT32 GLRenderSystem::getGLTextureUnit(GpuProgramType gptype, UINT32 unit)
	{
		if (gptype != GPT_VERTEX_PROGRAM && gptype != GPT_FRAGMENT_PROGRAM && gptype != GPT_GEOMETRY_PROGRAM)
		{
			BS_EXCEPT(InvalidParametersException, "OpenGL cannot assign textures to this gpu program type: " + toString(gptype));
		}

		UINT32 numSupportedUnits = mCurrentCapabilities->getNumTextureUnits(gptype);
		if (unit < 0 || unit >= numSupportedUnits)
		{
			BS_EXCEPT(InvalidParametersException, "Invalid texture unit index for the provided stage. Unit index: " + toString(unit) + ". Stage: " +
				toString(gptype) + ". Supported range is 0 .. " + toString(numSupportedUnits - 1));
		}

		switch (gptype)
		{
		case GPT_FRAGMENT_PROGRAM:
			return mFragmentTexOffset + unit;
		case GPT_VERTEX_PROGRAM:
			return mVertexTexOffset + unit;
		case GPT_GEOMETRY_PROGRAM:
			return mGeometryTexOffset + unit;
		default:
			BS_EXCEPT(InternalErrorException, "Invalid program type: " + toString(gptype));
		}
	}

	UINT32 GLRenderSystem::getGLUniformBlockBinding(GpuProgramType gptype, UINT32 binding)
	{
		UINT32 maxNumBindings = mCurrentCapabilities->getNumGpuParamBlockBuffers(gptype);
		if (binding < 0 || binding >= maxNumBindings)
		{
			BS_EXCEPT(InvalidParametersException, "Invalid buffer binding for the provided stage. Buffer binding: " + toString(binding) + ". Stage: " +
				toString(gptype) + ". Supported range is 0 .. " + toString(maxNumBindings - 1));
		}

		switch (gptype)
		{
		case GPT_FRAGMENT_PROGRAM:
			return mFragmentUBOffset + binding;
		case GPT_VERTEX_PROGRAM:
			return mVertexUBOffset + binding;
		case GPT_GEOMETRY_PROGRAM:
			return mGeometryUBOffset + binding;
		case GPT_HULL_PROGRAM:
			return mHullUBOffset + binding;
		case GPT_DOMAIN_PROGRAM:
			return mDomainUBOffset + binding;
		case GPT_COMPUTE_PROGRAM:
			return mComputeUBOffset + binding;
		default:
			BS_EXCEPT(InternalErrorException, "Invalid program type: " + toString(gptype));
		}
	}

	void GLRenderSystem::setActiveProgram(GpuProgramType gptype, const SPtr<GLSLGpuProgramCore>& program)
	{
		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			mCurrentVertexProgram = program;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mCurrentFragmentProgram = program;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mCurrentGeometryProgram = program;
			break;
		case GPT_DOMAIN_PROGRAM:
			mCurrentDomainProgram = program;
			break;
		case GPT_HULL_PROGRAM:
			mCurrentHullProgram = program;
			break;
		}
	}

	SPtr<GLSLGpuProgramCore> GLRenderSystem::getActiveProgram(GpuProgramType gptype) const
	{
		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return mCurrentVertexProgram;
			break;
		case GPT_FRAGMENT_PROGRAM:
			return mCurrentFragmentProgram;
			break;
		case GPT_GEOMETRY_PROGRAM:
			return mCurrentGeometryProgram;
			break;
		case GPT_DOMAIN_PROGRAM:
			return mCurrentDomainProgram;
			break;
		case GPT_HULL_PROGRAM:
			return mCurrentHullProgram;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Insupported gpu program type: " + toString(gptype));
		}
	}

	void GLRenderSystem::initFromCaps(RenderSystemCapabilities* caps)
	{
		if(caps->getRenderSystemName() != getName())
		{
			BS_EXCEPT(InvalidParametersException, 
				"Trying to initialize GLRenderSystem from RenderSystemCapabilities that do not support OpenGL");
		}

#if BS_DEBUG_MODE
		if (mGLSupport->checkExtension("GL_ARB_debug_output"))
		{
			glDebugMessageCallback(&openGlErrorCallback, 0);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
#endif

		HardwareBufferManager::startUp();
		HardwareBufferCoreManager::startUp<GLHardwareBufferCoreManager>();

		// GPU Program Manager setup
		if(caps->isShaderProfileSupported("glsl"))
		{
			mGLSLProgramFactory = bs_new<GLSLProgramFactory>();
			GpuProgramCoreManager::instance().addFactory(mGLSLProgramFactory);
		}

		// Check for framebuffer object extension
		if(caps->hasCapability(RSC_FBO))
		{
			if(caps->hasCapability(RSC_HWRENDER_TO_TEXTURE))
			{
				// Create FBO manager
				GLRTTManager::startUp<GLRTTManager>();
			}
		}
		else
		{
			BS_EXCEPT(RenderingAPIException, "GPU doesn't support frame buffer objects. OpenGL versions lower than 3.0 are not supported.");
		}

		mFragmentTexOffset = 0;
		mVertexTexOffset = caps->getNumTextureUnits(GPT_FRAGMENT_PROGRAM);
		mGeometryTexOffset = mVertexTexOffset + caps->getNumTextureUnits(GPT_VERTEX_PROGRAM);

		UINT16 numCombinedTexUnits = caps->getNumCombinedTextureUnits();

		UINT32 totalNumTexUnits = caps->getNumTextureUnits(GPT_VERTEX_PROGRAM);
		totalNumTexUnits += caps->getNumTextureUnits(GPT_FRAGMENT_PROGRAM);
		totalNumTexUnits += caps->getNumTextureUnits(GPT_GEOMETRY_PROGRAM);
		totalNumTexUnits += caps->getNumTextureUnits(GPT_HULL_PROGRAM);
		totalNumTexUnits += caps->getNumTextureUnits(GPT_DOMAIN_PROGRAM);
		totalNumTexUnits += caps->getNumTextureUnits(GPT_COMPUTE_PROGRAM);

		if(totalNumTexUnits > numCombinedTexUnits)
			BS_EXCEPT(InternalErrorException, "Number of combined texture units less than the number of individual units!?");

		mNumTextureTypes = numCombinedTexUnits;
		mTextureTypes = bs_newN<GLenum>(mNumTextureTypes);
		for(UINT16 i = 0; i < numCombinedTexUnits; i++)
			mTextureTypes[i] = GL_TEXTURE_2D;

		mVertexUBOffset = 0;
		UINT32 totalNumUniformBlocks = caps->getNumGpuParamBlockBuffers(GPT_VERTEX_PROGRAM);
		mFragmentUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumGpuParamBlockBuffers(GPT_FRAGMENT_PROGRAM);
		mGeometryUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumGpuParamBlockBuffers(GPT_GEOMETRY_PROGRAM);
		mHullUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumGpuParamBlockBuffers(GPT_HULL_PROGRAM);
		mDomainUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumGpuParamBlockBuffers(GPT_DOMAIN_PROGRAM);
		mComputeUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumGpuParamBlockBuffers(GPT_COMPUTE_PROGRAM);

		UINT16 numCombinedUniformBlocks = caps->getNumCombinedGpuParamBlockBuffers();

		if(totalNumUniformBlocks > numCombinedUniformBlocks)
			BS_EXCEPT(InternalErrorException, "Number of combined uniform block buffers less than the number of individual per-stage buffers!?");

		TextureManager::startUp<GLTextureManager>(std::ref(*mGLSupport));
		TextureCoreManager::startUp<GLTextureCoreManager>(std::ref(*mGLSupport));
	}

	void GLRenderSystem::switchContext(GLContext *context)
	{
		// Unbind GPU programs and rebind to new context later, because
		// scene manager treat render system as ONE 'context' ONLY, and it
		// cached the GPU programs using state.
		unbindGpuProgram(GPT_VERTEX_PROGRAM);
		unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
		unbindGpuProgram(GPT_GEOMETRY_PROGRAM);
		unbindGpuProgram(GPT_HULL_PROGRAM);
		unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		// It's ready for switching
		if (mCurrentContext)
			mCurrentContext->endCurrent();

		mCurrentContext = context;
		mCurrentContext->setCurrent();

		// Must reset depth/colour write mask to according with user desired, otherwise,
		// clearFrameBuffer would be wrong because the value we are recorded may be
		// difference with the really state stored in GL context.
		glDepthMask(mDepthWrite);
		glColorMask(mColorWrite[0], mColorWrite[1], mColorWrite[2], mColorWrite[3]);
		glStencilMask(mStencilWriteMask);
	}

	RenderSystemCapabilities* GLRenderSystem::createRenderSystemCapabilities() const
	{
		RenderSystemCapabilities* rsc = bs_new<RenderSystemCapabilities>();

		rsc->setDriverVersion(mDriverVersion);
		const char* deviceName = (const char*)glGetString(GL_RENDERER);
		const char* vendorName = (const char*)glGetString(GL_VENDOR);
		rsc->setDeviceName(deviceName);
		rsc->setRenderSystemName(getName());

		// determine vendor
		if (strstr(vendorName, "NVIDIA"))
			rsc->setVendor(GPU_NVIDIA);
		else if (strstr(vendorName, "ATI"))
			rsc->setVendor(GPU_AMD);
		else if (strstr(vendorName, "AMD"))
			rsc->setVendor(GPU_AMD);
		else if (strstr(vendorName, "Intel"))
			rsc->setVendor(GPU_INTEL);
		else
			rsc->setVendor(GPU_UNKNOWN);

		// Check for hardware mipmapping support.
		if(GLEW_VERSION_1_4)
		{
			rsc->setCapability(RSC_AUTOMIPMAP);
		}

		// Check for Anisotropy support
		if (getGLSupport()->checkExtension("GL_EXT_texture_filter_anisotropic"))
		{
			rsc->setCapability(RSC_ANISOTROPY);
		}

		// Check for cube mapping
		if(GLEW_VERSION_1_3 || 
			getGLSupport()->checkExtension("GL_ARB_texture_cube_map") ||
			getGLSupport()->checkExtension("GL_EXT_texture_cube_map"))
		{
			rsc->setCapability(RSC_CUBEMAPPING);
		}

		// Point sprites
		if (GLEW_VERSION_2_0 || getGLSupport()->checkExtension("GL_ARB_point_sprite"))
		{
			rsc->setCapability(RSC_POINT_SPRITES);
		}

		// Check for hardware stencil support and set bit depth
		GLint stencil;
		glGetIntegerv(GL_STENCIL_BITS, &stencil);

		if(stencil)
		{
			rsc->setStencilBufferBitDepth(stencil);
		}

		if (GLEW_VERSION_2_0 || 
			(getGLSupport()->checkExtension("GL_ARB_shading_language_100") &&
			getGLSupport()->checkExtension("GL_ARB_shader_objects") &&
			getGLSupport()->checkExtension("GL_ARB_fragment_shader") &&
			getGLSupport()->checkExtension("GL_ARB_vertex_shader")))
		{
			rsc->addShaderProfile("glsl");
		}

		// Check if geometry shaders are supported
		if (GLEW_VERSION_2_0 &&
			getGLSupport()->checkExtension("GL_EXT_geometry_shader4"))
		{
			rsc->setCapability(RSC_GEOMETRY_PROGRAM);

			rsc->setGeometryProgramConstantBoolCount(0);
			rsc->setGeometryProgramConstantIntCount(0);

			GLint floatConstantCount = 0;
			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT, &floatConstantCount);
			rsc->setGeometryProgramConstantFloatCount(floatConstantCount);

			GLint maxOutputVertices;
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&maxOutputVertices);
			rsc->setGeometryProgramNumOutputVertices(maxOutputVertices);
		}

		//Check if render to vertex buffer (transform feedback in OpenGL)
		if (GLEW_VERSION_2_0 && getGLSupport()->checkExtension("GL_EXT_transform_feedback"))
		{
			rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
		}

		// Check for texture compression
		if (GLEW_VERSION_1_3 || getGLSupport()->checkExtension("GL_ARB_texture_compression"))
		{   
			rsc->setCapability(RSC_TEXTURE_COMPRESSION);

			// Check for dxt compression
			if (getGLSupport()->checkExtension("GL_EXT_texture_compression_s3tc"))
			{
				rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
			}

			// Check for vtc compression
			if (getGLSupport()->checkExtension("GL_NV_texture_compression_vtc"))
			{
				rsc->setCapability(RSC_TEXTURE_COMPRESSION_VTC);
			}
		}

		// As are user clipping planes
		rsc->setCapability(RSC_USER_CLIP_PLANES);

		// 2-sided stencil?
		if (GLEW_VERSION_2_0 || getGLSupport()->checkExtension("GL_EXT_stencil_two_side"))
		{
			rsc->setCapability(RSC_TWO_SIDED_STENCIL);
		}

		// stencil wrapping?
		if (GLEW_VERSION_1_4 || getGLSupport()->checkExtension("GL_EXT_stencil_wrap"))
		{
			rsc->setCapability(RSC_STENCIL_WRAP);
		}

		// Check for hardware occlusion support
		if (GLEW_VERSION_1_5 || getGLSupport()->checkExtension("GL_ARB_occlusion_query"))
		{
			rsc->setCapability(RSC_HWOCCLUSION);
		}

		// UBYTE4 always supported
		rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

		// Infinite far plane always supported
		rsc->setCapability(RSC_INFINITE_FAR_PLANE);

		// Check for non-power-of-2 texture support
		if (getGLSupport()->checkExtension("GL_ARB_texture_non_power_of_two"))
		{
			rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
		}

		// Check for Float textures
		if (getGLSupport()->checkExtension("GL_ATI_texture_float") || getGLSupport()->checkExtension("GL_ARB_texture_float"))
		{
			rsc->setCapability(RSC_TEXTURE_FLOAT);
		}

		// 3D textures should always be supported
		rsc->setCapability(RSC_TEXTURE_3D);

		// Check for framebuffer object extension
		if (getGLSupport()->checkExtension("GL_ARB_framebuffer_object"))
		{
			// Probe number of draw buffers
			// Only makes sense with FBO support, so probe here
			if (GLEW_VERSION_2_0 || getGLSupport()->checkExtension("GL_ARB_draw_buffers") || getGLSupport()->checkExtension("GL_ATI_draw_buffers"))
			{
				GLint buffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &buffers);
				rsc->setNumMultiRenderTargets(std::min<int>(buffers, (GLint)BS_MAX_MULTIPLE_RENDER_TARGETS));
				rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);

				rsc->setCapability(RSC_FBO);

			}
			rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
		}

		rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
		rsc->setCapability(RSC_PBUFFER);

		// Point size
		float ps;
		glGetFloatv(GL_POINT_SIZE_MAX, &ps);
		rsc->setMaxPointSize(ps);

		// Max number of fragment shader textures
		GLint units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units);
		rsc->setNumTextureUnits(GPT_FRAGMENT_PROGRAM, static_cast<UINT16>(units));

		// Max number of vertex shader textures
		GLint vUnits;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vUnits);
		rsc->setNumTextureUnits(GPT_VERTEX_PROGRAM, static_cast<UINT16>(vUnits));
		if (vUnits > 0)
		{
			rsc->setCapability(RSC_VERTEX_TEXTURE_FETCH);
		}

		GLint numUniformBlocks;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &numUniformBlocks);
		rsc->setNumGpuParamBlockBuffers(GPT_VERTEX_PROGRAM, numUniformBlocks);

		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &numUniformBlocks);
		rsc->setNumGpuParamBlockBuffers(GPT_FRAGMENT_PROGRAM, numUniformBlocks);

		if (mGLSupport->checkExtension("GL_ARB_geometry_shader4"))
		{
			GLint geomUnits;
			glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &geomUnits);
			rsc->setNumTextureUnits(GPT_GEOMETRY_PROGRAM, static_cast<UINT16>(geomUnits));

			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumGpuParamBlockBuffers(GPT_GEOMETRY_PROGRAM, numUniformBlocks);
		}

		if (mGLSupport->checkExtension("GL_ARB_tessellation_shader"))
		{
			rsc->setCapability(RSC_TESSELLATION_PROGRAM);

			glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumGpuParamBlockBuffers(GPT_HULL_PROGRAM, numUniformBlocks);

			glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumGpuParamBlockBuffers(GPT_DOMAIN_PROGRAM, numUniformBlocks);
		}

		if (mGLSupport->checkExtension("GL_ARB_compute_shader")) 
		{
			rsc->setCapability(RSC_COMPUTE_PROGRAM);

			GLint computeUnits;
			glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &computeUnits);
			rsc->setNumTextureUnits(GPT_COMPUTE_PROGRAM, static_cast<UINT16>(computeUnits));

			glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumGpuParamBlockBuffers(GPT_COMPUTE_PROGRAM, numUniformBlocks);
		}

		GLint combinedTexUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combinedTexUnits);
		rsc->setNumCombinedTextureUnits(static_cast<UINT16>(combinedTexUnits));

		GLint combinedUniformBlockUnits;
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &combinedUniformBlockUnits);
		rsc->setNumCombinedGpuParamBlockBuffers(static_cast<UINT16>(combinedUniformBlockUnits));

		// Mipmap LOD biasing

		if (mGLSupport->checkExtension("GL_EXT_texture_lod_bias"))
			rsc->setCapability(RSC_MIPMAP_LOD_BIAS);

		// Alpha to coverage?
		if (mGLSupport->checkExtension("GL_ARB_multisample"))
		{
			// Alpha to coverage always 'supported' when MSAA is available
			// although card may ignore it if it doesn't specifically support A2C
			rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
		}

		// Advanced blending operations
		if(GLEW_VERSION_2_0)
		{
			rsc->setCapability(RSC_ADVANCED_BLEND_OPERATIONS);
		}

		return rsc;
	}

	bool GLRenderSystem::checkForErrors() const
	{
		GLenum glErr = glGetError();
		bool errorsFound = false;
		String msg;
		while (glErr != GL_NO_ERROR)
		{
			const char* glerrStr = (const char*)gluErrorString(glErr);
			if (glerrStr)
			{
				msg += String(glerrStr);
			}
			glErr = glGetError();
			errorsFound = true;
		}

		if (errorsFound)
			LOGWRN("OpenGL error: " + msg);

		return errorsFound;
	}

	void GLRenderSystem::makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m)
	{
		UINT32 x = 0;
		for (UINT32 i = 0; i < 4; i++)
		{
			for (UINT32 j = 0; j < 4; j++)
			{
				gl_matrix[x] = m[j][i];
				x++;
			}
		}
	}

	void GLRenderSystem::applyViewport()
	{
		if (mActiveRenderTarget == nullptr)
			return;

		const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();

		// Calculate the "lower-left" corner of the viewport
		mViewportLeft = (UINT32)(rtProps.getWidth() * mViewportNorm.x);
		mViewportTop = (UINT32)(rtProps.getHeight() * mViewportNorm.y);
		mViewportWidth = (UINT32)(rtProps.getWidth() * mViewportNorm.width);
		mViewportHeight = (UINT32)(rtProps.getHeight() * mViewportNorm.height);

		if (rtProps.requiresTextureFlipping())
		{
			// Convert "upper-left" corner to "lower-left"
			mViewportTop = rtProps.getHeight() - (mViewportTop + mViewportHeight) - 1;
		}

		glViewport(mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight);

		// Configure the viewport clipping
		glScissor(mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight);
	}

	/************************************************************************/
	/* 								UTILITY		                     		*/
	/************************************************************************/
	float GLRenderSystem::getMinimumDepthInputValue()
	{
		return -1.0f;
	}

	float GLRenderSystem::getMaximumDepthInputValue()
	{
		return 1.0f;
	}

	float GLRenderSystem::getHorizontalTexelOffset()
	{
		return 0.0f;
	}

	float GLRenderSystem::getVerticalTexelOffset()
	{
		return 0.0f;
	}

	VertexElementType GLRenderSystem::getColorVertexElementType() const
	{
		return VET_COLOR_ABGR;
	}

	void GLRenderSystem::convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest)
	{
		dest = matrix;
	}

	void __stdcall openGlErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
	{
		if (type != GL_DEBUG_TYPE_PERFORMANCE && type != GL_DEBUG_TYPE_OTHER)
		{
			BS_EXCEPT(RenderingAPIException, "OpenGL error: " + String(message));
		}
	}
}