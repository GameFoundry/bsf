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
#include "CmCommon.h"

#include "CmRenderOperation.h"
#include "CmRenderSystemCapabilities.h"
#include "CmRenderTarget.h"
#include "CmRenderTexture.h"
#include "CmGpuProgram.h"
#include "CmPlane.h"
#include "CmModule.h"

#include "boost/function.hpp"
#include "boost/signal.hpp"

namespace CamelotEngine
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	typedef multimap<UINT8, RenderTarget * >::type RenderTargetPriorityMap;

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

		/** Attaches the passed render target to the render system.
		*/
		virtual void attachRenderTarget(RenderTarget &target);

		/** Detaches the render target from the render system.
		@note
		If the render target cannot be found, NULL is returned.
		*/
		virtual void detachRenderTarget(RenderTarget& renderTarget);

		/** Destroys a render window */
		virtual void destroyRenderWindow(RenderWindow* renderWindow);
		/** Destroys a render texture */
		virtual void destroyRenderTexture(RenderTexture* renderTexture);
		/** Destroys a render target of any sort */
		virtual void destroyRenderTarget(RenderTarget* renderTarget);

		/** Defines whether or now fullscreen render windows wait for the vertical blank before flipping buffers.
		@remarks
		By default, all rendering windows wait for a vertical blank (when the CRT beam turns off briefly to move
		from the bottom right of the screen back to the top left) before flipping the screen buffers. This ensures
		that the image you see on the screen is steady. However it restricts the frame rate to the refresh rate of
		the monitor, and can slow the frame rate down. You can speed this up by not waiting for the blank, but
		this has the downside of introducing 'tearing' artefacts where part of the previous frame is still displayed
		as the buffers are switched. Speed vs quality, you choose.
		@note
		Has NO effect on windowed mode render targets. Only affects fullscreen mode.
		@param
		enabled If true, the system waits for vertical blanks - quality over speed. If false it doesn't - speed over quality.
		*/
		void setWaitForVerticalBlank(bool enabled);

		/** Returns true if the system is synchronising frames with the monitor vertical blank.
		*/
		bool getWaitForVerticalBlank(void) const;

		/**
		 * @brief	Sets a sampler state for the specified texture unit.
		 * @see		SamplerState
		 */
		virtual void setSamplerState(UINT16 texUnit, const SamplerState& samplerState) = 0;
		/** Turns off a texture unit. */
		virtual void disableTextureUnit(UINT16 texUnit);
		/** Disables all texture units from the given unit upwards */
		virtual void disableTextureUnitsFrom(UINT16 texUnit);

		/**
		 * @brief	Sets a blend state used for all active render targets.
		 * @see		BlendState
		 */
		virtual void setBlendState(const BlendState& blendState) = 0;

		/**
		 * @brief	Sets a state that controls various rasterizer options. 
		 * @see		RasterizerState
		 */
		virtual void setRasterizerState(const RasterizerState& rasterizerState) = 0;

		/**
		 * @brief	Sets a state that controls depth & stencil buffer options.
		 * @see		DepthStencilState
		 */
		virtual void setDepthStencilState(const DepthStencilState& depthStencilState) = 0;

		/**
		 * @brief	Sets a reference values used for stencil buffer comparisons. 
		 * 			Actual comparison function and stencil operations are set by setting the StencilState.
		 * 			
		 * @remarks
		 *		 The stencil buffer is used to mask out pixels in the render target, allowing
		 *		 you to do effects like mirrors, cut-outs, stencil shadows and more. Each of
		 *		 your batches of rendering is likely to ignore the stencil buffer,
		 *		 update it with new values, or apply it to mask the output of the render.
		 *		 The stencil test is:<PRE>
		 *		 (Reference Value & Mask) CompareFunction (Stencil Buffer Value & Mask)</PRE>
		 *		 The result of this will cause one of 3 actions depending on whether the test fails,
		 *		 succeeds but with the depth buffer check still failing, or succeeds with the
		 *		 depth buffer check passing too.
		 */
		virtual void setStencilRefValue(UINT32 refValue) = 0;

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
		virtual void setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr) = 0;

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
		virtual void setViewport(const Viewport& vp) = 0;
		/** Get the current active viewport for rendering. */
		virtual Viewport getViewport(void);

		/** Sets the current vertex declaration, ie the source of vertex data. */
		virtual void setVertexDeclaration(VertexDeclarationPtr decl) = 0;
		/** Sets the current vertex buffer binding state. */
		virtual void setVertexBufferBinding(VertexBufferBinding* binding) = 0;

		/**
		Render something to the active viewport.

		Low-level rendering interface to perform rendering
		operations. Unlikely to be used directly by client
		applications, since the SceneManager and various support
		classes will be responsible for calling this method.
		Can only be called between _beginScene and _endScene

		@param op A rendering operation instance, which contains
		details of the operation to be performed.
		*/
		virtual void render(const RenderOperation& op);

		/** Gets the capabilities of the render system. */
		const RenderSystemCapabilities* getCapabilities(void) const;

		/** Returns the driver version.
		*/
		virtual const DriverVersion& getDriverVersion(void) const;

		/** Binds a given GpuProgram (but not the parameters). 
		@remarks Only one GpuProgram of each type can be bound at once, binding another
		one will simply replace the existing one.
		*/
		virtual void bindGpuProgram(GpuProgramHandle prg);

		/** Bind Gpu program parameters.
		@param gptype The type of program to bind the parameters to
		@param params The parameters to bind
		@param variabilityMask A mask of GpuParamVariability identifying which params need binding
		*/
		virtual void bindGpuProgramParameters(GpuProgramType gptype, 
			GpuProgramParametersSharedPtr params, UINT16 variabilityMask) = 0;

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
		/** Add a user clipping plane. */
		virtual void addClipPlane (float A, float B, float C, float D);

		/** Clears the user clipping region.
		*/
		virtual void resetClipPlanes();

		/** Internal method for swapping all the buffers on all render targets,
		if _updateAllRenderTargets was called with a 'false' parameter. */
		virtual void swapAllRenderTargetBuffers(bool waitForVsync = true);

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
		virtual void setScissorRect(UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600) = 0;

		/** Clears one or more frame buffers on the active render target. 
		@param buffers Combination of one or more elements of FrameBufferType
		denoting which buffers are to be cleared
		@param colour The colour to clear the colour buffer with, if enabled
		@param depth The value to initialise the depth buffer with, if enabled
		@param stencil The value to initialise the stencil buffer with, if enabled.
		*/
		virtual void clearFrameBuffer(unsigned int buffers, 
			const Color& color = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0) = 0;

		/**
         * Set current render target to target, enabling its device context if needed
         */
        virtual void setRenderTarget(RenderTarget* target) = 0;

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
		back a generic Camelot matrix for storage in the engine.
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

		/** The render targets. */
		vector<RenderTarget*>::type mRenderTargets;
		/** The render targets, ordered by priority. */
		RenderTargetPriorityMap mPrioritisedRenderTargets;
		/** The Active render target. */
		RenderTarget* mActiveRenderTarget;
		/** The Active GPU programs and gpu program parameters*/
		GpuProgramParametersSharedPtr mActiveVertexGpuProgramParameters;
		GpuProgramParametersSharedPtr mActiveGeometryGpuProgramParameters;
		GpuProgramParametersSharedPtr mActiveFragmentGpuProgramParameters;

		// Active viewport (dest for future rendering operations)
		Viewport mActiveViewport;

		CullingMode mCullingMode;

		bool mVsync;
		unsigned int mVSyncInterval;

		bool mInvertVertexWinding;

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

		/**
		 * @brief	Call right after creation to properly initialize the RenderSystem;
		 */
		void initialize();
		virtual void initialize_internal();
		virtual void destroy_internal();

		/// Internal method used to set the underlying clip planes when needed
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes) = 0;

		/** Query the real capabilities of the GPU and driver in the RenderSystem*/
		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const = 0;

		/** Initialize the render system from the capabilities*/
		virtual void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps) = 0;

		/**	Create a MultiRenderTarget, which is a render target that renders to multiple RenderTextures
		at once. Surfaces can be bound and unbound at will.
		This fails if mCapabilities->getNumMultiRenderTargets() is smaller than 2.
		*/
		virtual MultiRenderTarget * createMultiRenderTarget(const String & name) = 0; 

		/** Returns a description of an error code.
		*/
		virtual String getErrorDescription(long errorNumber) const = 0;

		DriverVersion mDriverVersion;

		/************************************************************************/
		/* 								THREADING	                     		*/
		/************************************************************************/

		class RenderWorkerFunc CM_THREAD_WORKER_INHERIT
		{
		public:
			RenderWorkerFunc(RenderSystem* rs);

			void operator()();

		private:
			RenderSystem* mRS;
		};

		RenderWorkerFunc* mRenderThreadFunc;
		bool mRenderThreadShutdown;

		CM_THREAD_ID_TYPE mRenderThreadId;
		CM_THREAD_SYNCHRONISER(mRenderThreadStartCondition)
		CM_MUTEX(mRenderThreadStartMutex)
		CM_MUTEX(mCommandQueueMutex)
		CM_THREAD_SYNCHRONISER(mCommandReadyCondition)
		CM_MUTEX(mCommandNotifyMutex)
		CM_THREAD_SYNCHRONISER(mCommandCompleteCondition)

