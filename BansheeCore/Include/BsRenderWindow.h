#pragma once

#include "BsCorePrerequisites.h"

#include "BsRenderTarget.h"
#include "BsVideoModeInfo.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Enum that defines possible window border styles.
	 */
	enum class WindowBorder
	{
		Normal,
		None,
		Fixed
	};

	/**
	 * @brief	Structure that is used for initializing a render window.
	 */
	struct BS_CORE_EXPORT RENDER_WINDOW_DESC
	{
		RENDER_WINDOW_DESC()
		: vsync(false), vsyncInterval(1), fullscreen(false), hidden(false), depthBuffer(true)
			, multisampleCount(0), multisampleHint(""), gamma(false), left(-1), top(-1)
			, title(""), border(WindowBorder::Normal), outerDimensions(false), enableDoubleClick(true)
			, toolWindow(false), modal(false)
		{ }

		VideoMode videoMode; /**< A set of frame buffer options. */
		bool fullscreen; /**< Should the window be opened in fullscreen mode. */
		bool vsync; /**< Should the window wait for vertical sync before swapping buffers. */
		UINT32 vsyncInterval; /**< Determines how many vsync intervals occur per frame. FPS = refreshRate/interval. Usually 1 when vsync active. */
		bool hidden; /**< Should the window be hidden. */
		bool depthBuffer; /**< Should the window be created with a depth/stencil buffer. */
		UINT32 multisampleCount; /**< If higher than 1, texture containing multiple samples per pixel is created. */
		String multisampleHint; /**< Hint about what kind of multisampling to use. Render system specific. */
		bool gamma; /**< Should the written color pixels be gamma corrected before write. */
		INT32 left; /**< Window origin on X axis in pixels. -1 == screen center. Relative to monitor provided in videoMode. */
		INT32 top; /**< Window origin on Y axis in pixels. -1 == screen center. Relative to monitor provided in videoMode. */
		String title; /**< Title of the window. */
		WindowBorder border; /**< Type of border to create the window with. */
		bool outerDimensions; /**< Do our dimensions include space for things like title-bar and border. */
		bool enableDoubleClick; /**< Does window accept double-clicks. */
		bool toolWindow; /**< Tool windows have a different style than normal windows and can be created with no border or title bar. */
		bool modal; /**< When a modal window is open all other windows will be locked until modal window is closed. */

		NameValuePairList platformSpecific; /**< Platform-specific creation options. */
	};

	/**
	 * @brief	Contains various properties that describe a render window.
	 */
	class BS_CORE_EXPORT RenderWindowProperties : public RenderTargetProperties
	{
	public:
		RenderWindowProperties(const RENDER_WINDOW_DESC& desc);
		virtual ~RenderWindowProperties() { }

		/**
		 * @brief	Gets the horizontal origin of the window in pixels.
		 */
		INT32 getLeft() const { return mLeft; }

		/**
		 * @brief	Gets the vertical origin of the window in pixels.
		 */
		INT32 getTop() const { return mTop; }

		/**
		 * @brief	Indicates whether the window currently has keyboard focus.
		 */
		bool hasFocus() const { return mHasFocus; }

		/**
		 * @brief	Returns true if window is running in fullscreen mode.
		 */
		bool isFullScreen() const { return mIsFullScreen; }

		/**
		 * @brief	Returns true if the window is modal (blocks interaction with
		 *			any non-modal window until closed).
		 */
		bool isModal() const { return mIsModal; }

		/**
		 * @brief	Returns true if the window is hidden.
		 */
		bool isHidden() const { return mHidden; }

		/**
		 * @brief	Returns true if the window is maximized.
		 */
		bool isMaximized() const { return mIsMaximized; }

	protected:
		friend class RenderWindowCore;
		friend class RenderWindow;

		bool mIsFullScreen = false;
		INT32 mLeft = 0;
		INT32 mTop = 0;
		bool mHasFocus = false;
		bool mHidden = false;
		bool mIsModal = false;
		bool mIsMaximized = false;
	};

	/**
	 * @brief	Provides access to internal render window implementation usable only from the core thread.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderWindowCore : public RenderTargetCore
	{
	public:
		RenderWindowCore(const RENDER_WINDOW_DESC& desc);
		virtual ~RenderWindowCore();

		/** 
		 * @brief	Switches the window to fullscreen mode. Child windows cannot go into fullscreen mode.
		 *
		 * @param	width		Width of the window frame buffer in pixels.
		 * @param	height		Height of the window frame buffer in pixels.
		 * @param	refreshRate	Refresh rate of the window in Hertz.
		 * @param	monitorIdx	Index of the monitor to go fullscreen on.
		 *
		 * @note	If the exact provided mode isn't available, closest one is used instead.
		 */
		virtual void setFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) { }

		/**
		* @brief	Switches the window to fullscreen mode. Child windows cannot go into fullscreen mode.
		*
		* @param	videoMode	Mode retrieved from VideoModeInfo in RenderSystem.
		*/
		virtual void setFullscreen(const VideoMode& mode) { }

		/**
		 * @brief	Switches the window to windowed mode.
		 *
		 * @param	Window width in pixels.
		 * @param	Window height in pixels.
		 */
		virtual void setWindowed(UINT32 width, UINT32 height) { }

        /**
         * @brief	Hide or show the window.
         */
        virtual void setHidden(bool hidden);

		/**
		 * @brief	Makes the render target active or inactive. (e.g. for a window, it will hide or restore the window).
		 */
		virtual void setActive(bool state);

		/**
		 * @brief	Minimizes the window to the taskbar.
		 */
		virtual void minimize() { }

		/**
		 * @brief	Maximizes the window over the entire current screen.
		 */
		virtual void maximize() { }

		/**
		 * @brief	Restores the window to original position and size if it is
		 *			minimized or maximized.
		 */
		virtual void restore() { }

        /**
         * @brief	Change the size of the window.
         */
        virtual void resize(UINT32 width, UINT32 height) = 0;

        /**
         * @brief	Reposition the window.
         */
        virtual void move(INT32 left, INT32 top) = 0;

		/**
		 * @brief	Returns properties that describe the render window.
		 */
		const RenderWindowProperties& getProperties() const;

		/**
		 * @brief	Called when window is moved or resized.
		 *
		 * @note	Core thread. Internal method.
		 */
		virtual void _windowMovedOrResized();

		/**
		 * @brief	Called when window has received focus.
		 *
		 * @note	Core thread.
		 */
		virtual void _windowFocusReceived();

		/**
		 * @brief	Called when window has lost focus.
		 *
		 * @note	Core thread.
		 */
		virtual void _windowFocusLost();

		/**
		 * @brief	Called when window has been maximized.
		 *
		 * @note	Core thread.
		 */
		virtual void _notifyMaximized();

		/**
		 * @brief	Called when window has been minimized.
		 *
		 * @note	Core thread.
		 */
		virtual void _notifyMinimized();

		/**
		 * @brief	Called when window has been restored 
		 *			from minimized or maximized state.
		 *
		 * @note	Core thread.
		 */
		virtual void _notifyRestored();

	protected:
		friend class RenderWindow;
		friend class RenderWindowManager;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual void syncToCore(const CoreSyncData& data);

		/**
		 * @brief	Retrieves data that is to be used for syncing between core and sim thread
		 *			versions of this object.
		 */
		virtual UINT32 getSyncData(UINT8* buffer) { return 0; }

		RENDER_WINDOW_DESC mDesc;
	};

	/**
	 * @brief	Render target specialization that allows you to render into window
	 *			frame buffer(s).
	 *
	 * @note	Sim thread only. Retrieve core implementation from getCore()
	 *			for core thread only functionality.
	 */
    class BS_CORE_EXPORT RenderWindow : public RenderTarget
    {
    public:
		virtual ~RenderWindow() { }

		/**
		 * @copydoc	RenderTarget::destroy
		 */
		virtual void destroy();	

		/**
		 * @brief	Converts screen position into window local position.
		 */
		virtual Vector2I screenToWindowPos(const Vector2I& screenPos) const = 0;

		/**
		 * @brief	Converts window local position to screen position.
		 */
		virtual Vector2I windowToScreenPos(const Vector2I& windowPos) const = 0;

		/**
		 * @brief	Resize the window to specified width and height in pixels.
		 */
		void resize(CoreAccessor& accessor, UINT32 width, UINT32 height);

		/**
		 * @brief	Move the window to specified screen coordinates.
		 */
		void move(CoreAccessor& accessor, INT32 left, INT32 top);

		/**
		 * @brief	Hide the window. (Does not destroy it, just hides it).
		 */
		void hide(CoreAccessor& accessor);

		/**
		 * @brief	Shows a previously hidden window.
		 */
		void show(CoreAccessor& accessor);

		/**
		 * @copydoc	RenderWindowCore::minimize
		 */
		void minimize(CoreAccessor& accessor);

		/**
		 * @copydoc	RenderWindowCore::maximize
		 */
		void maximize(CoreAccessor& accessor);

		/**
		 * @copydoc	RenderWindowCore::restore
		 */
		void restore(CoreAccessor& accessor);

		/**
		 * @copydoc RenderWindowCore::setFullscreen(UINT32, UINT32, float, UINT32)
		 */
		void setFullscreen(CoreAccessor& accessor, UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0);

		/**
		 * @copydoc RenderWindowCore::setFullscreen(const VideoMode&)
		 */
		void setFullscreen(CoreAccessor& accessor, const VideoMode& mode);

		/**
		 * @copydoc RenderWindowCore::setWindowed
		 */
		void setWindowed(CoreAccessor& accessor, UINT32 width, UINT32 height);

		/**
		 * @brief	Retrieves a core implementation of a render window usable only from the
		 *			core thread.
		 */
		SPtr<RenderWindowCore> getCore() const;

		/**
		 * @brief	Returns properties that describe the render window.
		 */
		const RenderWindowProperties& getProperties() const;

		/**
		 * @brief	Creates a new render window using the specified options. Optionally
		 *			makes the created window a child of another window.
		 */
		static RenderWindowPtr create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow = nullptr);

    protected:
		friend class RenderWindowManager;

		RenderWindow(const RENDER_WINDOW_DESC& desc);

		/**
		 * @brief	Returns render window properties that may be edited.
		 */
		RenderWindowProperties& getMutableProperties();

		/**
		 * @brief	Updates internal properties using the provided data. Data must have been retrieved from
		 *			"getSyncData" method of the core version of this object.
		 */
		virtual void setSyncData(UINT8* buffer, UINT32 size) { }

		/**
		 * @copydoc	RenderTarget::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual CoreSyncData syncToCore(FrameAlloc* allocator);

	protected:
		RENDER_WINDOW_DESC mDesc;
    };
}
