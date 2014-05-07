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

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __RenderSystem_H_
#define __RenderSystem_H_

// Precompiler options
#include "CmPrerequisites.h"

#include <memory>

#include "CmString.h"

#include "CmSamplerState.h"
#include "CmCommonEnums.h"

#include "CmCommandQueue.h"
#include "CmDrawOps.h"
#include "CmRenderSystemCapabilities.h"
#include "CmRenderTarget.h"
#include "CmRenderTexture.h"
#include "CmRenderWindow.h"
#include "CmGpuProgram.h"
#include "CmVertexDeclaration.h"
#include "CmPlane.h"
#include "CmModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	typedef MultiMap<UINT8, RenderTarget * > RenderTargetPriorityMap;

	class TextureManager;

	/** Defines the functionality of a 3D API
	@remarks
	The RenderSystem class provides a base interface
	which abstracts the general functionality of the 3D API
	e.g. Direct3D or OpenGL. Whilst a few of the general
	methods have implementations, most of this class is
	abstract, requiring a subclass based on a specific API
	to be constructed to provide the full functionality.
	@author
	Steven Streeting
	@version
	1.0
	*/
	class CM_EXPORT RenderSystem : public Module<RenderSystem>
	{
	public:
		/** Default Constructor.
		*/
		RenderSystem();

		/** Destructor.
		*/
		virtual ~RenderSystem();

		/** Returns the name of the rendering system.
		*/
		virtual const String& getName(void) const = 0;

		/**
		 * @brief	Gets the name of the primary shading language.
		 */
		virtual const String& getShadingLanguageName() const = 0;

		/**
		 * @brief	Sets a sampler state for the specified texture unit.
		 * @see		SamplerState
		 */
		virtual void setSamplerState(GpuProgramType gptype, UINT16 texUnit, const SamplerStatePtr& samplerState) = 0;
		/** Turns off a texture unit. */
		virtual void disableTextureUnit(GpuProgramType gptype, UINT16 texUnit);

		/**
		 * @brief	Sets a blend state used for all active render targets.
		 * @see		BlendState
		 */
		virtual void setBlendState(const BlendStatePtr& blendState) = 0;

		/**
		 * @brief	Sets a state that controls various rasterizer options. 
		 * @see		RasterizerState
		 */
		virtual void setRasterizerState(const RasterizerStatePtr& rasterizerState) = 0;

		/**
		 * @brief	Sets a state that controls depth & stencil buffer options.
		 * @see		DepthStencilState
		 */
		virtual void setDepthStencilState(const DepthStencilStatePtr& depthStencilState, UINT32 stencilRefValue) = 0;

		/**
		Sets the texture to bind to a given texture unit.

		User processes would not normally call this direct unless rendering
		primitives themselves.

		@param unit The index of the texture unit to modify. Multitexturing 
		hardware can support multiple units (see 
		RenderSystemCapabilites::getNumTextureUnits)
		@param enabled Boolean to turn the unit on/off
		@param texPtr Pointer to the texture to use.
		*/
		virtual void setTexture(GpuProgramType gptype, UINT16 unit, bool enabled, const TexturePtr &texPtr) = 0;

		/**
		* Signifies the beginning of a frame, i.e. the start of rendering on a single viewport. Will occur
		* several times per complete frame if multiple viewports exist.
		*/
		virtual void beginFrame(void) = 0;
		
		/**
		* Ends rendering of a frame to the current viewport.
		*/
		virtual void endFrame(void) = 0;
		/**
		Sets the provided viewport as the active one for future
		rendering operations. This viewport is aware of it's own
		camera and render target. Must be implemented by subclass.

		@param target Viewport to render to.
		*/
		virtual void setViewport(const ViewportPtr& vp) = 0;

		/** Sets the provided vertex buffers starting at the specified source index.   
		/** @note Set buffer to nullptr to clear the buffer at the specified index.*/
		virtual void setVertexBuffers(UINT32 index, VertexBufferPtr* buffers, UINT32 numBuffers) = 0;

		/**
		 * @brief	Sets an index buffer to use when drawing. Indices in an index buffer
		 * 			reference vertices in the vertex buffer, which increases cache coherency
		 * 			and reduces the size of vertex buffers by eliminating duplicate data.
		 */
		virtual void setIndexBuffer(const IndexBufferPtr& buffer) = 0;

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
		 * @brief	A helper method that provides a simple way of rendering a single object. It will
		 * 			automatically set up vertex declaration, draw operation, vertex and index buffers and
		 * 			draw them.
		 *
		 * @param	mesh	   	The mesh.
		 * @param	indexOffset	(optional) Offset into the mesh buffer to start drawing from.
		 * @param	indexCount 	(optional) Number of indexes to draw, starting at the offset. Ignored if "drawIndexed" is false. If 0 all indices in the mesh will be drawn.
		 * @param	useIndices	(optional) If true, drawing is done using the index buffer on the mesh and the provided offset and size, otherwise all mesh vertices are drawn sequentially.
		 * @param	drawOp	   	(optional) Draw operation to use when rendering.
		 */
		virtual void render(const MeshBasePtr& mesh, UINT32 indexOffset = 0, UINT32 indexCount = 0, bool useIndices = true, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Draw an object based on currently set
		 * 			shaders, vertex declaration index buffers.
		 * 			
		 *			Draws directly from the vertex buffer without using
		 *			indices.
		 */
		virtual void draw(UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/**
		 * @brief	Draw an object based on currently set
		 * 			shaders, vertex declaration and vertex 
		 * 			and index buffers.
		 */
		virtual void drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount) = 0;

		/**
		 * @brief	Swap the front and back buffer of the specified render target.
		 */
		virtual void swapBuffers(RenderTargetPtr target);

		/**
		 * @brief	Gets the capabilities of the render system.
		 *
		 * @note	Thread safe.
		 */
		const RenderSystemCapabilities* getCapabilities() const;

		/** Returns the driver version.
		*/
		virtual const DriverVersion& getDriverVersion() const;

		/** Binds a given GpuProgram (but not the parameters). 
		@remarks Only one GpuProgram of each type can be bound at once, binding another
		one will simply replace the existing one.
		*/
		virtual void bindGpuProgram(HGpuProgram prg);

		/** Bind Gpu program parameters.
		@param gptype The type of program to bind the parameters to
		@param params The parameters to bind
		*/
		virtual void bindGpuParams(GpuProgramType gptype, BindableGpuParams& params) = 0;

		/** Unbinds GpuPrograms of a given GpuProgramType.
		@remarks
		This returns the pipeline to fixed-function processing for this type.
		*/
		virtual void unbindGpuProgram(GpuProgramType gptype);

		/** Returns whether or not a Gpu program of the given type is currently bound. */
		virtual bool isGpuProgramBound(GpuProgramType gptype);

		/** Sets the user clipping region.
		*/
		virtual void setClipPlanes(const PlaneList& clipPlanes);

		/** Add a user clipping plane. */
		virtual void addClipPlane (const Plane& p);

		/** Clears the user clipping region.
		*/
		virtual void resetClipPlanes();

		/** Sets the 'scissor region' ie the region of the target in which rendering can take place.
		@remarks
		This method allows you to 'mask off' rendering in all but a given rectangular area
		as identified by the parameters to this method.
		@note
		Not all systems support this method. Check the RenderSystemCapabilities for the
		RSC_SCISSOR_TEST capability to see if it is supported.
		@param left, top, right, bottom The location of the corners of the rectangle, expressed in
		<i>pixels</i>.
		*/
		virtual void setScissorRect(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom) = 0;

		/**
		 * @brief	Clears the currently active render target.
		 *
		 * @param	buffers			Combination of one or more elements of FrameBufferType
		 *							denoting which buffers are to be cleared.
		 * @param	color			(optional) The color to clear the color buffer with, if enabled.
		 * @param	depth			(optional) The value to initialise the depth buffer with, if enabled.
		 * @param	stencil			(optional) The value to initialise the stencil buffer with, if enabled.
		 */
		virtual void clearRenderTarget(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) = 0;

		/**
		 * @brief	Clears the currently active viewport (i.e. it clears just a sub-area of a render-target that is covered by the viewport,
		 * 			as opposed to clearRenderTarget which always clears the entire render target).
		 *
		 * @param	buffers			Combination of one or more elements of FrameBufferType
		 *							denoting which buffers are to be cleared.
		 * @param	color			(optional) The color to clear the color buffer with, if enabled.
		 * @param	depth			(optional) The value to initialise the depth buffer with, if enabled.
		 * @param	stencil			(optional) The value to initialise the stencil buffer with, if enabled.
		 */
		virtual void clearViewport(UINT32 buffers, const Color& color = Color::Black, float depth = 1.0f, UINT16 stencil = 0) = 0;

		/**
         * Set current render target to target, enabling its device context if needed
         */
        virtual void setRenderTarget(RenderTargetPtr target) = 0;

		/**
		 * @brief	Updates the resource with the specified data.
		 */
		void writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data, bool discardEntireBuffer, AsyncOp& asyncOp);

		/**
		 * @brief	Reads data from a resource into a pre-allocated GpuResourceData instance.
		 */
		void readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, GpuResourceDataPtr& data, AsyncOp& asyncOp);

		/************************************************************************/
		/* 								UTILITY METHODS                    		*/
		/************************************************************************/

		/** Get the native VertexElementType for a compact 32-bit colour value
		for this rendersystem.
		*/
		virtual VertexElementType getColorVertexElementType(void) const = 0;

		/** Converts a uniform projection matrix to suitable for this render system.
		@remarks
		Because different APIs have different requirements (some incompatible) for the
		projection matrix, this method allows each to implement their own correctly and pass
		back a generic Banshee matrix for storage in the engine.
		*/
		virtual void convertProjectionMatrix(const Matrix4& matrix,
			Matrix4& dest, bool forGpuProgram = false) = 0;

		/** Returns the horizontal texel offset value required for mapping 
		texel origins to pixel origins in this rendersystem.
		@remarks
		Since rendersystems sometimes disagree on the origin of a texel, 
		mapping from texels to pixels can sometimes be problematic to 
		implement generically. This method allows you to retrieve the offset
		required to map the origin of a texel to the origin of a pixel in
		the horizontal direction.
		*/
		virtual float getHorizontalTexelOffset(void) = 0;

		/** Returns the vertical texel offset value required for mapping 
		texel origins to pixel origins in this rendersystem.
		@remarks
		Since rendersystems sometimes disagree on the origin of a texel, 
		mapping from texels to pixels can sometimes be problematic to 
		implement generically. This method allows you to retrieve the offset
		required to map the origin of a texel to the origin of a pixel in
		the vertical direction.
		*/
		virtual float getVerticalTexelOffset(void) = 0;

		/** Gets the minimum (closest) depth value to be used when rendering
		using identity transforms.
		@remarks
		When using identity transforms you can manually set the depth
		of a vertex; however the input values required differ per
		rendersystem. This method lets you retrieve the correct value.
		@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		*/
		virtual float getMinimumDepthInputValue(void) = 0;

		/** Gets the maximum (farthest) depth value to be used when rendering
		using identity transforms.
		@remarks
		When using identity transforms you can manually set the depth
		of a vertex; however the input values required differ per
		rendersystem. This method lets you retrieve the correct value.
		@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		*/
		virtual float getMaximumDepthInputValue(void) = 0;

		/************************************************************************/
		/* 						INTERNAL DATA & METHODS                      	*/
		/************************************************************************/
	protected:
		friend class RenderSystemManager;

		/** The Active render target. */
		RenderTargetPtr mActiveRenderTarget;

		CullingMode mCullingMode;

		/// Texture units from this upwards are disabled
		UINT16 mDisabledTexUnitsFrom;

		bool mVertexProgramBound;
		bool mGeometryProgramBound;
		bool mFragmentProgramBound;

		// Recording user clip planes
		PlaneList mClipPlanes;
		// Indicator that we need to re-set the clip planes on next render call
		bool mClipPlanesDirty;

		/// Used to store the capabilities of the graphics card
		RenderSystemCapabilities* mCurrentCapabilities;

		// TODO - Only used between initialize and initialize_internal. Handle it better?
		RENDER_WINDOW_DESC mPrimaryWindowDesc;

		/**
		 * @brief	Initializes the render system and creates a primary render window.
		 * 			
		 * @note	Although I'd like otherwise, due to the nature of some render system implementations, 
		 * 			you cannot initialize the render system without a window.
		 */
		RenderWindowPtr initialize(const RENDER_WINDOW_DESC& primaryWindowDesc);
		virtual void initialize_internal(AsyncOp& asyncOp);

		void destroy();
		virtual void destroy_internal();

		/// Internal method used to set the underlying clip planes when needed
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes) = 0;

		/** Query the real capabilities of the GPU and driver in the RenderSystem*/
		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const = 0;

		/** Initialize the render system from the capabilities*/
		virtual void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps) = 0;

		/** Returns a description of an error code.
		*/
		virtual String getErrorDescription(long errorNumber) const = 0;

		DriverVersion mDriverVersion;
	};
	/** @} */
	/** @} */
}

#endif
