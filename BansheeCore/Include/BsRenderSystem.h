#pragma once
#include "BsCorePrerequisites.h"

#include <memory>

#include "BsSamplerState.h"
#include "BsCommandQueue.h"
#include "BsDrawOps.h"
#include "BsRenderSystemCapabilities.h"
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
	/**
	 * @brief	Render system provides base functionality for a rendering API like
	 *			DirectX or OpenGL. Most of the class is abstract and specific
	 *			subclass for each rendering API needs to be implemented.
	 *
	 * @note	Core thread only unless specifically noted otherwise on per-method basis.
	 */
	class BS_CORE_EXPORT RenderSystem : public Module<RenderSystem>
	{
	public:
		RenderSystem();
		virtual ~RenderSystem();

		/**
		 * @brief	Returns the name of the rendering system. 
		 *
		 * @note	Thread safe.
		 */
		virtual const String& getName() const = 0;

		/**
		 * @brief	Gets the name of the primary shading language
		 *			used by the rendering system.
		 *
		 * @note	Thread safe.
		 */
		virtual const String& getShadingLanguageName() const = 0;

		/**
		 * @brief	Sets a sampler state for the specified texture unit.
		 *
		 * @see		SamplerState
		 */
		virtual void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState) = 0;

		/**
		 * @brief	Sets a blend state used for all active render targets.
		 *
		 * @see		BlendState
		 */
		virtual void setBlendState(const BlendStatePtr& blendState) = 0;

		/**
		 * @brief	Sets a state that controls various rasterizer options. 
		 *
		 * @see		RasterizerState
		 */
		virtual void setRasterizerState(const RasterizerStatePtr& rasterizerState) = 0;

		/**
		 * @brief	Sets a state that controls depth & stencil buffer options.
		 *
		 * @see		DepthStencilState
		 */
		virtual void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue) = 0;

		/**
		 * @brief	Binds a texture to the pipeline for the specified GPU program type at the specified slot.
		 *			If the slot matches the one configured in the GPU program the program will be able to access
		 *			this texture on the GPU.
		 */
		virtual void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr& texPtr) = 0;

		/**
		* @brief	Turns off a texture unit.
		*/
		virtual void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/**
		 * @brief	Binds a texture that can be used for random load/store operations from a GPU program.
		 */
		virtual void setLoadStoreTexture(GpuProgramType gptype, UINT16 unit, bool enabled, 
			const TexturePtr& texPtr, const TextureSurface& surface) = 0;

		/**
		 * @brief	Signals that rendering for a specific viewport has started. Any draw calls
		 *			need to be called between beginFrame and endFrame. You may not switch render targets
		 *			until you call endFrame.
		 */
		virtual void beginFrame() = 0;
		
		/**
		 * @brief	Ends that rendering to a specific viewport has ended.
		 */
		virtual void endFrame() = 0;

		/**
		 * @brief	Sets the active viewport that will be used for all render operations.
		 *
		 * @param	area	Area of the viewport, in normalized ([0,1] range) coordinates.
		 */
		virtual void setViewport(const Rect2& area) = 0;

		/**
		 * @brief	Sets the provided vertex buffers starting at the specified source index.
		 *			Set buffer to nullptr to clear the buffer at the specified index.
		 */
		virtual void setVertexBuffers(UINT32 index, SPtr<VertexBufferCore>* buffers, UINT32 numBuffers) = 0;

		/**
		 * @brief	Sets an index buffer to use when drawing. Indices in an index buffer
		 * 			reference vertices in the vertex buffer, which increases cache coherency
		 * 			and reduces the size of vertex buffers by eliminating duplicate data.
		 */
		virtual void setIndexBuffer(const SPtr<IndexBufferCore>& buffer) = 0;

		/**
		 * @brief	Sets the vertex declaration to use when drawing. Vertex declaration
		 * 			is used to decode contents of a single vertex in a vertex buffer.
		 */
		virtual void setVertexDeclaration(VertexDeclarationPtr vertexDeclaration) = 0;

		/**
		 * @brief	Sets the draw operation that determines how to interpret the elements
		 * 			of the index or vertex buffers.
		 */
		virtual void setDrawOperation(DrawOperationType op) = 0;

		/**
		 * @brief	Draw an object based on currently bound GPU programs, vertex declaration and vertex buffers.
		 * 			
		 *			Draws directly from the vertex buffer without using indices.
		 */
		virtual void draw(UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/**
		 * @brief	Draw an object based on currently bound GPU programs, vertex declaration, vertex 
		 * 			and index buffers.
		 */
		virtual void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/**
		 * @brief	Swap the front and back buffer of the specified render target.
		 */
		virtual void swapBuffers(const SPtr<RenderTargetCore>& target);

		/**
		 * @brief	Gets the capabilities of the render system.
		 *
		 * @note	Thread safe.
		 */
		const RenderSystemCapabilities* getCapabilities() const;

		/**
		 * @brief	Returns information about the driver version.
		 */
		virtual const DriverVersion& getDriverVersion() const;

		/**
		 * @brief	Binds the provided GPU program to the pipeline. Any following
		 *			draw operations will use this program. 
		 *
		 * @note	You need to bind at least a vertex and a fragment program in order to draw something.
		 */
		virtual void bindGpuProgram(HGpuProgram prg);

		/**
		 * @brief	Binds GPU program parameters. Caller must ensure these match the previously
		 *			bound GPU program.
		 */
		virtual void bindGpuParams(GpuProgramType gptype, GpuParamsPtr params) = 0;

		/**
		 * @brief	Unbinds a program of a given type. 
		 */
		virtual void unbindGpuProgram(GpuProgramType gptype);

		/**
		 * @brief	Query if a GPU program of a given type is currently bound.
		 */
		virtual bool isGpuProgramBound(GpuProgramType gptype);

		/**
		 * @brief	Sets up clip planes that will clip drawn geometry on the negative side of the planes.
		 */
		virtual void setClipPlanes(const PlaneList& clipPlanes);

		/**
		 * @brief	Adds a new clip plane. All drawn geometry will be clipped to this plane.
		 */
		virtual void addClipPlane(const Plane& p);

		/**
		 * @brief	Clears all clip planes.
		 */
		virtual void resetClipPlanes();

		/**
		 * @brief	Allows you to set up a region in which rendering can take place. Coordinates are in pixels.
		 *			No rendering will be done to render target pixels outside of the provided region.
		 */
		virtual void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) = 0;

		/**
		 * @brief	Clears the currently active render target.
		 *
		 * @param	buffers			Combination of one or more elements of FrameBufferType
		 *							denoting which buffers are to be cleared.
		 * @param	color			(optional) The color to clear the color buffer with, if enabled.
		 * @param	depth			(optional) The value to initialize the depth buffer with, if enabled.
		 * @param	stencil			(optional) The value to initialize the stencil buffer with, if enabled.
		 */
		virtual void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) = 0;

		/**
		 * @brief	Clears the currently active viewport (i.e. it clears just a sub-area of a render-target that is covered by the viewport,
		 * 			as opposed to clearRenderTarget which always clears the entire render target).
		 *
		 * @param	buffers			Combination of one or more elements of FrameBufferType
		 *							denoting which buffers are to be cleared.
		 * @param	color			(optional) The color to clear the color buffer with, if enabled.
		 * @param	depth			(optional) The value to initialize the depth buffer with, if enabled.
		 * @param	stencil			(optional) The value to initialize the stencil buffer with, if enabled.
		 */
		virtual void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) = 0;

		/**
		 * @brief	Change the render target into which we want to draw.
		 */
        virtual void setRenderTarget(const SPtr<RenderTargetCore>& target) = 0;

		/**
		 * @brief	Updates the resource with the specified data.
		 *
		 * @note	It is assumed GpuResourceData has been locked before being passed here. Data will be unlocked
		 *			when this method finishes.
		 */
		void writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data, bool discardEntireBuffer, AsyncOp& asyncOp);

		/**
		 * @brief	Reads data from a resource into a pre-allocated GpuResourceData instance.
		 *
		 * @note	It is assumed GpuResourceData has been locked before being passed here. Data will be unlocked
		 *			when this method finishes.
		 */
		void readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, GpuResourceDataPtr& data, AsyncOp& asyncOp);

		/**
		 * @brief	Returns information about available output devices and their video modes.
		 *
		 * @note	Thread safe.
		 */
		const VideoModeInfo& getVideoModeInfo() const { return *mVideoModeInfo; }

		/************************************************************************/
		/* 								UTILITY METHODS                    		*/
		/************************************************************************/

		/**
		 * @brief	Gets the native type used for vertex colors.
		 *
		 * @note	Thread safe.
		 */
		virtual VertexElementType getColorVertexElementType() const = 0;

		/**
		 * @brief	Contains a default matrix into a matrix suitable for use
		 *			by this specific render system.
		 *
		 * @note	Thread safe.
		 */
		virtual void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) = 0;

		/**
		 * @brief	Gets horizontal texel offset used for mapping texels to pixels
		 *			in this render system.
		 *
		 * @note	Thread safe.
		 */
		virtual float getHorizontalTexelOffset() = 0;

		/**
		 * @brief	Gets vertical texel offset used for mapping texels to pixels
		 *			in this render system.
		 *
		 * @note		Thread safe.
		 */
		virtual float getVerticalTexelOffset() = 0;

		/**
		 * @brief	Gets the minimum (closest) depth value used by this
		 *			render system.
		 *
		 * @note	Thread safe.
		 */
		virtual float getMinimumDepthInputValue() = 0;

		/**
		 * @brief	Gets the maximum (farthest) depth value used by this
		 *			render system.
		 *
		 * @note	Thread safe.
		 */
		virtual float getMaximumDepthInputValue() = 0;

		/**
		 * @brief	Checks if vertex color needs to be flipped before sent to the shader.
		 *
		 * @note	Thread safe.
		 */
		virtual bool getVertexColorFlipRequired() const { return false; }

		/************************************************************************/
		/* 							INTERNAL METHODS				        	*/
		/************************************************************************/
	protected:
		/**
		 * @brief	Initializes the render system and creates a primary render window.
		 *
		 * @note		Although I'd like otherwise, due to the nature of some render system implementations,
		 * 			you cannot initialize the render system without a window.
		 *
		 *			Sim thread.
		 */
		RenderWindowPtr initialize(const RENDER_WINDOW_DESC& primaryWindowDesc);

		/**
		 * @brief	Performs second part of the initialization (on Core thread), first part
		 *			being in initialize().
		 */
		virtual void initialize_internal(AsyncOp& asyncOp);

		/**
		 * @brief	Shuts down the render system and cleans up all resources.
		 *
		 * @note	Sim thread.
		 */
		void destroy();

		/**
		 * @brief	Performs second part of render system shutdown on the core thread.
		 */
		virtual void destroy_internal();

		/**
		 * @copydoc	setClipPlanes.
		 */
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes) = 0;

		/************************************************************************/
		/* 								INTERNAL DATA					       	*/
		/************************************************************************/
	protected:
		friend class RenderSystemManager;

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

		RenderSystemCapabilities* mCurrentCapabilities;
		VideoModeInfoPtr mVideoModeInfo;

		// TODO - Only used between initialize and initialize_internal. Handle it better?
		RENDER_WINDOW_DESC mPrimaryWindowDesc;
	};
}