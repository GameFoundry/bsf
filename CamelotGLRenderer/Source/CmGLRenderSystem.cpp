/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in*
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.s
-----------------------------------------------------------------------------
*/


#include "CmGLRenderSystem.h"
#include "CmRenderSystem.h"
#include "CmCamera.h"
#include "CmGLTextureManager.h"
#include "CmGLVertexBuffer.h"
#include "CmGLIndexBuffer.h"
#include "CmGLUtil.h"
#include "CmGLSLGpuProgram.h"
#include "CmGLSLProgram.h"
#include "CmGLGpuProgramManager.h"
#include "CmException.h"
#include "CmGLSLExtSupport.h"
#include "CmGLOcclusionQuery.h"
#include "CmGLContext.h"
#include "CmAsyncOp.h"
#include "CmBlendState.h"
#include "CmRasterizerState.h"
#include "CmDepthStencilState.h"
#include "CmGLRenderTexture.h"
#include "CmGLRenderWindowManager.h"
#include "CmGLBuiltinMaterialManager.h"
#include "CmGLSLProgramPipelineManager.h"
#include "CmRenderStateManager.h"
#include "CmGpuParams.h"
#include "CmGLGpuParamBlock.h"
#include "CmDebug.h"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

// Convenience macro from ARB_vertex_buffer_object spec
#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

#if CM_THREAD_SUPPORT != 1
GLenum glewContextInit (CamelotEngine::GLSupport *glSupport);
#endif

namespace CamelotEngine 
{
	/************************************************************************/
	/* 								PUBLIC INTERFACE                   		*/
	/************************************************************************/

	GLRenderSystem::GLRenderSystem()
		: mDepthWrite(true),
		mGLSLProgramFactory(nullptr),
		mCgProgramFactory(nullptr),
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
		mCurrentDrawOperation(DOT_TRIANGLE_LIST)
	{
		// Get our GLSupport
		mGLSupport = CamelotEngine::getGLSupport();

		mViewMatrix = Matrix4::IDENTITY;

		mColourWrite[0] = mColourWrite[1] = mColourWrite[2] = mColourWrite[3] = true;

		mCurrentContext = 0;
		mMainContext = 0;

		mGLInitialised = false;

		mMinFilter = FO_LINEAR;
		mMipFilter = FO_POINT;

		mProgramPipelineManager = CM_NEW(GLSLProgramPipelineManager, GenAlloc) GLSLProgramPipelineManager();
	}

	GLRenderSystem::~GLRenderSystem()
	{
		// This needs to be called from the child class, since destroy_internal is virtual
		queueCommand(boost::bind(&GLRenderSystem::destroy_internal, this), true);
	}

	const String& GLRenderSystem::getName(void) const
	{
		static String strName("GLRenderSystem");
		return strName;
	}


	const String& GLRenderSystem::getShadingLanguageName() const
	{
		static String strName("glsl");
		return strName;
	}

	void GLRenderSystem::initialize_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		mGLSupport->start();
		RenderWindowManager::startUp(CM_NEW(GLRenderWindowManager, GenAlloc) GLRenderWindowManager(this));

		RenderStateManager::startUp(CM_NEW(RenderStateManager, GenAlloc) RenderStateManager());

		BuiltinMaterialManager::startUp(CM_NEW(GLBuiltinMaterialManager, GenAlloc) GLBuiltinMaterialManager());

