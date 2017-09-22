//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "RenderAPI/BsRenderWindow.h"

namespace bs
{
	class LinuxWindow;
	class LinuxRenderWindow;

	namespace ct
	{
		class LinuxGLSupport;
		class LinuxContext;
		class LinuxRenderWindow;
	}

	/** @addtogroup GL
	 *  @{
	 */

	/**	Contains various properties that describe a render window. */
	class LinuxRenderWindowProperties : public RenderWindowProperties
	{
	public:
		LinuxRenderWindowProperties(const RENDER_WINDOW_DESC& desc);
		virtual ~LinuxRenderWindowProperties() { }

	private:
		friend class ct::LinuxRenderWindow;
		friend class LinuxRenderWindow;
	};

	/**
	 * Render window implementation for Linux.
	 *
	 * @note	Sim thread only.
	 */
	class LinuxRenderWindow : public RenderWindow
	{
	public:
		~LinuxRenderWindow() { }

		/** @copydoc RenderWindow::getCustomAttribute */
		void getCustomAttribute(const String& name, void* pData) const override;

		/** @copydoc RenderWindow::screenToWindowPos */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const override;

		/** @copydoc RenderWindow::windowToScreenPos */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const override;

		/** @copydoc RenderWindow::getCore */
		SPtr<ct::LinuxRenderWindow> getCore() const;

	protected:
		friend class GLRenderWindowManager;
		friend class ct::LinuxGLSupport;
		friend class ct::LinuxRenderWindow;

		LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, ct::LinuxGLSupport& glSupport);

		/** @copydoc RenderWindow::getProperties */
		const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

		/** @copydoc RenderWindow::syncProperties */
		void syncProperties() override;

	private:
		ct::LinuxGLSupport& mGLSupport;
		LinuxRenderWindowProperties mProperties;
	};

	namespace ct
	{
		/**
		 * Render window implementation for Linux.
		 *
		 * @note	Core thread only.
		 */
		class LinuxRenderWindow : public RenderWindow
		{
		public:
			LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, LinuxGLSupport &glSupport);
			~LinuxRenderWindow();

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

		protected:
			friend class LinuxGLSupport;

			/** @copydoc CoreObject::initialize */
			void initialize() override;

			/** @copydoc RenderWindow::getProperties */
			const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

			/** @copydoc RenderWindow::getSyncedProperties */
			RenderWindowProperties& getSyncedProperties() override { return mSyncedProperties; }

			/** @copydoc RenderWindow::syncProperties */
			void syncProperties() override;

		protected:
			friend class bs::LinuxRenderWindow;

			LinuxWindow* mWindow;
			LinuxGLSupport& mGLSupport;
			SPtr<LinuxContext> mContext;
			LinuxRenderWindowProperties mProperties;
			LinuxRenderWindowProperties mSyncedProperties;
			bool mIsChild;
			bool mShowOnSwap;
		};
	}

	/** @} */
}

