#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderAPI.h"
#include "BsGLHardwareBufferManager.h"
#include "BsGLSLProgramFactory.h"
#include "BsVector4.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Implementation of a render system using OpenGL. Provides abstracted
	 *			access to various low level OpenGL methods.
	 */
    class BS_RSGL_EXPORT GLRenderAPI : public RenderAPICore
    {
    public:
        GLRenderAPI();
        ~GLRenderAPI();

		/**
		 * @copydoc RenderAPICore::getName()
		 */
		const StringID& getName() const override;

		/**
		 * @copydoc RenderAPICore::getShadingLanguageName()
		 */
		const String& getShadingLanguageName() const override;

		/**
		 * @copydoc RenderAPICore::setRenderTarget()
		 */
		void setRenderTarget(const SPtr<RenderTargetCore>& target) override;

        /**
		 * @copydoc RenderAPICore::setVertexBuffers()
		 */
		void setVertexBuffers(UINT32 index, SPtr<VertexBufferCore>* buffers, UINT32 numBuffers) override;

		/**
		 * @copydoc RenderAPICore::setIndexBuffer()
		 */
		void setIndexBuffer(const SPtr<IndexBufferCore>& buffer) override;

		/**
		 * @copydoc RenderAPICore::setVertexDeclaration()
		 */
		void setVertexDeclaration(const SPtr<VertexDeclarationCore>& vertexDeclaration) override;

		/**
		 * @copydoc RenderAPICore::setDrawOperation()
		 */
		void setDrawOperation(DrawOperationType op) override;

        /**
		 * @copydoc RenderAPICore::setScissorRect()
		 */
		void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) override;

		/**
		 * @copydoc RenderAPICore::setTexture()
		 */
		void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& tex) override;

		/**
		 * @copydoc	RenderAPICore::setLoadStoreTexture
		 */
		void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr,
			const TextureSurface& surface) override;
        
		/**
		 * @copydoc RenderAPICore::setSamplerState()
		 */
		void setSamplerState(GpuProgramType gptype, UINT16 unit, const SPtr<SamplerStateCore>& state) override;

		/**
		 * @copydoc RenderAPICore::setBlendState()
		 */
		void setBlendState(const SPtr<BlendStateCore>& blendState) override;

		/**
		 * @copydoc RenderAPICore::setRasterizerState()
		 */
		void setRasterizerState(const SPtr<RasterizerStateCore>& rasterizerState) override;

		/**
		 * @copydoc RenderAPICore::setDepthStencilState()
		 */
		void setDepthStencilState(const SPtr<DepthStencilStateCore>& depthStencilState, UINT32 stencilRefValue) override;

		/**
		 * @copydoc RenderAPICore::setViewport()
		 */
		void setViewport(const Rect2& area) override;

		/**
		 * @copydoc RenderAPICore::bindGpuProgram()
		 */
		void bindGpuProgram(const SPtr<GpuProgramCore>& prg) override;

		/**
		 * @copydoc RenderAPICore::unbindGpuProgram()
		 */
		void unbindGpuProgram(GpuProgramType gptype) override;

		/**
		 * @copydoc RenderAPICore::setConstantBuffers()
		 */
		void setConstantBuffers(GpuProgramType gptype, const SPtr<GpuParamsCore>& params) override;

		/**
		 * @copydoc RenderAPICore::beginFrame()
		 */
		void beginFrame() override;

		/**
		 * @copydoc RenderAPICore::endFrame()
		 */
		void endFrame() override;

		/**
		 * @copydoc RenderAPICore::draw()
		 */
		void draw(UINT32 vertexOffset, UINT32 vertexCount) override;

		/**
		 * @copydoc RenderAPICore::drawIndexed()
		 */
		void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount) override;

		/**
		 * @copydoc RenderAPICore::clearRenderTarget()
		 */
		void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) override;

		/**
		 * @copydoc RenderAPICore::clearViewport()
		 */
		void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) override;

        /**
		 * @copydoc RenderAPICore::getColorVertexElementType()
		 */
		VertexElementType getColorVertexElementType() const override;

		/**
		 * @copydoc RenderAPICore::getHorizontalTexelOffset()
		 */
		float getHorizontalTexelOffset() override;

		/**
		 * @copydoc RenderAPICore::getVerticalTexelOffset()
		 */
		float getVerticalTexelOffset() override;

		/**
		 * @copydoc RenderAPICore::getMinimumDepthInputValue()
		 */
		float getMinimumDepthInputValue() override;

		 /**
		 * @copydoc RenderAPICore::getMaximumDepthInputValue()
		 */
		float getMaximumDepthInputValue() override;

		/**
		 * @copydoc RenderAPICore::convertProjectionMatrix()
		 */
		void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) override;

		/**
		 * @copydoc RenderAPICore::generateParamBlockDesc()
		 */
		GpuParamBlockDesc generateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params);

		/************************************************************************/
		/* 				Internal use by OpenGL RenderSystem only                */
		/************************************************************************/

		/**
		 * @brief	Query has the main context been initialized.
		 */
		bool _isContextInitialized() const { return mGLInitialised; }

		/**
		 * @brief	Returns main context. Caller must ensure the context has been initialized.
		 */
		SPtr<GLContext> getMainContext() const { return mMainContext; } 

		/**
		 * @brief	Returns a support object you may use for creating
		 */
		GLSupport* getGLSupport() const { return mGLSupport; }

	protected:
		/**
		 * @copydoc	RenderAPICore::initializePrepare
		 */
		void initializePrepare() override;

		/**
		 * @copydoc	RenderAPICore::initializeFinalize
		 */
		void initializeFinalize(const SPtr<RenderWindowCore>& primaryWindow) override;

		/**
		 * @copydoc	RenderAPICore::destroy_internal().
		 */
		void destroyCore() override;

		/**
		* @brief	Call before doing a draw operation, this method sets everything up.
		*/
		void beginDraw();

		/**
		* @brief	Needs to accompany every beginDraw after you are done with a single draw operation.
		*/
		void endDraw();

		/**
		 * @brief	Clear a part of a render target.
		 */
		void clearArea(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, const Rect2I& clearArea = Rect2I::EMPTY);

		/**
		 * @brief	Set up clip planes against which all geometry will get clipped.
		 */
		void setClipPlanesImpl(const PlaneList& clipPlanes) override;

		/**
		 * @brief	Set up a clip plane at a specific clip plane index. If enabled,
		 *			geometry will be clipped against the positive side of the plane.
		 *
		 * @note	Valid index range is [0, 5].
		 */
        void setClipPlane(UINT16 index, float A, float B, float C, float D);

		/**
		 * @brief	Enable or disable clipping against a clip plane at the specified index.
		 *
		 * @note	Valid index range is [0, 5].
		 */
        void enableClipPlane (UINT16 index, bool enable);

		/**
		* @brief	Changes the currently active texture unit. Any texture related operations
		*			will then be performed on this unit.
		*/
		bool activateGLTextureUnit(UINT16 unit);

		/**
		 * @brief	Changes the active GPU program.
		 */
		void setActiveProgram(GpuProgramType gptype, const SPtr<GLSLGpuProgramCore>& program);

		/**
		 * @brief	Retrieves the active GPU program of the specified type.
		 */
		SPtr<GLSLGpuProgramCore> getActiveProgram(GpuProgramType gptype) const;

		/**
		 * @brief	Converts Banshee blend mode to OpenGL blend mode.
		 */
		GLint getBlendMode(BlendFactor blendMode) const;

		/**
		* @brief	Converts Banshee texture addressing mode to OpenGL texture addressing mode.
		*/
		GLint getTextureAddressingMode(TextureAddressingMode tam) const;

		/**
		 * @brief	Gets a combined min/mip filter value usable by OpenGL from the currently
		 *			set min and mip filters.
		 */
		GLuint getCombinedMinMipFilter() const;

		/**
		* @brief	OpenGL shares all texture slots, but the engine prefers to keep textures
		* 			separate per-stage. This will convert texture unit that is set per stage
		* 			into a global texture unit usable by OpenGL.
		*/
		UINT32 getGLTextureUnit(GpuProgramType gptype, UINT32 unit);

		/**
		* @brief	OpenGL shares all buffer bindings, but the engine prefers to keep buffers
		* 			separate per-stage. This will convert block buffer binding that is set per stage
		* 			into a global block buffer binding usable by OpenGL.
		*/
		UINT32 getGLUniformBlockBinding(GpuProgramType gptype, UINT32 binding);

		/**
		* @brief	Returns the OpenGL specific mode used for drawing, depending on the
		* 			currently set draw operation;
		*/
		GLint getGLDrawMode() const;

		/**
		 * @brief	Creates render system capabilities that specify which features are
		 *			or aren't supported.
		 */
		RenderAPICapabilities* createRenderSystemCapabilities() const;

		/**
		 * @brief	Finish initialization by setting up any systems dependant on render system
		 *			capabilities.
		 */
		void initFromCaps(RenderAPICapabilities* caps);

		/**
		 * @brief	Switch the currently used OpenGL context. You will need to re-bind
		 *			any previously bound values manually. (e.g. textures, gpu programs and such)
		 */
		void switchContext(const SPtr<GLContext>& context);

		/************************************************************************/
		/* 								Sampler states                     		*/
		/************************************************************************/

		/**
		 * @brief	Sets the texture addressing mode for a texture unit. This determines
		 *			how are UV address values outside of [0, 1] range handled when sampling
		 *			from texture.
		 */
        void setTextureAddressingMode(UINT16 stage, const UVWAddressingMode& uvw);

		/**
		 * @brief	Sets the texture border color for a texture unit. Border color
		 *			determines color returned by the texture sampler when border addressing mode
		 *			is used and texture address is outside of [0, 1] range.
		 */
        void setTextureBorderColor(UINT16 stage, const Color& color);

		/**
		 * @brief	Sets the mipmap bias value for a given texture unit. Bias allows
		 *			you to adjust the mipmap selection calculation. Negative values force a
		 *			larger mipmap to be used, and positive values smaller. Units are in values
		 *			of mip levels, so -1 means use a mipmap one level higher than default.
		 */
		void setTextureMipmapBias(UINT16 unit, float bias);

		/**
		 * @brief	Allows you to specify how is the texture bound to the specified texture unit filtered.
		 *			Different filter types are used for different situations like magnifying or minifying a texture.
		 */
        void setTextureFiltering(UINT16 unit, FilterType ftype, FilterOptions filter);

		/**
		 * @brief	Sets anisotropy value for the specified texture unit.
		 */
		void setTextureAnisotropy(UINT16 unit, UINT32 maxAnisotropy);

		/**
		 * @brief	Gets anisotropy value for the specified texture unit.
		 */
		GLfloat getCurrentAnisotropy(UINT16 unit);

		/************************************************************************/
		/* 								Blend states                      		*/
		/************************************************************************/

		/**
		 * @brief	Sets up blending mode that allows you to combine new pixels with pixels already in the render target.
		 *			Final pixel value = (renderTargetPixel * sourceFactor) op (pixel * destFactor).
		 */
		void setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, BlendOperation op);

		/**
		* @brief	Sets up blending mode that allows you to combine new pixels with pixels already in the render target.
		*			Allows you to set up separate blend operations for alpha values.
		*	
		*			Final pixel value = (renderTargetPixel * sourceFactor) op (pixel * destFactor). (And the same for alpha)
		*/
		void setSceneBlending(BlendFactor sourceFactor, BlendFactor destFactor, BlendFactor sourceFactorAlpha, 
			BlendFactor destFactorAlpha, BlendOperation op, BlendOperation alphaOp);

		/**
		 * @brief	Sets alpha test that allows you to reject pixels that fail the comparison function
		 *			versus the provided reference value.
		 */
		void setAlphaTest(CompareFunction func, unsigned char value);

		/**
		 * @brief	Enable alpha to coverage. Alpha to coverage allows you to perform blending without needing 
		 *			to worry about order of rendering like regular blending does. It requires multi-sampling to 
		 *			be active in order to work, and you need to supply an alpha texture that determines object transparency.
		 */
		void setAlphaToCoverage(bool enabled);

		/**
		 * @brief	Enables or disables writing to certain color channels of the render target.
		 */
		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		/************************************************************************/
		/* 								Rasterizer states                  		*/
		/************************************************************************/

		/**
		 * @brief	Sets vertex winding order. Normally you would use this to cull back facing
		 *			polygons.
		 */
		void setCullingMode(CullingMode mode);

		/**
		 * @brief	Sets the polygon rasterization mode. Determines how are polygons interpreted.
		 */
		void setPolygonMode(PolygonMode level);

		/**
		 * @brief	Sets a depth bias that will offset the depth values of new pixels by the specified amount.
		 *			Final depth bias value is a combination of the constant depth bias and slope depth bias.
		 *			Slope depth bias has more effect the higher the slope of the rendered polygon.
		 *
		 * @note	This is useful if you want to avoid z fighting for objects at the same or similar depth.
		 */
		void setDepthBias(float constantBias, float slopeScaleBias);

		/**
		 * @brief	Scissor test allows you to mask off rendering in all but a given rectangular area
		 * 			identified by the rectangle set by setScissorRect().
		 */
		void setScissorTestEnable(bool enable);

		/**
		 * @brief	Enables or disables multisample antialiasing.
		 */
		void setMultisamplingEnable(bool enable);

		/**
		 * @brief	Enables or disables depth clipping (i.e. near/fear plane clipping).
		 */
		void setDepthClipEnable(bool enable);

		/**
		 * @brief	Enables or disables antialiased line rendering.
		 */
		void setAntialiasedLineEnable(bool enable);

		/************************************************************************/
		/* 						Depth stencil state                      		*/
		/************************************************************************/
		
		/**
		 * @brief	Should new pixels perform depth testing using the set depth comparison function before
		 *			being written.
		 */
		void setDepthBufferCheckEnabled(bool enabled = true);

		/**
		 * @brief	Should new pixels write to the depth buffer.
		 */
		void setDepthBufferWriteEnabled(bool enabled = true);

		/**
		 * @brief	Sets comparison function used for depth testing. Determines how are new and existing
		 *			pixel values compared - if comparison function returns true the new pixel is written.
		 */
		void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);

		/**
		 * @brief	Turns stencil tests on or off. By default this is disabled.
		 *			Stencil testing allow you to mask out a part of the rendered image by using
		 *			various stencil operations provided.
		 */
		void setStencilCheckEnabled(bool enabled);

		/**
		 * @brief	Allows you to set stencil operations that are performed when stencil test passes or fails.
		 *
		 * @param	stencilFailOp	Operation executed when stencil test fails.
		 * @param	depthFailOp		Operation executed when stencil test succeeds but depth test fails.
		 * @param	passOp			Operation executed when stencil test succeeds and depth test succeeds.
		 * @param	front			Should the stencil operations be applied to front or back facing polygons.
		 */
		void setStencilBufferOperations(StencilOperation stencilFailOp = SOP_KEEP,
			StencilOperation depthFailOp = SOP_KEEP, StencilOperation passOp = SOP_KEEP,
			bool front = true);

		/**
		 * @brief	Sets a stencil buffer comparison function. The result of this will cause one of 3 actions 
		 *			depending on whether the test fails, succeeds but with the depth buffer check still failing, 
		 *			or succeeds with the depth buffer check passing too.
		 *
		 * @param func	Comparison function that determines whether a stencil test fails or passes. Reference value
		 *				gets compared to the value already in the buffer using this function.
		 * @param mask	The bitmask applied to both the stencil value and the reference value
		 *				before comparison
		 * @param ccw	If set to true, the stencil operations will be applied to counterclockwise
		 *				faces. Otherwise they will be applied to clockwise faces.
		 */
		void setStencilBufferFunc(CompareFunction func = CMPF_ALWAYS_PASS, UINT32 mask = 0xFFFFFFFF, bool front = true);

		/**
		 * @brief	The bitmask applied to the stencil value before writing it to the stencil buffer.
		 */
		void setStencilBufferWriteMask(UINT32 mask = 0xFFFFFFFF);

		/**
		 * @brief	Sets a reference values used for stencil buffer comparisons. 
		 * 			Actual comparison function and stencil operations are set by setting the DepthStencilState.
		 */
		void setStencilRefValue(UINT32 refValue);

		/************************************************************************/
		/* 							UTILITY METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Recalculates actual viewport dimensions based on currently 
		 *			set viewport normalized dimensions and render target and applies
		 *			them for further rendering.
		 */
		void applyViewport();

		/**
		 * @brief	Converts the provided matrix m into a representation usable by OpenGL.
		 */
		void makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m);

		/**
		 * @brief	Converts the engine depth/stencil compare function into OpenGL representation.
		 */
		GLint convertCompareFunction(CompareFunction func) const;

		/**
		 * @brief	Convers the engine stencil operation in OpenGL representation. Optionally inverts
		 *			the operation (increment becomes decrement, etc.).
		 */
		GLint convertStencilOp(StencilOperation op, bool invert = false) const;

		/**
		 * @brief	Checks if there are any OpenGL errors and prints them to the log.
		 */
		bool checkForErrors() const;

	private:
		Rect2 mViewportNorm;
		UINT32 mScissorTop, mScissorBottom, mScissorLeft, mScissorRight;
		UINT32 mViewportLeft, mViewportTop, mViewportWidth, mViewportHeight;

		UINT32 mStencilReadMask;
		UINT32 mStencilWriteMask;
		UINT32 mStencilRefValue;
		CompareFunction mStencilCompareFront;
		CompareFunction mStencilCompareBack;

		// View matrix to set world against
		Matrix4 mViewMatrix;

		// Last min & mip filtering options, so we can combine them
		FilterOptions mMinFilter;
		FilterOptions mMipFilter;

		// Holds texture type settings for every stage
		UINT32	mNumTextureTypes;
		GLenum* mTextureTypes;

		bool mDepthWrite;
		bool mColorWrite[4];

		GLSupport* mGLSupport;
		bool mGLInitialised;

		GLSLProgramFactory* mGLSLProgramFactory;
		GLSLProgramPipelineManager* mProgramPipelineManager;

		SPtr<GLSLGpuProgramCore> mCurrentVertexProgram;
		SPtr<GLSLGpuProgramCore> mCurrentFragmentProgram;
		SPtr<GLSLGpuProgramCore> mCurrentGeometryProgram;
		SPtr<GLSLGpuProgramCore> mCurrentHullProgram;
		SPtr<GLSLGpuProgramCore> mCurrentDomainProgram;

		const GLSLProgramPipeline* mActivePipeline;

		UINT32 mFragmentTexOffset;
		UINT32 mVertexTexOffset;
		UINT32 mGeometryTexOffset;

		UINT32 mFragmentUBOffset;
		UINT32 mVertexUBOffset;
		UINT32 mGeometryUBOffset;
		UINT32 mHullUBOffset;
		UINT32 mDomainUBOffset;
		UINT32 mComputeUBOffset;

		Vector<SPtr<VertexBufferCore>> mBoundVertexBuffers;
		SPtr<VertexDeclarationCore> mBoundVertexDeclaration;
		SPtr<IndexBufferCore> mBoundIndexBuffer;
		DrawOperationType mCurrentDrawOperation;

		SPtr<GLContext> mMainContext;
		SPtr<GLContext> mCurrentContext;

		bool mDrawCallInProgress;

		UINT16 mActiveTextureUnit;
    };
}
