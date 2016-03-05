//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsWin32Prerequisites.h"
#include "BsGLSupport.h"
#include "BsGLRenderAPI.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles OpenGL initialization, window creation and extensions
	 *			on Windows.
	 */
	class BS_RSGL_EXPORT Win32GLSupport : public GLSupport
	{
	public:
        Win32GLSupport();

		/**
		 * @copydoc	GLSupport::newWindow
		 */
		virtual RenderWindowPtr newWindow(RENDER_WINDOW_DESC& desc, UINT32 windowId, RenderWindowPtr parentWindow) override;

		/**
		 * @copydoc	GLSupport::newWindowCore
		 */
		virtual SPtr<RenderWindowCore> newWindowCore(RENDER_WINDOW_DESC& desc, UINT32 windowId) override;

		/**
		 * @copydoc	GLSupport::start
		 */
		void start() override;

		/**
		 * @copydoc	GLSupport::stop
		 */
		void stop() override;

		/**
		 * @copydoc	GLSupport::getProcAddress
		 */
		void* getProcAddress(const String& procname) override;

		/**
		 * @copydoc	GLSupport::initializeExtensions
		 */
		virtual void initializeExtensions() override;
		
		/**
		 * @brief	Creates a new OpenGL context.
		 *
		 * @param	hdc				Handle to device context to create the context from.
		 * @param	externalGlrc	(Optional) Handle to external OpenGL context. If not provided
		 *							new context will be created.
		 *
		 * @returns	Newly created GLContext class referencing the created or external context handle.
		 */
		SPtr<Win32Context> createContext(HDC hdc, HGLRC externalGlrc = 0);

		/**
		 * @brief	Selects and sets an appropriate pixel format based on the provided parameters.
		 *
		 * @param	hdc			Handle to device context to create the context from.
		 * @param	colorDepth	Wanted color depth of the pixel format, in bits.
		 * @param	multisample	Amount of multisampling wanted, if any.
		 * @param	hwGamma		Should the format support automatic gamma conversion on write/read.
		 * @param	depth		Should the pixel format contain the depth/stencil buffer.
		 *
		 * @returns	True if a pixel format was successfully set.
		 */
		bool selectPixelFormat(HDC hdc, int colorDepth, int multisample, bool hwGamma, bool depthStencil);

		/**
		 * @copydoc	GLSupport::getVideoModeInfo
		 */
		VideoModeInfoPtr getVideoModeInfo() const override;

	private:
		/**
		 * @brief	Initializes windows specific OpenGL extensions needed for advanced context creation.
		 */
		void initialiseWGL();

		/**
		 * @brief	Dummy window procedure used when creating the initial dummy OpenGL context.
		 */
		static LRESULT CALLBACK dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);

		Vector<DEVMODE> mDevModes;
		Win32RenderWindowCore *mInitialWindow;
		Vector<int> mMultisampleLevels;
		bool mHasPixelFormatARB;
        bool mHasMultisample;
		bool mHasHardwareGamma;
		bool mHasAdvancedContext;
	};
}