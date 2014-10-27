#pragma once

#include "BsCorePrerequisites.h"
#include "BsPixelUtil.h"
#include "BsViewport.h"
#include "BsCoreObject.h"
#include "BsEvent.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Structure that contains information about
	 *			what part of the texture represents the render surface.
	 */
	struct BS_CORE_EXPORT RENDER_SURFACE_DESC
	{
		TexturePtr texture;
		UINT32 face;
		UINT32 mipLevel;
	};

	/**
	 * @brief	Contains various properties that describe a render target.
	 */
	class BS_CORE_EXPORT RenderTargetProperties
	{
	public:
		virtual ~RenderTargetProperties() { }

		/**
		 * @brief	Returns a name of the render target, used for easier identification.
		 */
        const String& getName() const { return mName; }

		/**
		 * @brief	Returns width of the render target, in pixels.
		 * 
		 * @note	Sim thread only.
		 */
        UINT32 getWidth() const { return mWidth; }

		/**
		 * @brief	Returns height of the render target, in pixels.
		 *
		 * @note	Sim thread only.
		 */
        UINT32 getHeight() const { return mHeight; }

		/**
		 * @brief	Gets the number of samples used for multisampling.
		 *			(0 if multisampling is not used).
		 */
		UINT32 getMultisampleCount() const { return mMultisampleCount; }

		/**
		 * @brief	Get a render-system specific hint used for determining
		 *			multisampling type.
		 */
		const String& getMultisampleHint() const { return mMultisampleHint; }

		/**
		 * @brief	Returns true if the render target will wait for vertical sync 
		 *			before swapping buffers. This will eliminate tearing but may increase
		 *			input latency.
		 */
		bool getVSync() const { return mVSync; }

		/**
		 * @brief	Returns how often should the frame be presented in respect to
		 *			display device refresh rate. Normal value is 1 where it will
		 *			match the refresh rate. Higher values will decrease the frame
		 *			rate (e.g. present interval of 2 on 60Hz refresh rate will display
		 *			at most 30 frames per second).
		 */
		UINT32 getVSyncInterval() const { return mVSyncInterval; }

		/**
		 * @brief	Returns true if pixels written to the render target will be gamma corrected.
		 */
		bool isHwGammaEnabled() const { return mHwGamma; }

		/**
		 * @brief	Returns true if the render target can be used for rendering.
		 *
		 * @note	Core thread only.
		 */
		bool isActive() const { return mActive; }

		/**
		 * @brief	Returns render target priority. Targets with higher priority will be 
		 *			rendered before ones with lower priority.
		 */
		INT32 getPriority() const { return mPriority; }

		/**
		 * @brief	Returns true if the render target is a render window.
		 */
		bool isWindow() const { return mIsWindow; }

		/**
		 * @brief	Does the texture need to be vertically flipped because of different screen space coordinate systems.
		 *			(i.e. is origin top left or bottom left. Engine default is top left.)
		 */
		bool requiresTextureFlipping() const { return mRequiresTextureFlipping; }

		/**
		 * @brief	Copies all data from the provided object to this object.
		 */
		virtual void copyFrom(const RenderTargetProperties& other);

	protected:
		friend class RenderTargetCore;
		friend class RenderTarget;

		String mName;

		UINT32 mWidth = 0;
		UINT32 mHeight = 0;
		UINT32 mColorDepth = 32;

		INT32 mPriority = 0;
		UINT32 mVSyncInterval = 1;

		bool mActive = true;
		bool mHwGamma = false;
		bool mVSync = false;
		bool mRequiresTextureFlipping = false;
		bool mIsWindow = false;

		UINT32 mMultisampleCount = 0;
		String mMultisampleHint;
	};

	/**
	 * @brief	Provides access to internal render target implementation usable only from the core thread.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderTargetCore
	{
	public:
		/**
		 * @brief	Frame buffer type when double-buffering is used.
		 */
		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};

		RenderTargetCore(RenderTarget* parent, RenderTargetProperties* properties);
		virtual ~RenderTargetCore();

		/**
		 * @brief	Makes the render target active or inactive. (e.g. for a window, it will hide or restore the window).
		 *
		 * @note	Core thread only.
		 */
		virtual void setActive(bool state) { mProperties->mActive = state; markCoreDirty(); }

		/**
		 * @brief	Sets a priority that determines in which orders the render targets the processed.
		 * 			
		 * @param	priority	The priority. Higher value means the target will be rendered sooner.
		 */
		void setPriority(INT32 priority) { mProperties->mPriority = priority; markCoreDirty(); }

		/**
		 * @brief	Swaps the frame buffers to display the next frame.
		 *
		 * @note	Core thread only.
		 */
		virtual void swapBuffers() {};

		/**
		 * @brief	Queries the render target for a custom attribute. This may be anything and is
		 *			implementation specific.
		 *
		 * @note	Core thread only.
		 */
		virtual void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @brief	Returns properties that describe the render target.
		 */
		const RenderTargetProperties& getProperties() const;

		/**
		 * @brief	Returns the non core version of the render target.
		 */
		RenderTarget* getNonCore() const { return mParent; }

		/**
		 * @brief	Returns true if this object was modified and the sim thread version requires an update.
		 */
		bool _isCoreDirty() const { return mCoreDirty; }

		/**
		 * @brief	Marks the object as clean. Usually called after the sim thread version was updated.
		 */
		void _markCoreClean() { mCoreDirty = false; }

	protected:
		friend class RenderTarget;

		/**
		 * @brief	Marks this object as modified. Signals the system that the sim thread verison
		 *			of the object needs an update.
		 */
		void markCoreDirty() { mCoreDirty = true; }

		RenderTargetProperties* mProperties;
		RenderTarget* mParent;

	private:
		bool mCoreDirty = true;
	};

	/**
	 * @brief	Render target is a frame buffer or a texture that the render
	 *			system renders to.
	 *
	 * @note	Sim thread unless noted otherwise. Retrieve core implementation from getCore() 
	 *			for core thread only functionality.
	 */
    class BS_CORE_EXPORT RenderTarget : public CoreObject
    {
    public:
        virtual ~RenderTarget();

		/**
		 * @brief	Does the texture need to be vertically flipped because of different screen space coordinate systems.
		 *			(i.e. is origin top left or bottom left. Engine default is top left.)
		 */
		virtual bool requiresTextureFlipping() const = 0;

		/**
		 * @brief	Queries the render target for a custom attribute. This may be anything and is
		 *			implementation specific.
		 */
		virtual void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @brief	Returns properties that describe the render target.
		 *
		 * @note	Sim thread only.
		 */
		const RenderTargetProperties& getProperties() const;

		/**
		 * @brief	Retrieves a core implementation of a render target usable only from the
		 *			core thread.
		 */
		RenderTargetCore* getCore() const;

		/**
		 * @brief	Event that gets triggered whenever the render target is resized.
		 *
		 * @note	Sim thread only.
		 */
		mutable Event<void()> onResized;

    protected:
		friend class RenderTargetManager;

		RenderTarget();

		/**
		 * @brief	Creates a new instance of render target properties used for storing
		 *			render target data and providing easy access to it.
		 */
		virtual RenderTargetProperties* createProperties() const = 0;

		/**
		 * @brief	Creates a core implementation of a render target. This implementation
		 *			is to be used on the core thread only.
		 */
		virtual RenderTargetCore* createCore() = 0;

		/**
		 * @copydoc	CoreObject::initialize_internal
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc	CoreObject::destroy_internal
		 */
		virtual void destroy_internal();

		RenderTargetCore* mCore;
		RenderTargetProperties* mProperties;
    };
}
