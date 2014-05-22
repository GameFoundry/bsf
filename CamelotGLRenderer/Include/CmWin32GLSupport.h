#pragma once

#include "CmWin32Prerequisites.h"
#include "CmGLSupport.h"
#include "CmGLRenderSystem.h"

namespace BansheeEngine
{
	class CM_RSGL_EXPORT Win32GLSupport : public GLSupport
	{
	public:
        Win32GLSupport();

		/// @copydoc RenderSystem::_createRenderWindow
		virtual RenderWindowPtr newWindow(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

		/**
		* Start anything special
		*/
		void start();

		/**
		* Stop anything special
		*/
		void stop();

		/**
		* Get the address of a function
		*/
		void* getProcAddress(const String& procname);

		/**
		 * Initialize extensions
		 */
		virtual void initializeExtensions();
		
		Win32Context* createContext(HDC hdc, HGLRC externalGlrc = 0);

		bool selectPixelFormat(HDC hdc, int colourDepth, int multisample, bool hwGamma);

		/**
		 * @copydoc	GLSupport::getVideoModeInfo
		 */
		VideoModeInfoPtr getVideoModeInfo() const;

	private:
		// Allowed video modes
		Vector<DEVMODE> mDevModes;
		Win32Window *mInitialWindow;
		Vector<int> mFSAALevels;
		bool mHasPixelFormatARB;
        bool mHasMultisample;
		bool mHasHardwareGamma;
		bool mHasAdvancedContext;

		void initialiseWGL();
		static LRESULT CALLBACK dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);
	};
}