		RenderSystem::initialize_internal();
	}

	void GLRenderSystem::destroy_internal()
	{
		RenderSystem::destroy_internal();

		// Deleting the GLSL program factory
		if (mGLSLProgramFactory)
		{
			// Remove from manager safely
			HighLevelGpuProgramManager::instance().removeFactory(mGLSLProgramFactory);
			CM_DELETE(mGLSLProgramFactory, GLSLProgramFactory, GenAlloc);
			mGLSLProgramFactory = nullptr;
		}

		// Deleting Cg GLSL program factory
		if (mCgProgramFactory)
		{
			// Remove from manager safely
			HighLevelGpuProgramManager::instance().removeFactory(mCgProgramFactory);
			CM_DELETE(mCgProgramFactory, CgProgramFactory, GenAlloc);
			mCgProgramFactory = nullptr;
		}

		// Deleting the GPU program manager and hardware buffer manager.  Has to be done before the mGLSupport->stop().
		GpuProgramManager::shutDown();
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

		TextureManager::shutDown();
		BuiltinMaterialManager::shutDown();
		RenderWindowManager::shutDown();
		RenderStateManager::shutDown();

		// There will be a new initial window and so forth, thus any call to test
		//  some params will access an invalid pointer, so it is best to reset
		//  the whole state.
		mGLInitialised = 0;

		if(mProgramPipelineManager != nullptr)
			CM_DELETE(mProgramPipelineManager, GLSLProgramPipelineManager, GenAlloc);

		if(mGLSupport)
			CM_DELETE(mGLSupport, GLSupport, GenAlloc);

		if(mTextureTypes != nullptr)
			CM_DELETE_ARRAY(mTextureTypes, GLenum, mNumTextureTypes, GenAlloc);
	}

	//---------------------------------------------------------------------
	void GLRenderSystem::bindGpuProgram(HGpuProgram prg)
	{
		THROW_IF_NOT_RENDER_THREAD;

		GpuProgramPtr bindingPrg = prg->getBindingDelegate();
		GLSLGpuProgramPtr glprg = std::static_pointer_cast<GLSLGpuProgram>(bindingPrg);

		switch (glprg->getType())
		{
		case GPT_VERTEX_PROGRAM:
			if (mCurrentVertexProgram != glprg)
			{
				unbindGpuProgram(glprg->getType());
				mCurrentVertexProgram = glprg;
			}
			break;

		case GPT_FRAGMENT_PROGRAM:
			if (mCurrentFragmentProgram != glprg)
			{
				unbindGpuProgram(glprg->getType());
				mCurrentFragmentProgram = glprg;
			}
			break;
		case GPT_GEOMETRY_PROGRAM:
			if (mCurrentGeometryProgram != glprg)
			{
				unbindGpuProgram(glprg->getType());
				mCurrentGeometryProgram = glprg;
			}
			break;
		case GPT_DOMAIN_PROGRAM:
			if (mCurrentDomainProgram != glprg)
			{
				unbindGpuProgram(glprg->getType());
				mCurrentDomainProgram = glprg;
			}
			break;
		case GPT_HULL_PROGRAM:
			if (mCurrentHullProgram != glprg)
			{
				unbindGpuProgram(glprg->getType());
				mCurrentHullProgram = glprg;
			}
			break;
		}

		RenderSystem::bindGpuProgram(prg);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		THROW_IF_NOT_RENDER_THREAD;

		setActiveProgram(gptype, nullptr);

		RenderSystem::unbindGpuProgram(gptype);
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::bindGpuParams(GpuProgramType gptype, GpuParamsPtr params)
	{
		THROW_IF_NOT_RENDER_THREAD;

		const GpuParamDesc& paramDesc = params->getParamDesc();
		GLSLGpuProgramPtr activeProgram = getActiveProgram(gptype);
		GLuint glProgram = activeProgram->getGLSLProgram()->getGLHandle();

		for(auto iter = paramDesc.textures.begin(); iter != paramDesc.textures.end(); ++iter)
		{
			HTexture texture = params->getTexture(iter->second.slot);

			if(!texture.isLoaded())
				setTexture(gptype, iter->second.slot, false, nullptr);
			else
				setTexture(gptype, iter->second.slot, true, texture.getInternalPtr());
		}

		UINT32 texUnit = 0;
		for(auto iter = paramDesc.samplers.begin(); iter != paramDesc.samplers.end(); ++iter)
		{
			HSamplerState& samplerState = params->getSamplerState(iter->second.slot);

			if(samplerState == nullptr)
				setSamplerState(gptype, iter->second.slot, SamplerState::getDefault());
			else
				setSamplerState(gptype, iter->second.slot, samplerState.getInternalPtr());

			glProgramUniform1i(glProgram, iter->second.slot, getGLTextureUnit(gptype, texUnit));

			texUnit++;
		}

		UINT32 blockBinding = 0;
		for(auto iter = paramDesc.paramBlocks.begin(); iter != paramDesc.paramBlocks.end(); ++iter)
		{
			if(iter->second.slot == 0)
				continue;

			GpuParamBlockPtr paramBlock = params->getParamBlock(iter->second.slot);
			if(paramBlock == nullptr)
				continue;

			GLGpuParamBlockPtr glParamBlock = std::static_pointer_cast<GLGpuParamBlock>(paramBlock);
			const GpuParamBlockBuffer* paramBlockBuffer = glParamBlock->getBindableBuffer();
			const GLGpuParamBlockBuffer* glParamBlockBuffer = static_cast<const GLGpuParamBlockBuffer*>(paramBlockBuffer);

			UINT32 globalBlockBinding = getGLUniformBlockBinding(gptype, blockBinding);
			glUniformBlockBinding(glProgram, iter->second.slot - 1, globalBlockBinding);
			glBindBufferRange(GL_UNIFORM_BUFFER, globalBlockBinding, glParamBlockBuffer->getGLHandle(), 0, glParamBlockBuffer->getSize());

			blockBinding++;
		}

		for(auto iter = paramDesc.params.begin(); iter != paramDesc.params.end(); ++iter)
		{
			const GpuParamDataDesc& paramDesc = iter->second;

			GpuParamBlockPtr paramBlock = params->getParamBlock(paramDesc.paramBlockSlot);
			const GpuParamBlockBuffer* paramBlockBuffer = paramBlock->getBindableBuffer();

			if(paramDesc.paramBlockSlot != 0) // 0 means uniforms are not in a block
				continue;

			const UINT8* ptrData = paramBlockBuffer->getDataPtr(paramDesc.cpuMemOffset * sizeof(UINT32));

			switch(paramDesc.type)
			{
			case GCT_FLOAT1:
				glProgramUniform1fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GCT_FLOAT2:
				glProgramUniform2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GCT_FLOAT3:
				glProgramUniform3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GCT_FLOAT4:
				glProgramUniform4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_2X2:
				glProgramUniformMatrix2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_2X3:
				glProgramUniformMatrix2x3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_2X4:
				glProgramUniformMatrix2x4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_3X2:
				glProgramUniformMatrix3x2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_3X3:
				glProgramUniformMatrix3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_3X4:
				glProgramUniformMatrix3x4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_4X2:
				glProgramUniformMatrix4x2fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_4X3:
				glProgramUniformMatrix4x3fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_MATRIX_4X4:
				glProgramUniformMatrix4fv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, 
					GL_FALSE, (GLfloat*)ptrData);
				break;
			case GCT_INT1:
				glProgramUniform1iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GCT_INT2:
				glProgramUniform2iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GCT_INT3:
				glProgramUniform3iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GCT_INT4:
				glProgramUniform4iv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLint*)ptrData);
				break;
			case GPDT_BOOL:
				glProgramUniform1uiv(glProgram, paramDesc.gpuMemOffset, paramDesc.arraySize, (GLuint*)ptrData);
				break;
			case GCT_UNKNOWN:
				break;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		THROW_IF_NOT_RENDER_THREAD;

		unit = getGLTextureUnit(gptype, unit);

		GLTexturePtr tex = std::static_pointer_cast<GLTexture>(texPtr);

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

			// bind zero texture
			glBindTexture(GL_TEXTURE_2D, 0); 
		}

		activateGLTextureUnit(0);
	}
	//-----------------------------------------------------------------------
	void GLRenderSystem::setSamplerState(GpuProgramType gptype, UINT16 unit, const SamplerStatePtr& state)
	{
		THROW_IF_NOT_RENDER_THREAD;

		unit = getGLTextureUnit(gptype, unit);

		// Set texture layer filtering
		setTextureFiltering(unit, FT_MIN, state->getTextureFiltering(FT_MIN));
		setTextureFiltering(unit, FT_MAG, state->getTextureFiltering(FT_MAG));
		setTextureFiltering(unit, FT_MIP, state->getTextureFiltering(FT_MIP));

		// Set texture anisotropy
		setTextureAnisotropy(unit, state->getTextureAnisotropy());

		// Set mipmap biasing
		setTextureMipmapBias(unit, state->getTextureMipmapBias());

		// Texture addressing mode
		const UVWAddressingMode& uvw = state->getTextureAddressingMode();
		setTextureAddressingMode(unit, uvw);

		// Set border color
		setTextureBorderColor(unit, state->getBorderColor());
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setBlendState(const BlendStatePtr& blendState)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// Alpha to coverage
		setAlphaToCoverage(blendState->getAlphaToCoverageEnabled());

		// Blend states
		// DirectX 9 doesn't allow us to specify blend state per render target, so we just use the first one.
		if(blendState->getBlendEnabled(0))
		{
			setSceneBlending(blendState->getSrcBlend(0), blendState->getDstBlend(0), blendState->getAlphaSrcBlend(0), blendState->getAlphaDstBlend(0)
				, blendState->getBlendOperation(0), blendState->getAlphaBlendOperation(0));
		}
		else
		{
			setSceneBlending(BF_ONE, BF_ZERO, BO_ADD);
		}

		// Color write mask
		UINT8 writeMask = blendState->getRenderTargetWriteMask(0);
		setColorBufferWriteEnabled((writeMask & 0x1) != 0, (writeMask & 0x2) != 0, (writeMask & 0x4) != 0, (writeMask & 0x8) != 0);
	}
	//----------------------------------------------------------------------
	void GLRenderSystem::setRasterizerState(const RasterizerStatePtr& rasterizerState)
	{
		THROW_IF_NOT_RENDER_THREAD;

		setDepthBias((float)rasterizerState->getDepthBias(), rasterizerState->getSlopeScaledDepthBias());

		setCullingMode(rasterizerState->getCullMode());

		setPolygonMode(rasterizerState->getPolygonMode());

		setScissorTestEnable(rasterizerState->getScissorEnable());
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// Set stencil buffer options
		setStencilCheckEnabled(depthStencilState->getStencilEnable());

		setStencilBufferOperations(depthStencilState->getStencilFrontFailOp(), depthStencilState->getStencilFrontZFailOp(), depthStencilState->getStencilFrontPassOp(), true);
		setStencilBufferFunc(depthStencilState->getStencilFrontCompFunc(), depthStencilState->getStencilReadMask(), true);

		setStencilBufferOperations(depthStencilState->getStencilBackFailOp(), depthStencilState->getStencilBackZFailOp(), depthStencilState->getStencilBackPassOp(), false);
		setStencilBufferFunc(depthStencilState->getStencilBackCompFunc(), depthStencilState->getStencilReadMask(), false);

		setStencilBufferWriteMask(depthStencilState->getStencilWriteMask());

		// Set depth buffer options
		setDepthBufferCheckEnabled(depthStencilState->getDepthReadEnable());
		setDepthBufferWriteEnabled(depthStencilState->getDepthWriteEnable());
		setDepthBufferFunction(depthStencilState->getDepthComparisonFunc());	

		// Set stencil ref value
		setStencilRefValue(stencilRefValue);
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setViewport(ViewportPtr& vp)
	{
		THROW_IF_NOT_RENDER_THREAD;

		assert(vp != nullptr);

		RenderTargetPtr target;
		target = vp->getTarget();
		setRenderTarget(target);

		// Calculate the "lower-left" corner of the viewport
		mViewportWidth = vp->getWidth();
		mViewportHeight = vp->getHeight();
		mViewportLeft = vp->getLeft();
		mViewportTop = vp->getTop();

		if (!target->requiresTextureFlipping())
		{
			// Convert "upper-left" corner to "lower-left"
			mViewportTop = target->getHeight() - mViewportHeight - mViewportTop;
		}
		glViewport(mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight);

		// Configure the viewport clipping
		glScissor(mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setRenderTarget(RenderTargetPtr target)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mActiveRenderTarget = target;

		// Switch context if different from current one
		GLContext *newContext = 0;
		target->getCustomAttribute("GLCONTEXT", &newContext);
		if(newContext && mCurrentContext != newContext) 
		{
			switchContext(newContext);
		}

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
			if (target->isHardwareGammaEnabled())
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
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::beginFrame(void)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// Activate the viewport clipping
		glEnable(GL_SCISSOR_TEST);
	}

	//-----------------------------------------------------------------------------
	void GLRenderSystem::endFrame(void)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// Deactivate the viewport clipping.
		glDisable(GL_SCISSOR_TEST);
		// unbind GPU programs at end of frame
		// this is mostly to avoid holding bound programs that might get deleted
		// outside via the resource manager
		unbindGpuProgram(GPT_VERTEX_PROGRAM);
		unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setVertexBuffer(UINT32 index, const VertexBufferPtr& buffer)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mBoundVertexBuffers[index] = buffer;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setVertexDeclaration(VertexDeclarationPtr vertexDeclaration)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mBoundVertexDeclaration = vertexDeclaration;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setDrawOperation(DrawOperationType op)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mCurrentDrawOperation = op;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setIndexBuffer(const IndexBufferPtr& buffer)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mBoundIndexBuffer = buffer;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::draw(UINT32 vertexCount)
	{
		// Find the correct type to render
		GLint primType = getGLDrawMode();
		beginDraw();

		glDrawArrays(primType, 0, vertexCount);

		endDraw();
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexCount)
	{
		if(mBoundIndexBuffer == nullptr)
		{
			LOGWRN("Cannot draw indexed because index buffer is not set.");
			return;
		}

		// Find the correct type to render
		GLint primType = getGLDrawMode();
		beginDraw();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
			static_cast<GLIndexBuffer*>(mBoundIndexBuffer.get())->getGLBufferId());

		void* pBufferData = VBO_BUFFER_OFFSET(startIndex * mBoundIndexBuffer->getIndexSize());

		GLenum indexType = (mBoundIndexBuffer->getType() == IndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		glDrawElements(primType, indexCount, indexType, 0);

		endDraw();
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mScissorTop = top;
		mScissorBottom = bottom;
		mScissorLeft = left;
		mScissorRight = right;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::clear(RenderTargetPtr target, unsigned int buffers, 
		const Color& colour, float depth, unsigned short stencil)
	{
		THROW_IF_NOT_RENDER_THREAD;

		RenderTargetPtr previousRenderTarget = mActiveRenderTarget;
		if(target != mActiveRenderTarget)
		{
			previousRenderTarget = mActiveRenderTarget;
			setRenderTarget(target);
		}

		bool colourMask = !mColourWrite[0] || !mColourWrite[1] 
		|| !mColourWrite[2] || !mColourWrite[3]; 

		GLbitfield flags = 0;
		if (buffers & FBT_COLOUR)
		{
			flags |= GL_COLOR_BUFFER_BIT;
			// Enable buffer for writing if it isn't
			if (colourMask)
			{
				glColorMask(true, true, true, true);
			}
			glClearColor(colour.r, colour.g, colour.b, colour.a);
		}
		if (buffers & FBT_DEPTH)
		{
			flags |= GL_DEPTH_BUFFER_BIT;
			// Enable buffer for writing if it isn't
			if (!mDepthWrite)
			{
				glDepthMask( GL_TRUE );
			}
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
		if (scissorTestEnabled)
		{
			glDisable(GL_SCISSOR_TEST);
		}

		// Clear buffers
		glClear(flags);

		// Restore scissor test
		if (scissorTestEnabled)
		{
			glEnable(GL_SCISSOR_TEST);
		}

		// Reset buffer write state
		if (!mDepthWrite && (buffers & FBT_DEPTH))
		{
			glDepthMask( GL_FALSE );
		}
		if (colourMask && (buffers & FBT_COLOUR))
		{
			glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
		}
		if (buffers & FBT_STENCIL)
		{
			glStencilMask(mStencilWriteMask);
		}

		if(target != previousRenderTarget)
		{
			setRenderTarget(previousRenderTarget);
		}
	}











	/************************************************************************/
	/* 								PRIVATE		                     		*/
	/************************************************************************/

	//-----------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTextureBorderColor(UINT16 stage, const Color& colour)
	{
		GLfloat border[4] = { colour.r, colour.g, colour.b, colour.a };
		if (activateGLTextureUnit(stage))
		{
			glTexParameterfv( mTextureTypes[stage], GL_TEXTURE_BORDER_COLOR, border);
			activateGLTextureUnit(0);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTextureMipmapBias(UINT16 stage, float bias)
	{
		if (mCurrentCapabilities->hasCapability(RSC_MIPMAP_LOD_BIAS))
		{
			if (activateGLTextureUnit(stage))
			{
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, bias);
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
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setSceneBlending(
		BlendFactor sourceFactor, BlendFactor destFactor, 
		BlendFactor sourceFactorAlpha, BlendFactor destFactorAlpha,
		BlendOperation op, BlendOperation alphaOp )
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
	//-----------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------
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
	//---------------------------------------------------------------------
	void GLRenderSystem::setScissorTestEnable(bool enable)
	{
		// If request texture flipping, use "upper-left", otherwise use "lower-left"
		bool flipping = mActiveRenderTarget->requiresTextureFlipping();
		//  GL measures from the bottom, not the top
		UINT32 targetHeight = mActiveRenderTarget->getHeight();
		// Calculate the "lower-left" corner of the viewport
		GLsizei x = 0, y = 0, w = 0, h = 0;

		if (enable)
		{
			glEnable(GL_SCISSOR_TEST);
			// GL uses width / height rather than right / bottom
			x = mScissorLeft;
			if (flipping)
				y = mScissorTop;
			else
				y = targetHeight - mScissorBottom;
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
			if (flipping)
				y = mViewportTop;
			else
				y = targetHeight - mViewportTop - h;

			glScissor(x, y, w, h);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setCullingMode(CullingMode mode)
	{
		mCullingMode = mode;
		GLenum cullMode;

		switch( mode )
		{
		case CULL_NONE:
			glDisable( GL_CULL_FACE );
			return;
		default:
		case CULL_CLOCKWISE:
			if (mActiveRenderTarget && 
				((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
				(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
			{
				cullMode = GL_FRONT;
			}
			else
			{
				cullMode = GL_BACK;
			}
			break;
		case CULL_COUNTERCLOCKWISE:
			if (mActiveRenderTarget && 
				((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
				(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
			{
				cullMode = GL_BACK;
			}
			else
			{
				cullMode = GL_FRONT;
			}
			break;
		}

		glEnable( GL_CULL_FACE );
		glCullFace( cullMode );
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		setDepthBufferCheckEnabled(depthTest);
		setDepthBufferWriteEnabled(depthWrite);
		setDepthBufferFunction(depthFunction);
	}
	//-----------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		GLboolean flag = enabled ? GL_TRUE : GL_FALSE;
		glDepthMask( flag );  
		// Store for reference in _beginFrame
		mDepthWrite = enabled;
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferFunction(CompareFunction func)
	{
		glDepthFunc(convertCompareFunction(func));
	}
	//-----------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		glColorMask(red, green, blue, alpha);
		// record this
		mColourWrite[0] = red;
		mColourWrite[1] = blue;
		mColourWrite[2] = green;
		mColourWrite[3] = alpha;
	}
	//---------------------------------------------------------------------
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
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilCheckEnabled(bool enabled)
	{
		if (enabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilBufferOperations(StencilOperation stencilFailOp,
		StencilOperation depthFailOp, StencilOperation passOp, bool front)
	{
		if (front)
		{
			glStencilOpSeparate(GL_FRONT, 
				convertStencilOp(stencilFailOp, mInvertVertexWinding),
				convertStencilOp(depthFailOp, mInvertVertexWinding), 
				convertStencilOp(passOp, mInvertVertexWinding));
		}
		else
		{
			glStencilOpSeparate(GL_BACK, 
				convertStencilOp(stencilFailOp, !mInvertVertexWinding), 
				convertStencilOp(depthFailOp, !mInvertVertexWinding), 
				convertStencilOp(passOp, !mInvertVertexWinding));
		}
	}
	//---------------------------------------------------------------------
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
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilBufferWriteMask(UINT32 mask)
	{
		mStencilWriteMask = mask;
		glStencilMask(mask);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilRefValue(UINT32 refValue)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mStencilRefValue = refValue;

		glStencilFuncSeparate(GL_FRONT, convertCompareFunction(mStencilCompareFront), mStencilRefValue, mStencilReadMask);
		glStencilFuncSeparate(GL_BACK, convertCompareFunction(mStencilCompareBack), mStencilRefValue, mStencilReadMask);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setTextureFiltering(UINT16 unit, 
		FilterType ftype, FilterOptions fo)
	{
		if (!activateGLTextureUnit(unit))
			return;
		switch(ftype)
		{
		case FT_MIN:
			mMinFilter = fo;
			// Combine with existing mip filter
			glTexParameteri(
				mTextureTypes[unit],
				GL_TEXTURE_MIN_FILTER, 
				getCombinedMinMipFilter());
			break;
		case FT_MAG:
			switch (fo)
			{
			case FO_ANISOTROPIC: // GL treats linear and aniso the same
			case FO_LINEAR:
				glTexParameteri(
					mTextureTypes[unit],
					GL_TEXTURE_MAG_FILTER, 
					GL_LINEAR);
				break;
			case FO_POINT:
			case FO_NONE:
				glTexParameteri(
					mTextureTypes[unit],
					GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
				break;
			}
			break;
		case FT_MIP:
			mMipFilter = fo;
			// Combine with existing min filter
			glTexParameteri(
				mTextureTypes[unit],
				GL_TEXTURE_MIN_FILTER, 
				getCombinedMinMipFilter());
			break;
		}

		activateGLTextureUnit(0);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy)
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
		if (_getCurrentAnisotropy(unit) != maxAnisotropy)
			glTexParameterf(mTextureTypes[unit], GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)maxAnisotropy);

		activateGLTextureUnit(0);
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
	{
		// A note on GL user clipping:
		// When an ARB vertex program is enabled in GL, user clipping is completely
		// disabled. There is no way around this, it's just turned off.
		// When using GLSL, user clipping can work but you have to include a 
		// glClipVertex command in your vertex shader. 
		// Thus the planes set here may not actually be respected.


		size_t i = 0;
		size_t numClipPlanes;
		GLdouble clipPlane[4];

		// Save previous modelview
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		// just load view matrix (identity world)
		GLfloat mat[16];
		makeGLMatrix(mat, mViewMatrix);
		glLoadMatrixf(mat);

		numClipPlanes = clipPlanes.size();
		for (i = 0; i < numClipPlanes; ++i)
		{
			GLenum clipPlaneId = static_cast<GLenum>(GL_CLIP_PLANE0 + i);
			const Plane& plane = clipPlanes[i];

			if (i >= 6/*GL_MAX_CLIP_PLANES*/)
			{
				CM_EXCEPT(RenderingAPIException, "Unable to set clip plane");
			}

			clipPlane[0] = plane.normal.x;
			clipPlane[1] = plane.normal.y;
			clipPlane[2] = plane.normal.z;
			clipPlane[3] = plane.d;

			glClipPlane(clipPlaneId, clipPlane);
			glEnable(clipPlaneId);
		}

		// disable remaining clip planes
		for ( ; i < 6/*GL_MAX_CLIP_PLANES*/; ++i)
		{
			glDisable(static_cast<GLenum>(GL_CLIP_PLANE0 + i));
		}

		// restore matrices
		glPopMatrix();
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::oneTimeContextInitialization()
	{
		if (GLEW_VERSION_1_2)
		{
		// Set nicer lighting model -- d3d9 has this by default
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);        
		}
		if (GLEW_VERSION_1_4)
		{
		glEnable(GL_COLOR_SUM);
		glDisable(GL_DITHER);
		}

		// Check for FSAA
		// Enable the extension if it was enabled by the GLSupport
		if (mGLSupport->checkExtension("GL_ARB_multisample"))
		{
			int fsaa_active = false;
			glGetIntegerv(GL_SAMPLE_BUFFERS_ARB,(GLint*)&fsaa_active);
			if(fsaa_active)
			{
				glEnable(GL_MULTISAMPLE_ARB);
			}            
		}
	}
	//---------------------------------------------------------------------
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

		// Check if the context has already done one-time initialisation
		if(!mCurrentContext->getInitialized()) 
		{
			oneTimeContextInitialization();
			mCurrentContext->setInitialized();
		}

		// Must reset depth/colour write mask to according with user desired, otherwise,
		// clearFrameBuffer would be wrong because the value we are recorded may be
		// difference with the really state stored in GL context.
		glDepthMask(mDepthWrite);
		glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
		glStencilMask(mStencilWriteMask);

	}
	//---------------------------------------------------------------------
	void GLRenderSystem::registerContext(GLContext* context)
	{
		if (!mGLInitialised) 
		{                
			// set up glew and GLSupport
			initialiseContext(context);

			checkForErrors();

			std::vector<CamelotEngine::String> tokens = StringUtil::split(mGLSupport->getGLVersion(), ".");

			if (!tokens.empty())
			{
				mDriverVersion.major = parseInt(tokens[0]);
				if (tokens.size() > 1)
					mDriverVersion.minor = parseInt(tokens[1]);
				if (tokens.size() > 2)
					mDriverVersion.release = parseInt(tokens[2]); 
			}
			mDriverVersion.build = 0;
			// Initialise GL after the first window has been created
			// TODO: fire this from emulation options, and don't duplicate float and Current capabilities
			mCurrentCapabilities = createRenderSystemCapabilities();
			checkForErrors();

			initialiseFromRenderSystemCapabilities(mCurrentCapabilities);
			checkForErrors();

			// Initialise the main context
			oneTimeContextInitialization();
			checkForErrors();

			if(mCurrentContext)
			{
				mCurrentContext->setInitialized();
				checkForErrors();
			}
		}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::unregisterContext(GLContext *context)
	{
		if(mCurrentContext == context) {
			// Change the context to something else so that a valid context
			// remains active. When this is the main context being unregistered,
			// we set the main context to 0.
			if(mCurrentContext != mMainContext) {
				switchContext(mMainContext);
			} else {
				/// No contexts remain
				mCurrentContext->endCurrent();
				mCurrentContext = 0;
				mMainContext = 0;
			}
		}
	}
	//---------------------------------------------------------------------
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
	//---------------------------------------------------------------------
	GLfloat GLRenderSystem::_getCurrentAnisotropy(UINT16 unit)
	{
		GLfloat curAniso = 0;
		glGetTexParameterfv(mTextureTypes[unit], 
			GL_TEXTURE_MAX_ANISOTROPY_EXT, &curAniso);
		return curAniso ? curAniso : 1;
	}
	//---------------------------------------------------------------------
	GLuint GLRenderSystem::getCombinedMinMipFilter(void) const
	{
		switch(mMinFilter)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// linear min, linear mip
				return GL_LINEAR_MIPMAP_LINEAR;
			case FO_POINT:
				// linear min, point mip
				return GL_LINEAR_MIPMAP_NEAREST;
			case FO_NONE:
				// linear min, no mip
				return GL_LINEAR;
			}
			break;
		case FO_POINT:
		case FO_NONE:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// nearest min, linear mip
				return GL_NEAREST_MIPMAP_LINEAR;
			case FO_POINT:
				// nearest min, point mip
				return GL_NEAREST_MIPMAP_NEAREST;
			case FO_NONE:
				// nearest min, no mip
				return GL_NEAREST;
			}
			break;
		}

		// should never get here
		return 0;

	}
	//---------------------------------------------------------------------
	GLint GLRenderSystem::convertStencilOp(StencilOperation op, bool invert) const
	{
		switch(op)
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
	//---------------------------------------------------------------------
	GLint GLRenderSystem::convertCompareFunction(CompareFunction func) const
	{
		switch(func)
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
		// to keep compiler happy
		return GL_ALWAYS;
	}
	//-----------------------------------------------------------------------------
	String GLRenderSystem::getErrorDescription(long errCode) const
	{
		const GLubyte *errString = gluErrorString (errCode);
		return (errString != 0) ? String((const char*) errString) : StringUtil::BLANK;
	}
	//-----------------------------------------------------------------------------
	GLint GLRenderSystem::getBlendMode(BlendFactor blendMode) const
	{
		switch(blendMode)
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
		// to keep compiler happy
		return GL_ONE;
	}
	//-----------------------------------------------------------------------------
	GLint GLRenderSystem::getTextureAddressingMode(
		TextureAddressingMode tam) const
	{
		switch(tam)
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
	//-----------------------------------------------------------------------------
	void GLRenderSystem::makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m)
	{
		size_t x = 0;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				gl_matrix[x] = m[j][i];
				x++;
			}
		}
	}
	//-----------------------------------------------------------------------
	void GLRenderSystem::beginDraw()
	{
		if(mDrawCallInProgress)
			CM_EXCEPT(InternalErrorException, "Calling beginDraw without finishing previous draw call. Please call endDraw().");

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

		void* pBufferData = 0;

		const VertexDeclaration::VertexElementList& decl = mBoundVertexDeclaration->getElements();
		VertexDeclaration::VertexElementList::const_iterator elem, elemEnd;
		elemEnd = decl.end();

		const VertexDeclaration::VertexElementList& inputAttributes = mCurrentVertexProgram->getGLSLProgram()->getInputAttributes().getElements();

		for (elem = decl.begin(); elem != elemEnd; ++elem)
		{
			auto iterFind = mBoundVertexBuffers.find(elem->getSource());

			if(iterFind == mBoundVertexBuffers.end() || iterFind->second == nullptr)
				continue; // skip unbound elements

			VertexBufferPtr vertexBuffer = iterFind->second;

			bool foundSemantic = false; 
			GLint attribLocation = 0;
			for(auto iter = inputAttributes.begin(); iter != inputAttributes.end(); ++iter)
			{
				if(iter->getSemantic() == elem->getSemantic() && iter->getIndex() == elem->getIndex())
				{
					foundSemantic = true;
					attribLocation = iter->getOffset();
					break;
				}
			}

			if(!foundSemantic) // Shader needs to have a matching input attribute, otherwise we skip it
				continue;

			// TODO - We might also want to check the size of input and buffer, and make sure they match? Or does OpenGL handle that internally?

			glBindBuffer(GL_ARRAY_BUFFER, static_cast<const GLVertexBuffer*>(vertexBuffer.get())->getGLBufferId());
			pBufferData = VBO_BUFFER_OFFSET(elem->getOffset());

			unsigned int i = 0;
			VertexElementSemantic sem = elem->getSemantic();

			unsigned short typeCount = VertexElement::getTypeCount(elem->getType());
			GLboolean normalised = GL_FALSE;
			switch(elem->getType())
			{
			case VET_COLOR:
			case VET_COLOR_ABGR:
			case VET_COLOR_ARGB:
				normalised = GL_TRUE;
				break;
			default:
				break;
			};

			glVertexAttribPointerARB(
				attribLocation,
				typeCount, 
				GLHardwareBufferManager::getGLType(elem->getType()), 
				normalised, 
				static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
				pBufferData);

			glEnableVertexAttribArray(attribLocation);

			mBoundAttributes.push_back(attribLocation);
		}
	}
	//-----------------------------------------------------------------------
	void GLRenderSystem::endDraw()
	{
		if(!mDrawCallInProgress)
			return;

		mDrawCallInProgress = false;

		// unbind any custom attributes
		for (auto ai = mBoundAttributes.begin(); ai != mBoundAttributes.end(); ++ai)
		{
			glDisableVertexAttribArray(*ai); 
		}

		glColor4f(1,1,1,1);
	}
	//-----------------------------------------------------------------------
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

		if(errorsFound)
			LOGWRN("OpenGL error: " + msg);

		return errorsFound;
	}
	//-----------------------------------------------------------------------
	GLint GLRenderSystem::getGLDrawMode() const
	{
		GLint primType;
		//Use adjacency if there is a geometry program and it requested adjacency info
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
	//-----------------------------------------------------------------------
	void GLRenderSystem::initialiseContext(GLContext* primary)
	{
		// Set main and current context
		mMainContext = primary;
		mCurrentContext = mMainContext;

		// Set primary context as active
		if(mCurrentContext)
			mCurrentContext->setCurrent();

		// Setup GLSupport
		mGLSupport->initialiseExtensions();

		// Get extension function pointers
#if CM_THREAD_SUPPORT != 1
		glewContextInit(mGLSupport);
#endif
	}
	void GLRenderSystem::initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps)
	{
		if(caps->getRenderSystemName() != getName())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Trying to initialize GLRenderSystem from RenderSystemCapabilities that do not support OpenGL");
		}

		if(caps->hasCapability(RSC_GL1_5_NOVBO))
		{
			// Assign ARB functions same to GL 1.5 version since
			// interface identical
			glBindBufferARB = glBindBuffer;
			glBufferDataARB = glBufferData;
			glBufferSubDataARB = glBufferSubData;
			glDeleteBuffersARB = glDeleteBuffers;
			glGenBuffersARB = glGenBuffers;
			glGetBufferParameterivARB = glGetBufferParameteriv;
			glGetBufferPointervARB = glGetBufferPointerv;
			glGetBufferSubDataARB = glGetBufferSubData;
			glIsBufferARB = glIsBuffer;
			glMapBufferARB = glMapBuffer;
			glUnmapBufferARB = glUnmapBuffer;
		}

		HardwareBufferManager::startUp(CM_NEW(GLHardwareBufferManager, GenAlloc) GLHardwareBufferManager);
		checkForErrors();

		// GPU Program Manager setup
		GpuProgramManager::startUp(CM_NEW(GLGpuProgramManager, GenAlloc) GLGpuProgramManager());
		checkForErrors();

		if(caps->isShaderProfileSupported("glsl"))
		{
			mGLSLProgramFactory = CM_NEW(GLSLProgramFactory, GenAlloc) GLSLProgramFactory();
			HighLevelGpuProgramManager::instance().addFactory(mGLSLProgramFactory);
			checkForErrors();
		}

		if(caps->isShaderProfileSupported("cg"))
		{
			mCgProgramFactory = CM_NEW(CgProgramFactory, GenAlloc) CgProgramFactory();
			HighLevelGpuProgramManager::instance().addFactory(mCgProgramFactory);
			checkForErrors();
		}

		if(caps->hasCapability(RSC_HWOCCLUSION))
		{
			if(caps->hasCapability(RSC_GL1_5_NOHWOCCLUSION))
			{
				// Assign ARB functions same to GL 1.5 version since
				// interface identical
				glBeginQueryARB = glBeginQuery;
				glDeleteQueriesARB = glDeleteQueries;
				glEndQueryARB = glEndQuery;
				glGenQueriesARB = glGenQueries;
				glGetQueryObjectivARB = glGetQueryObjectiv;
				glGetQueryObjectuivARB = glGetQueryObjectuiv;
				glGetQueryivARB = glGetQueryiv;
				glIsQueryARB = glIsQuery;
			}
		}

		// Check for framebuffer object extension
		if(caps->hasCapability(RSC_FBO))
		{
			// Before GL version 2.0, we need to get one of the extensions
			if(caps->hasCapability(RSC_FBO_ARB))
				GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersARB;
			else if(caps->hasCapability(RSC_FBO_ATI))
				GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersATI;

			if(caps->hasCapability(RSC_HWRENDER_TO_TEXTURE))
			{
				// Create FBO manager
				GLRTTManager::startUp(CM_NEW(GLRTTManager, GenAlloc) GLRTTManager());
				checkForErrors();
			}
		}
		else
		{
			CM_EXCEPT(RenderingAPIException, "GPU doesn't support frame buffer objects. OpenGL versions lower than 3.0 are not supported.");
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
			CM_EXCEPT(InternalErrorException, "Number of combined texture units less than the number of individual units!?");

		mNumTextureTypes = numCombinedTexUnits;
		mTextureTypes = CM_NEW_ARRAY(GLenum, mNumTextureTypes, GenAlloc);
		for(UINT16 i = 0; i < numCombinedTexUnits; i++)
			mTextureTypes[i] = 0;

		mVertexUBOffset = 0;
		UINT32 totalNumUniformBlocks = caps->getNumUniformBlockBuffers(GPT_VERTEX_PROGRAM);
		mFragmentUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumUniformBlockBuffers(GPT_FRAGMENT_PROGRAM);
		mGeometryUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumUniformBlockBuffers(GPT_GEOMETRY_PROGRAM);
		mHullUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumUniformBlockBuffers(GPT_HULL_PROGRAM);
		mDomainUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumUniformBlockBuffers(GPT_DOMAIN_PROGRAM);
		mComputeUBOffset = totalNumUniformBlocks;
		totalNumUniformBlocks += caps->getNumUniformBlockBuffers(GPT_COMPUTE_PROGRAM);

		UINT16 numCombinedUniformBlocks = caps->getNumCombinedUniformBlockBuffers();

		if(totalNumUniformBlocks > numCombinedUniformBlocks)
			CM_EXCEPT(InternalErrorException, "Number of combined uniform block buffers less than the number of individual per-stage buffers!?");

		/// Create the texture manager        
		TextureManager::startUp(CM_NEW(GLTextureManager, GenAlloc) GLTextureManager(*mGLSupport)); 
		checkForErrors();

		mGLInitialised = true;
	}

	RenderSystemCapabilities* GLRenderSystem::createRenderSystemCapabilities() const
	{
		RenderSystemCapabilities* rsc = CM_NEW(RenderSystemCapabilities, GenAlloc) RenderSystemCapabilities();

		rsc->setCategoryRelevant(CAPS_CATEGORY_GL, true);
		rsc->setDriverVersion(mDriverVersion);
		const char* deviceName = (const char*)glGetString(GL_RENDERER);
		const char* vendorName = (const char*)glGetString(GL_VENDOR);
		rsc->setDeviceName(deviceName);
		rsc->setRenderSystemName(getName());

		// determine vendor
		if (strstr(vendorName, "NVIDIA"))
			rsc->setVendor(GPU_NVIDIA);
		else if (strstr(vendorName, "ATI"))
			rsc->setVendor(GPU_ATI);
		else if (strstr(vendorName, "Intel"))
			rsc->setVendor(GPU_INTEL);
		else if (strstr(vendorName, "S3"))
			rsc->setVendor(GPU_S3);
		else if (strstr(vendorName, "Matrox"))
			rsc->setVendor(GPU_MATROX);
		else if (strstr(vendorName, "3DLabs"))
			rsc->setVendor(GPU_3DLABS);
		else if (strstr(vendorName, "SiS"))
			rsc->setVendor(GPU_SIS);
		else
			rsc->setVendor(GPU_UNKNOWN);

		// Supports fixed-function
		rsc->setCapability(RSC_FIXED_FUNCTION);

		// Check for hardware mipmapping support.
		if(GLEW_VERSION_1_4 || GLEW_SGIS_generate_mipmap)
		{
			bool disableAutoMip = false;
#if CM_PLATFORM == CM_PLATFORM_APPLE || CM_PLATFORM == CM_PLATFORM_LINUX
			// Apple & Linux ATI drivers have faults in hardware mipmap generation
			if (rsc->getVendor() == GPU_ATI)
				disableAutoMip = true;
#endif
			// The Intel 915G frequently corrupts textures when using hardware mip generation
			// I'm not currently sure how many generations of hardware this affects, 
			// so for now, be safe.
			if (rsc->getVendor() == GPU_INTEL)
				disableAutoMip = true;

			// SiS chipsets also seem to have problems with this
			if (rsc->getVendor() == GPU_SIS)
				disableAutoMip = true;

			if (!disableAutoMip)
				rsc->setCapability(RSC_AUTOMIPMAP);
		}

		// Check for blending support
		if(GLEW_VERSION_1_3 || 
			GLEW_ARB_texture_env_combine || 
			GLEW_EXT_texture_env_combine)
		{
			rsc->setCapability(RSC_BLENDING);
		}

		// Check for Anisotropy support
		if(GLEW_EXT_texture_filter_anisotropic)
		{
			rsc->setCapability(RSC_ANISOTROPY);
		}

		// Check for DOT3 support
		if(GLEW_VERSION_1_3 ||
			GLEW_ARB_texture_env_dot3 ||
			GLEW_EXT_texture_env_dot3)
		{
			rsc->setCapability(RSC_DOT3);
		}

		// Check for cube mapping
		if(GLEW_VERSION_1_3 || 
			GLEW_ARB_texture_cube_map ||
			GLEW_EXT_texture_cube_map)
		{
			rsc->setCapability(RSC_CUBEMAPPING);
		}


		// Point sprites
		if (GLEW_VERSION_2_0 ||	GLEW_ARB_point_sprite)
		{
			rsc->setCapability(RSC_POINT_SPRITES);
		}
		// Check for point parameters
		if (GLEW_VERSION_1_4)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
		}
		if (GLEW_ARB_point_parameters)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS_ARB);
		}
		if (GLEW_EXT_point_parameters)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS_EXT);
		}

		// Check for hardware stencil support and set bit depth
		GLint stencil;
		glGetIntegerv(GL_STENCIL_BITS,&stencil);

		if(stencil)
		{
			rsc->setCapability(RSC_HWSTENCIL);
			rsc->setStencilBufferBitDepth(stencil);
		}

		if(GLEW_VERSION_1_5 || GLEW_ARB_vertex_buffer_object)
		{
			if (!GLEW_ARB_vertex_buffer_object)
			{
				rsc->setCapability(RSC_GL1_5_NOVBO);
			}
			rsc->setCapability(RSC_VBO);
		}

		rsc->setCapability(RSC_VERTEX_PROGRAM);
		rsc->setCapability(RSC_FRAGMENT_PROGRAM);

		rsc->addShaderProfile("cg");

		// NFZ - Check if GLSL is supported
		if ( GLEW_VERSION_2_0 || 
			(GLEW_ARB_shading_language_100 &&
			GLEW_ARB_shader_objects &&
			GLEW_ARB_fragment_shader &&
			GLEW_ARB_vertex_shader) )
		{
			rsc->addShaderProfile("glsl");
		}

		// Check if geometry shaders are supported
		if (GLEW_VERSION_2_0 &&
			GLEW_EXT_geometry_shader4)
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
		if (GLEW_VERSION_2_0 && 
			GLEW_NV_transform_feedback)
		{
			rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
		}

		// Check for texture compression
		if(GLEW_VERSION_1_3 || GLEW_ARB_texture_compression)
		{   
			rsc->setCapability(RSC_TEXTURE_COMPRESSION);

			// Check for dxt compression
			if(GLEW_EXT_texture_compression_s3tc)
			{
				rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
			}
			// Check for vtc compression
			if(GLEW_NV_texture_compression_vtc)
			{
				rsc->setCapability(RSC_TEXTURE_COMPRESSION_VTC);
			}
		}

		// Scissor test is standard in GL 1.2 (is it emulated on some cards though?)
		rsc->setCapability(RSC_SCISSOR_TEST);
		// As are user clipping planes
		rsc->setCapability(RSC_USER_CLIP_PLANES);

		// 2-sided stencil?
		if (GLEW_VERSION_2_0 || GLEW_EXT_stencil_two_side)
		{
			rsc->setCapability(RSC_TWO_SIDED_STENCIL);
		}
		// stencil wrapping?
		if (GLEW_VERSION_1_4 || GLEW_EXT_stencil_wrap)
		{
			rsc->setCapability(RSC_STENCIL_WRAP);
		}

		// Check for hardware occlusion support
		if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
		{
			// Some buggy driver claim that it is GL 1.5 compliant and
			// not support ARB_occlusion_query
			if (!GLEW_ARB_occlusion_query)
			{
				rsc->setCapability(RSC_GL1_5_NOHWOCCLUSION);
			}

			rsc->setCapability(RSC_HWOCCLUSION);
		}
		else if (GLEW_NV_occlusion_query)
		{
			// Support NV extension too for old hardware
			rsc->setCapability(RSC_HWOCCLUSION);
		}

		// UBYTE4 always supported
		rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

		// Infinite far plane always supported
		rsc->setCapability(RSC_INFINITE_FAR_PLANE);

		// Check for non-power-of-2 texture support
		if(GLEW_ARB_texture_non_power_of_two)
		{
			rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
		}

		// Check for Float textures
		if(GLEW_ATI_texture_float || GLEW_ARB_texture_float)
		{
			rsc->setCapability(RSC_TEXTURE_FLOAT);
		}

		// 3D textures should always be supported
		rsc->setCapability(RSC_TEXTURE_3D);

		// Check for framebuffer object extension
		if(GLEW_EXT_framebuffer_object)
		{
			// Probe number of draw buffers
			// Only makes sense with FBO support, so probe here
			if(GLEW_VERSION_2_0 || 
				GLEW_ARB_draw_buffers ||
				GLEW_ATI_draw_buffers)
			{
				GLint buffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &buffers);
				rsc->setNumMultiRenderTargets(std::min<int>(buffers, (GLint)CM_MAX_MULTIPLE_RENDER_TARGETS));
				rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);
				if(!GLEW_VERSION_2_0)
				{
					// Before GL version 2.0, we need to get one of the extensions
					if(GLEW_ARB_draw_buffers)
						rsc->setCapability(RSC_FBO_ARB);
					if(GLEW_ATI_draw_buffers)
						rsc->setCapability(RSC_FBO_ATI);
				}
				// Set FBO flag for all 3 'subtypes'
				rsc->setCapability(RSC_FBO);

			}
			rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
		}

		// Check GLSupport for PBuffer support
		if(mGLSupport->supportsPBuffers())
		{
			// Use PBuffers
			rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
			rsc->setCapability(RSC_PBUFFER);
		}

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
		rsc->setNumUniformBlockBuffers(GPT_VERTEX_PROGRAM, numUniformBlocks);

		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &numUniformBlocks);
		rsc->setNumUniformBlockBuffers(GPT_FRAGMENT_PROGRAM, numUniformBlocks);

		if (mGLSupport->checkExtension("GL_ARB_geometry_shader4"))
		{
			GLint geomUnits;
			glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &geomUnits);
			rsc->setNumTextureUnits(GPT_GEOMETRY_PROGRAM, static_cast<UINT16>(geomUnits));

			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumUniformBlockBuffers(GPT_GEOMETRY_PROGRAM, numUniformBlocks);
		}

		if (mGLSupport->checkExtension("GL_ARB_tessellation_shader"))
		{
			rsc->setCapability(RSC_TESSELLATION_PROGRAM);

			glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumUniformBlockBuffers(GPT_HULL_PROGRAM, numUniformBlocks);

			glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, &numUniformBlocks);
			rsc->setNumUniformBlockBuffers(GPT_DOMAIN_PROGRAM, numUniformBlocks);
		}

		if (mGLSupport->checkExtension("GL_ARB_compute_shader")) // Enable once I include GL 4.3
		{
			//rsc->setCapability(RSC_COMPUTE_PROGRAM);

			//GLint computeUnits;
			//glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &computeUnits);
			//rsc->setNumTextureUnits(GPT_COMPUTE_PROGRAM, static_cast<UINT16>(computeUnits));
			
			//glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &numUniformBlocks);
			//rsc->setNumUniformBlockBuffers(GPT_COMPUTE_PROGRAM, numUniformBlocks);
		}

		GLint combinedTexUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combinedTexUnits);
		rsc->setNumCombinedTextureUnits(static_cast<UINT16>(combinedTexUnits));

		GLint combinedUniformBlockUnits;
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &combinedUniformBlockUnits);
		rsc->setNumCombinedUniformBlockBuffers(static_cast<UINT16>(combinedUniformBlockUnits));

		// Mipmap LOD biasing
		rsc->setCapability(RSC_MIPMAP_LOD_BIAS);

		// These are Cg supported profiles, not really used in OpenGL itself in any way
		rsc->addShaderProfile("glslf");
		rsc->addShaderProfile("glslv");
		rsc->addShaderProfile("glslg");

		rsc->addGpuProgramProfile(GPP_PS_1_1, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_1_2, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_1_3, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_1_4, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_2_0, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_2_x, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_2_a, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_2_b, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_3_0, "glslf");
		rsc->addGpuProgramProfile(GPP_PS_3_x, "glslf");

		rsc->addGpuProgramProfile(GPP_VS_1_1, "glslv");
		rsc->addGpuProgramProfile(GPP_VS_2_0, "glslv");
		rsc->addGpuProgramProfile(GPP_VS_2_x, "glslv");
		rsc->addGpuProgramProfile(GPP_VS_2_a, "glslv");
		rsc->addGpuProgramProfile(GPP_VS_3_0, "glslv");

		rsc->addGpuProgramProfile(GPP_PS_4_0, "glslf");
		rsc->addGpuProgramProfile(GPP_VS_4_0, "glslv");
		rsc->addGpuProgramProfile(GPP_GS_4_0, "glslg");

		rsc->addGpuProgramProfile(GPP_PS_4_0, "glslf");
		rsc->addGpuProgramProfile(GPP_VS_4_0, "glslv");
		rsc->addGpuProgramProfile(GPP_GS_4_0, "glslg");

		rsc->addGpuProgramProfile(GPP_PS_4_1, "glslf");
		rsc->addGpuProgramProfile(GPP_VS_4_1, "glslv");
		rsc->addGpuProgramProfile(GPP_GS_4_1, "glslg");

		// No SM5 support for Cg as right now it only supports NV extensions, which isn't very useful

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
	//---------------------------------------------------------------------
	UINT32 GLRenderSystem::getGLTextureUnit(GpuProgramType gptype, UINT32 unit)
	{
		if(gptype != GPT_VERTEX_PROGRAM && gptype != GPT_FRAGMENT_PROGRAM && gptype != GPT_GEOMETRY_PROGRAM)
		{
			CM_EXCEPT(InvalidParametersException, "OpenGL cannot assign textures to this gpu program type: " + toString(gptype));
		}

		UINT32 numSupportedUnits = mCurrentCapabilities->getNumTextureUnits(gptype);
		if(unit < 0 || unit >= numSupportedUnits)
		{
			CM_EXCEPT(InvalidParametersException, "Invalid texture unit index for the provided stage. Unit index: " + toString(unit) + ". Stage: " + 
				toString(gptype) + ". Supported range is 0 .. " + toString(numSupportedUnits - 1));
		}

		switch(gptype)
		{
		case GPT_FRAGMENT_PROGRAM:
			return mFragmentTexOffset + unit;
		case GPT_VERTEX_PROGRAM:
			return mVertexTexOffset + unit;
		case GPT_GEOMETRY_PROGRAM:
			return mGeometryTexOffset + unit;
		default:
			CM_EXCEPT(InternalErrorException, "Invalid program type: " + toString(gptype));
		}
	}
	//---------------------------------------------------------------------
	UINT32 GLRenderSystem::getGLUniformBlockBinding(GpuProgramType gptype, UINT32 binding)
	{
		UINT32 maxNumBindings = mCurrentCapabilities->getNumUniformBlockBuffers(gptype);
		if(binding < 0 || binding >= maxNumBindings)
		{
			CM_EXCEPT(InvalidParametersException, "Invalid buffer binding for the provided stage. Buffer binding: " + toString(binding) + ". Stage: " + 
				toString(gptype) + ". Supported range is 0 .. " + toString(maxNumBindings - 1));
		}

		switch(gptype)
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
			CM_EXCEPT(InternalErrorException, "Invalid program type: " + toString(gptype));
		}
	}

	void GLRenderSystem::setActiveProgram(GpuProgramType gptype, GLSLGpuProgramPtr program)
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

	GLSLGpuProgramPtr GLRenderSystem::getActiveProgram(GpuProgramType gptype) const
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
			CM_EXCEPT(InvalidParametersException, "Insupported gpu program type: " + toString(gptype));
		}
	}
	/************************************************************************/
	/* 								UTILITY		                     		*/
	/************************************************************************/
	float GLRenderSystem::getMinimumDepthInputValue(void)
	{
		// Range [-1.0f, 1.0f]
		return -1.0f;
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getMaximumDepthInputValue(void)
	{
		// Range [-1.0f, 1.0f]
		return 1.0f;
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getHorizontalTexelOffset(void)
	{
		// No offset in GL
		return 0.0f;
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getVerticalTexelOffset(void)
	{
		// No offset in GL
		return 0.0f;
	}
	VertexElementType GLRenderSystem::getColorVertexElementType(void) const
	{
		return VET_COLOR_ABGR;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::convertProjectionMatrix(const Matrix4& matrix,
		Matrix4& dest, bool forGpuProgram)
	{
		// no any conversion request for OpenGL
		dest = matrix;
	}
}

#undef THROW_IF_NOT_RENDER_THREAD
