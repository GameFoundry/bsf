//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once
#include "BsCorePrerequisites.h"
#include "BsSamplerState.h"
#include "BsCommandQueue.h"
#include "BsDrawOps.h"
#include "BsRenderAPICapabilities.h"
#include "BsRenderTarget.h"
#include "BsRenderTexture.h"
#include "BsRenderWindow.h"
#include "BsGpuProgram.h"
#include "BsVertexDeclaration.h"
#include "BsPlane.h"
#include "BsModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	class RenderAPIInfo;

	/**
	 * Version of the render API interface usable from the sim thread. All the commands	get queued on the accessor provided
	 * to each method and will be executed on the core thread later.
	 *
	 * @see		RenderAPICore
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT RenderAPI
	{
	public:
		/** @copydoc RenderAPICore::disableTextureUnit() */
		static void disableTextureUnit(CoreAccessor& accessor, GpuProgramType gptype, UINT16 texUnit);

		/** @copydoc RenderAPICore::setTexture() */
		static void setTexture(CoreAccessor& accessor, GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr);

		/** @copydoc RenderAPICore::setLoadStoreTexture() */
		static void setLoadStoreTexture(CoreAccessor& accessor, GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr& texPtr,
			const TextureSurface& surface);

		/** @copydoc RenderAPICore::setSamplerState() */
		static void setSamplerState(CoreAccessor& accessor, GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState);

		/** @copydoc RenderAPICore::setBlendState() */
		static void setBlendState(CoreAccessor& accessor, const BlendStatePtr& blendState);

		/** @copydoc RenderAPICore::setRasterizerState() */
		static void setRasterizerState(CoreAccessor& accessor, const RasterizerStatePtr& rasterizerState);

		/** @copydoc RenderAPICore::setRasterizerState() */
		static void setDepthStencilState(CoreAccessor& accessor, const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue);

		/** @copydoc RenderAPICore::setVertexBuffers() */
		static void setVertexBuffers(CoreAccessor& accessor, UINT32 index, const Vector<VertexBufferPtr>& buffers);

		/** @copydoc RenderAPICore::setIndexBuffer() */
		static void setIndexBuffer(CoreAccessor& accessor, const IndexBufferPtr& buffer);

		/** @copydoc RenderAPICore::setVertexDeclaration() */
		static void setVertexDeclaration(CoreAccessor& accessor, const VertexDeclarationPtr& vertexDeclaration);

		/** @copydoc RenderAPICore::setViewport() */
		static void setViewport(CoreAccessor& accessor, const Rect2& vp);

		/** @copydoc RenderAPICore::setDrawOperation() */
		static void setDrawOperation(CoreAccessor& accessor, DrawOperationType op);

		/** @copydoc RenderAPICore::setClipPlanes() */
		static void setClipPlanes(CoreAccessor& accessor, const PlaneList& clipPlanes);

		/** @copydoc RenderAPICore::addClipPlane(const Plane&) */
		static void addClipPlane(CoreAccessor& accessor, const Plane& p);

		/** @copydoc RenderAPICore::resetClipPlanes() */
		static void resetClipPlanes(CoreAccessor& accessor);

		/** @copydoc RenderAPICore::setScissorTest() */
		static void setScissorTest(CoreAccessor& accessor, UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600);

		/** @copydoc RenderAPICore::setRenderTarget() */
		static void setRenderTarget(CoreAccessor& accessor, const RenderTargetPtr& target, bool readOnlyDepthStencil = false);

		/** @copydoc RenderAPICore::bindGpuProgram() */
		static void bindGpuProgram(CoreAccessor& accessor, const GpuProgramPtr& prg);

		/** @copydoc RenderAPICore::unbindGpuProgram() */
		static void unbindGpuProgram(CoreAccessor& accessor, GpuProgramType gptype);

		/** @copydoc RenderAPICore::setConstantBuffers() */
		static void setConstantBuffers(CoreAccessor& accessor, GpuProgramType gptype, const GpuParamsPtr& params);

		/** @copydoc RenderAPICore::setGpuParams() */
		static void setGpuParams(CoreAccessor& accessor, GpuProgramType gptype, const GpuParamsPtr& params);

		/** @copydoc RenderAPICore::beginFrame() */
		static void beginRender(CoreAccessor& accessor);

		/** @copydoc RenderAPICore::endFrame() */
		static void endRender(CoreAccessor& accessor);

		/** @copydoc RenderAPICore::clearRenderTarget() */
		static void clearRenderTarget(CoreAccessor& accessor, UINT32 buffers, 
			const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF);

		/** @copydoc RenderAPICore::clearViewport() */
		static void clearViewport(CoreAccessor& accessor, UINT32 buffers, const Color& color = Color::Black, 
			float depth = 1.0f, UINT16 stencil = 0, UINT8 targetMask = 0xFF);

		/** @copydoc RenderAPICore::swapBuffers() */
		static void swapBuffers(CoreAccessor& accessor, const RenderTargetPtr& target);

		/** @copydoc RenderAPICore::draw() */
		static void draw(CoreAccessor& accessor, UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderAPICore::drawIndexed() */
		static void drawIndexed(CoreAccessor& accessor, UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount);

		/** @copydoc RenderAPICore::dispatchCompute() */
		static void dispatchCompute(CoreAccessor& accessor, UINT32 numGroupsX, UINT32 numGroupsY = 1, UINT32 numGroupsZ = 1);

		/** @copydoc RenderAPICore::getVideoModeInfo */
		static const VideoModeInfo& getVideoModeInfo();

		/** @copydoc RenderAPICore::convertProjectionMatrix */
		static void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest);

		/** @copydoc RenderAPICore::getAPIInfo */
		static const RenderAPIInfo& getAPIInfo();
	};

	/** Contains properties specific to a render API implementation. */
	class RenderAPIInfo
	{
	public:
		RenderAPIInfo(float horzTexelOffset, float vertTexelOffset, float minDepth, float maxDepth, 
			VertexElementType vertexColorType, bool vertexColorFlip, bool ndcVerticalTopToBottom, bool columnMajorMatrices)
			: mHorizontalTexelOffset(horzTexelOffset), mVerticalTexelOffset(vertTexelOffset), mMinDepth(minDepth)
			, mMaxDepth(maxDepth), mVertexColorType(vertexColorType), mVertexColorFlip(vertexColorFlip)
			, mNDCYAxisDown(ndcVerticalTopToBottom), mColumnMajorMatrices(columnMajorMatrices)
		{
			
		}

		/** Gets the native type used for vertex colors. */
		VertexElementType getColorVertexElementType() const { return mVertexColorType; }

		/** Gets horizontal texel offset used for mapping texels to pixels in this render system. */
		float getHorizontalTexelOffset() const { return mHorizontalTexelOffset; }

		/** Gets vertical texel offset used for mapping texels to pixels in this render system. */
		float getVerticalTexelOffset() const { return mVerticalTexelOffset; }

		/** Gets the minimum (closest) depth value used by this render system. */
		float getMinimumDepthInputValue() const { return mMinDepth; }

		/** Gets the maximum (farthest) depth value used by this render system. */
		float getMaximumDepthInputValue() const { return mMaxDepth; }

		/** Checks if vertex color needs to be flipped before sent to the shader. */
		bool getVertexColorFlipRequired() const { return mVertexColorFlip; }

		/** Checks whether GPU programs expect matrices in column major format. */
		bool getGpuProgramHasColumnMajorMatrices() const { return mColumnMajorMatrices; }
		
		/** 
		 * Returns true if Y axis in normalized device coordinates points down, false if up. If axis is pointing down the
		 * NDC at the top is -1, and at the bottom is 1, otherwise reverse.
		 */
		bool getNDCYAxisDown() const { return mNDCYAxisDown; }

	private:
		float mHorizontalTexelOffset = 0.0f;
		float mVerticalTexelOffset = 0.0f;
		float mMinDepth = 0.0f;
		float mMaxDepth = 1.0f;
		VertexElementType mVertexColorType = VET_COLOR_ABGR;
		bool mVertexColorFlip = false;
		bool mNDCYAxisDown = true;
		bool mColumnMajorMatrices = false;
	};

	/** @cond INTERNAL */

	/**
	 * Render system provides base functionality for a rendering API like DirectX or OpenGL. Most of the class is abstract
	 * and specific subclass for each rendering API needs to be implemented.
	 *
	 * @note	Core thread only unless specifically noted otherwise on per-method basis.
	 */
	class BS_CORE_EXPORT RenderAPICore : public Module<RenderAPICore>
	{
	public:
		RenderAPICore();
		virtual ~RenderAPICore();

		/**
		 * Returns the name of the rendering system. 
		 *
		 * @note	Thread safe.
		 */
		virtual const StringID& getName() const = 0;

		/**
		 * Gets the name of the primary shading language used by the rendering system.
		 *
		 * @note	Thread safe.
		 */
		virtual const String& getShadingLanguageName() const = 0;

		/**
		 * Sets a sampler state for the specified texture unit.
		 *
		 * @see		SamplerState
		 */
		virtual void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SPtr<SamplerStateCore>& samplerState) = 0;

		/**
		 * Sets a blend state used for all active render targets.
		 *
		 * @see		BlendState
		 */
		virtual void setBlendState(const SPtr<BlendStateCore>& blendState) = 0;

		/**
		 * Sets a state that controls various rasterizer options. 
		 *
		 * @see		RasterizerState
		 */
		virtual void setRasterizerState(const SPtr<RasterizerStateCore>& rasterizerState) = 0;

		/**
		 * Sets a state that controls depth & stencil buffer options.
		 *
		 * @see		DepthStencilState
		 */
		virtual void setDepthStencilState(const SPtr<DepthStencilStateCore>& depthStencilState, UINT32 stencilRefValue) = 0;

		/**
		 * Binds a texture to the pipeline for the specified GPU program type at the specified slot. If the slot matches 
		 * the one configured in the GPU program the program will be able to access this texture on the GPU.
		 */
		virtual void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const SPtr<TextureCore>& texPtr) = 0;

		/**	Turns off a texture unit. */
		virtual void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/**	Binds a texture that can be used for random load/store operations from a GPU program. */
		virtual void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, 
			const SPtr<TextureCore>& texPtr, const TextureSurface& surface) = 0;

		/**
		 * Signals that rendering for a specific viewport has started. Any draw calls need to be called between beginFrame()
		 * and endFrame(). 
		 */
		virtual void beginFrame() = 0;
		
		/** Ends that rendering to a specific viewport has ended. */
		virtual void endFrame() = 0;

		/**
		 * Sets the active viewport that will be used for all render operations.
		 *
		 * @param[in]	area	Area of the viewport, in normalized ([0,1] range) coordinates.
		 */
		virtual void setViewport(const Rect2& area) = 0;

		/**
		 * Sets the provided vertex buffers starting at the specified source index.	Set buffer to nullptr to clear the 
		 * buffer at the specified index.
		 */
		virtual void setVertexBuffers(UINT32 index, SPtr<VertexBufferCore>* buffers, UINT32 numBuffers) = 0;

		/**
		 * Sets an index buffer to use when drawing. Indices in an index buffer reference vertices in the vertex buffer, 
		 * which increases cache coherency and reduces the size of vertex buffers by eliminating duplicate data.
		 */
		virtual void setIndexBuffer(const SPtr<IndexBufferCore>& buffer) = 0;

		/**
		 * Sets the vertex declaration to use when drawing. Vertex declaration is used to decode contents of a single 
		 * vertex in a vertex buffer.
		 */
		virtual void setVertexDeclaration(const SPtr<VertexDeclarationCore>& vertexDeclaration) = 0;

		/** Sets the draw operation that determines how to interpret the elements of the index or vertex buffers. */
		virtual void setDrawOperation(DrawOperationType op) = 0;

		/**
		 * Draw an object based on currently bound GPU programs, vertex declaration and vertex buffers. Draws directly from
		 * the vertex buffer without using indices.
		 */
		virtual void draw(UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/** Draw an object based on currently bound GPU programs, vertex declaration, vertex and index buffers. */
		virtual void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/** 
		 * Executes the currently bound compute shader. 
		 *
		 * @param[in]	numGroupsX	Number of groups to start in the X direction. Must be in range [1, 65535].
		 * @param[in]	numGroupsY	Number of groups to start in the Y direction. Must be in range [1, 65535].
		 * @param[in]	numGroupsZ	Number of groups to start in the Z direction. Must be in range [1, 64].
		 */
		virtual void dispatchCompute(UINT32 numGroupsX, UINT32 numGroupsY = 1, UINT32 numGroupsZ = 1) = 0;

		/** Swap the front and back buffer of the specified render target. */
		virtual void swapBuffers(const SPtr<RenderTargetCore>& target);

		/**
		 * Gets the capabilities of the render system.
		 *
		 * @note	Thread safe.
		 */
		const RenderAPICapabilities* getCapabilities() const;

		/** Returns information about the driver version. */
		virtual const DriverVersion& getDriverVersion() const;

		/**
		 * Binds the provided GPU program to the pipeline. Any following draw operations will use this program. 
		 *
		 * @note	You need to bind at least a vertex and a fragment program in order to draw something.
		 */
		virtual void bindGpuProgram(const SPtr<GpuProgramCore>& prg);

		/**
		 * Binds constant(uniform) GPU program parameters. Caller must ensure these match the previously bound GPU program.
		 */
		virtual void setConstantBuffers(GpuProgramType gptype, const SPtr<GpuParamsCore>& params) = 0;

		/** Binds all specified GPU program parameters. Caller must ensure these match the previously bound GPU program. */
		virtual void setGpuParams(GpuProgramType gptype, const SPtr<GpuParamsCore>& params);

		/**	Unbinds a program of a given type. */
		virtual void unbindGpuProgram(GpuProgramType gptype);

		/**	Query if a GPU program of a given type is currently bound. */
		virtual bool isGpuProgramBound(GpuProgramType gptype);

		/**	Sets up clip planes that will clip drawn geometry on the negative side of the planes. */
		virtual void setClipPlanes(const PlaneList& clipPlanes);

		/**	Adds a new clip plane. All drawn geometry will be clipped to this plane. */
		virtual void addClipPlane(const Plane& p);

		/**	Clears all clip planes. */
		virtual void resetClipPlanes();

		/**
		 * Allows you to set up a region in which rendering can take place. Coordinates are in pixels. No rendering will be
		 * done to render target pixels outside of the provided region.
		 */
		virtual void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) = 0;

		/**
		 * Clears the currently active render target.
		 *
		 * @param[in]	buffers		Combination of one or more elements of FrameBufferType denoting which buffers are 
		 *							to be cleared.
		 * @param[in]	color		(optional) The color to clear the color buffer with, if enabled.
		 * @param[in]	depth		(optional) The value to initialize the depth buffer with, if enabled.
		 * @param[in]	stencil		(optional) The value to initialize the stencil buffer with, if enabled.
		 * @param[in]	targetMask	(optional) In case multiple render targets are bound, this allows you to control
		 *							which ones to clear (0x01 first, 0x02 second, 0x04 third, etc., and combinations).
		 */
		virtual void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, 
			UINT16 stencil = 0, UINT8 targetMask = 0xFF) = 0;

		/**
		 * Clears the currently active viewport (meaning it clears just a sub-area of a render-target that is covered by the 
		 * viewport, as opposed to clearRenderTarget() which always clears the entire render target).
		 *
		 * @param[in]	buffers		Combination of one or more elements of FrameBufferType denoting which buffers are to be
		 *							cleared.
		 * @param[in]	color		(optional) The color to clear the color buffer with, if enabled.
		 * @param[in]	depth		(optional) The value to initialize the depth buffer with, if enabled.
		 * @param[in]	stencil		(optional) The value to initialize the stencil buffer with, if enabled.
		 * @param[in]	targetMask	(optional) In case multiple render targets are bound, this allows you to control
		 *							which ones to clear (0x01 first, 0x02 second, 0x04 third, etc., and combinations).
		 */
		virtual void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, 
			UINT16 stencil = 0, UINT8 targetMask = 0xFF) = 0;

		/**
		 * Change the render target into which we want to draw.
		 *
		 * @param[in]	target					Render target to draw to.
		 * @param[in]	readOnlyDepthStencil	If true the caller guarantees he won't write to the depth/stencil buffer 
		 *										(if any was provided). This allows the depth buffer to be bound for depth 
		 *										testing, as well as reading in a shader, at the same time.
		 */
        virtual void setRenderTarget(const SPtr<RenderTargetCore>& target, bool readOnlyDepthStencil = false) = 0;

		/**
		 * Returns information about available output devices and their video modes.
		 *
		 * @note	Thread safe.
		 */
		const VideoModeInfo& getVideoModeInfo() const { return *mVideoModeInfo; }

		/************************************************************************/
		/* 								UTILITY METHODS                    		*/
		/************************************************************************/

		/**
		 * Contains a default matrix into a matrix suitable for use by this specific render system.
		 *
		 * @note	Thread safe.
		 */
		virtual void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) = 0;

		/** 
		 * Returns information about the specific API implementation. 
		 *
		 * @note	Thread safe.
		 */
		virtual const RenderAPIInfo& getAPIInfo() const = 0;

		/**
		 * Generates a parameter block description and calculates per-parameter offsets for the provided gpu data 
		 * parameters. The offsets are render API specific and correspond to std140 layout for OpenGL, and the default 
		 * layout in DirectX.
		 * 			
		 * @param[in]	name	Name to assign the parameter block.
		 * @param[in]	params	List of parameters in the parameter block. Only name, type and array size fields need to be
		 * 						populated, the rest will be populated when the method returns.		
		 * @return				Descriptor for the parameter block holding the provided parameters as laid out by the 
		 *						default render API layout.
		 */
		virtual GpuParamBlockDesc generateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params) = 0;

		/************************************************************************/
		/* 							INTERNAL METHODS				        	*/
		/************************************************************************/
	protected:
		/**
		 * Initializes the render API system and creates a primary render window.
		 *
		 * @note	
		 * Although I'd like otherwise, due to the nature of some render API implementations, you cannot initialize the 
		 * render system without a window.
		 * @note
		 * Sim thread.
		 */
		RenderWindowPtr initialize(const RENDER_WINDOW_DESC& primaryWindowDesc);

		/**
		 * Prepares the initialization of the render API system on the core thread. After the system is prepared a render 
		 * window can be created and initialization finalized.
		 */
		virtual void initializePrepare();

		/**
		 * Finalizes the initialization of the render API system on the core thread. Should be called after the primary 
		 * render window is created.
		 */
		virtual void initializeFinalize(const SPtr<RenderWindowCore>& primaryWindow);

		/**
		 * Shuts down the render API system and cleans up all resources.
		 *
		 * @note	Sim thread.
		 */
		void destroy();

		/** Performs render API system shutdown on the core thread. */
		virtual void destroyCore();

		/** @copydoc setClipPlanes */
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes) = 0;

		/************************************************************************/
		/* 								INTERNAL DATA					       	*/
		/************************************************************************/
	protected:
		friend class RenderAPIManager;

		SPtr<RenderTargetCore> mActiveRenderTarget;

		DriverVersion mDriverVersion;
		CullingMode mCullingMode;
		UINT16 mDisabledTexUnitsFrom;

		bool mVertexProgramBound;
		bool mGeometryProgramBound;
		bool mFragmentProgramBound;
		bool mDomainProgramBound;
		bool mHullProgramBound;
		bool mComputeProgramBound;

		PlaneList mClipPlanes;
		bool mClipPlanesDirty;

		RenderAPICapabilities* mCurrentCapabilities;
		VideoModeInfoPtr mVideoModeInfo;
	};

	/** @endcond */
	/** @} */
}