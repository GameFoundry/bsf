#ifndef __OgreWin32GLSupport_H__
#define __OgreWin32GLSupport_H__

#include "CmWin32Prerequisites.h"
#include "CmGLSupport.h"
#include "CmGLRenderSystem.h"

namespace CamelotFramework
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
		 * Initialise extensions
		 */
		virtual void initialiseExtensions();
		

		bool selectPixelFormat(HDC hdc, int colourDepth, int multisample, bool hwGamma);

		virtual bool supportsPBuffers();
		virtual unsigned int getDisplayMonitorCount() const;
	private:
		// Allowed video modes
		vector<DEVMODE>::type mDevModes;
		Win32Window *mInitialWindow;
		vector<int>::type mFSAALevels;
		bool mHasPixelFormatARB;
        bool mHasMultisample;
		bool mHasHardwareGamma;

		struct DisplayMonitorInfo
		{
			HMONITOR		hMonitor;
			MONITORINFOEX	monitorInfoEx;
		};

		typedef vector<DisplayMonitorInfo>::type DisplayMonitorInfoList;
		typedef DisplayMonitorInfoList::iterator DisplayMonitorInfoIterator;

		DisplayMonitorInfoList mMonitorInfoList;

		void initialiseWGL();
		static LRESULT CALLBACK dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);
		static BOOL CALLBACK sCreateMonitorsInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, 
			LPRECT lprcMonitor, LPARAM dwData);
	};

}

#endif
