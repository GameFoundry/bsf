//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "RenderAPI/BsRenderWindow.h"

namespace bs
{
	class CocoaWindow;
	class MacOSRenderWindow;

	namespace ct
	{
		class MacOSGLSupport;
		class MacOSContext;
		class MacOSRenderWindow;
	}

	/** @addtogroup GL
	 *  @{
	 */

	/**
	 * Render window implementation for MacOS.
	 *
	 * @note	Sim thread only.
	 */
	class MacOSRenderWindow : public RenderWindow
	{
	public:
		~MacOSRenderWindow() { }

		/** @copydoc RenderWindow::getCustomAttribute */
		void getCustomAttribute(const String& name, void* pData) const override;

		/** @copydoc RenderWindow::screenToWindowPos */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const override;

		/** @copydoc RenderWindow::windowToScreenPos */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const override;

		/** @copydoc RenderWindow::getCore */
		SPtr<ct::MacOSRenderWindow> getCore() const;

	protected:
		friend class GLRenderWindowManager;
		friend class ct::MacOSGLSupport;
		friend class ct::MacOSRenderWindow;

		MacOSRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, ct::MacOSGLSupport& glSupport);

		/** @copydoc RenderWindow::getProperties */
		const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

		/** @copydoc RenderWindow::syncProperties */
		void syncProperties() override;

	private:
		RenderWindowProperties mProperties;
		const ct::MacOSGLSupport& mGLSupport;
	};

	namespace ct
	{
		/**
		 * Render window implementation for MacOS.
		 *
		 * @note	Core thread only.
		 */
		class MacOSRenderWindow : public RenderWindow
		{
		public:
			MacOSRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, MacOSGLSupport& glSupport);
			~MacOSRenderWindow();

			/** @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32) */
			void setFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

			/** @copydoc RenderWindow::setFullscreen(const VideoMode&) */
			void setFullscreen(const VideoMode& videoMode) override;

			/** @copydoc RenderWindow::setWindowed */
			void setWindowed(UINT32 width, UINT32 height) override;

			/** @copydoc RenderWindow::setHidden */
			void setHidden(bool hidden) override;

			/** @copydoc RenderWindow::minimize */
			void minimize() override;

			/** @copydoc RenderWindow::maximize */
			void maximize() override;

			/** @copydoc RenderWindow::restore */
			void restore() override;

			/** @copydoc RenderWindow::move */
			void move(INT32 left, INT32 top) override;

			/** @copydoc RenderWindow::resize */
			void resize(UINT32 width, UINT32 height) override;

			/** @copydoc RenderWindow::setVSync */
			void setVSync(bool enabled, UINT32 interval = 1) override;

			/**
			 * Copies the contents of a frame buffer into the pre-allocated buffer.
			 *
			 * @param[out]	dst		Previously allocated buffer to read the contents into. Must be of valid size.
			 * @param[in]	buffer	Frame buffer to read the contents from.
			 */
			void copyToMemory(PixelData& dst, FrameBuffer buffer);

			/** @copydoc RenderWindow::swapBuffers */
			void swapBuffers(UINT32 syncMask) override;

			/** @copydoc RenderWindow::getCustomAttribute */
			void getCustomAttribute(const String& name, void* pData) const override;

			/** @copydoc RenderWindow::setActive */
			void setActive(bool state) override;

			/** @copydoc RenderWindow::_windowMovedOrResized */
			void _windowMovedOrResized() override;

			/** Returns a lock that can be used for accessing synced properties. */
			SpinLock& _getPropertiesLock() { return mLock;}

		protected:
			friend class MacOSGLSupport;

			/** @copydoc CoreObject::initialize */
			void initialize() override;

			/** @copydoc RenderWindow::getProperties */
			const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

			/** @copydoc RenderWindow::getSyncedProperties */
			RenderWindowProperties& getSyncedProperties() override { return mSyncedProperties; }

			/** @copydoc RenderWindow::syncProperties */
			void syncProperties() override;

			/** Changes the display mode (resolution, refresh rate) of the specified output device. */
			void setDisplayMode(const VideoOutputInfo& output, const VideoMode& mode);

		protected:
			friend class bs::MacOSRenderWindow;

			CocoaWindow* mWindow;
			SPtr<MacOSContext> mContext;
			ct::MacOSGLSupport& mGLSupport;
			RenderWindowProperties mProperties;
			RenderWindowProperties mSyncedProperties;
			bool mIsChild;
			bool mShowOnSwap;

			// Coordinate conversions
			Mutex mDimensionsMutex;
			Rect2I mScreenArea;
			Rect2I mNativeWindowArea;
		};
	}

	/** @} */
}