#if CM_THREAD_SUPPORT
		CM_THREAD_TYPE* mRenderThread;
#endif

		CommandQueue* mCommandQueue;

		UINT32 mMaxCommandNotifyId; // ID that will be assigned to the next command with a notifier callback
		vector<UINT32>::type mCommandsCompleted; // Completed commands that have notifier callbacks set up

		/**
		 * @brief	Initializes a separate render thread. Should only be called once.
		 */
		void initRenderThread();

		/**
		 * @brief	Main function of the render thread. Called once thread is started.
		 */
		void runRenderThread();

		/**
		 * @brief	Shutdowns the render thread. It will complete all ready commands
		 * 			before shutdown.
		 */
		void shutdownRenderThread();

		/**
		 * @brief	Throws an exception if current thread isn't the render thread;
		 */
		void throwIfNotRenderThread() const;

		/**
		 * @brief	Blocks the calling thread until the command with the specified ID completes.
		 * 			Make sure that the specified ID actually exists, otherwise this will block forever.
		 */
		void blockUntilCommandCompleted(UINT32 commandId);

		/**
		 * @brief	Callback called by the command list when a specific command finishes executing.
		 * 			This is only called on commands that have a special notify on complete flag set.
		 *
		 * @param	commandId	Identifier for the command.
		 */
		void commandCompletedNotify(UINT32 commandId);

	public:
		/**
		 * @brief	Returns the id of the render thread. If a separate render thread
		 * 			is not used, then it returns the id of the thread RenderSystem
		 * 			was initialized on.
		 */
		CM_THREAD_ID_TYPE getRenderThreadId() const { return mRenderThreadId; }

		/**
		 * @brief	Creates a new render system context that you can use for rendering on 
		 * 			a non-render thread. You can have as many of these as you wish, the only limitation
		 * 			is that you do not use a single instance on more than one thread. Each thread
		 * 			requires its own context. The context will be bound to the thread you call this method on.
		 */
		DeferredRenderContextPtr createDeferredContext();

		/**
		 * @brief	Queues a new command that will be added to the global command queue. You are allowed to call this from any thread,
		 * 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		 * 			
		 * @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there be many commands queued before it
		 * 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		 * 	
		 * @see		CommandQueue::queueReturn
		 */
		AsyncOp queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete = false);

		/**
		* @brief	Queues a new command that will be added to the global command queue.You are allowed to call this from any thread,
		 * 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		 * 	
		 * @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there be many commands queued before it
		 * 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		 * @see		CommandQueue::queue
		 */
		void queueCommand(boost::function<void()> commandCallback, bool blockUntilComplete = false);
	};
	/** @} */
	/** @} */
}

#endif
