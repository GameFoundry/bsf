#include "CmException.h"

#include <algorithm>

#include "CmWin32GLSupport.h"
#include "CmGLTexture.h"
#include "CmWin32Window.h"
#include <GL/wglext.h>

using namespace CamelotFramework;

#if CM_THREAD_SUPPORT != 1
GLenum wglewContextInit (CamelotFramework::GLSupport *glSupport);
#endif

namespace CamelotFramework 
{
	Win32GLSupport::Win32GLSupport()
        : mInitialWindow(0)
        , mHasPixelFormatARB(false)
        , mHasMultisample(false)
		, mHasHardwareGamma(false)
    {
		// immediately test WGL_ARB_pixel_format and FSAA support
		// so we can set configuration options appropriately
		initialiseWGL();
    } 

	template<class C> void remove_duplicates(C& c)
	{
		std::sort(c.begin(), c.end());
		typename C::iterator p = std::unique(c.begin(), c.end());
		c.erase(p, c.end());
	}

	BOOL CALLBACK Win32GLSupport::sCreateMonitorsInfoEnumProc(
		HMONITOR hMonitor,  // handle to display monitor
		HDC hdcMonitor,     // handle to monitor DC
		LPRECT lprcMonitor, // monitor intersection rectangle
		LPARAM dwData       // data
		)
	{
		DisplayMonitorInfoList* pArrMonitorsInfo = (DisplayMonitorInfoList*)dwData;

		// Get monitor info
		DisplayMonitorInfo displayMonitorInfo;

		displayMonitorInfo.hMonitor = hMonitor;

		memset(&displayMonitorInfo.monitorInfoEx, 0, sizeof(MONITORINFOEX));
		displayMonitorInfo.monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &displayMonitorInfo.monitorInfoEx);

		pArrMonitorsInfo->push_back(displayMonitorInfo);

		return TRUE;
	}

	RenderWindowPtr Win32GLSupport::newWindow(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{		
		if(parentWindow != nullptr)
		{
			HWND hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString((unsigned long)hWnd);
		}

		Win32Window* window = new (cm_alloc<Win32Window, PoolAlloc>()) Win32Window(desc, *this);
		
		// TODO - Looking for monitors is disabled for now, as it should be done on the render thread and I need to port it but 
		//  I don't feel like it at the moment. Plus I'll probably implemented a more streamlined approach to this anyway.

		//HMONITOR hMonitor = NULL;
		//int monitorIndex = desc.monitorIndex;
		//
		//// If monitor index found, try to assign the monitor handle based on it.
		//if(monitorIndex >= 0)
		//{
		//	if (mMonitorInfoList.empty())		
		//		EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);			

		//	if (monitorIndex < (int)mMonitorInfoList.size())					
		//		hMonitor = mMonitorInfoList[monitorIndex].hMonitor;	
		//}

		//// If we didn't specified the monitor index, or if it didn't find it
		//if (hMonitor == NULL)
		//{
		//	POINT windowAnchorPoint;
		//
		//	// Fill in anchor point.
		//	windowAnchorPoint.x = desc.left;
		//	windowAnchorPoint.y = desc.top;


		//	// Get the nearest monitor to this window.
		//	hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTONEAREST);				
		//}

		//RENDER_WINDOW_DESC newDesc = desc;
		//newDesc.platformSpecific["monitorHandle"] = toString((size_t)hMonitor);

		//window->initialize(newDesc);

		if(!mInitialWindow)
			mInitialWindow = window;

		return RenderWindowPtr(window, &CoreObject::_deleteDelayed<Win32Window, PoolAlloc>);
	}

	void Win32GLSupport::start()
	{
	}

	void Win32GLSupport::stop()
	{
		mInitialWindow = 0; // Since there is no removeWindow, although there should be...
	}

	void Win32GLSupport::initialiseExtensions()
	{
		assert(mInitialWindow);
		// First, initialise the normal extensions
		GLSupport::initialiseExtensions();
		// wglew init
#if CM_THREAD_SUPPORT != 1
		wglewContextInit(this);
#endif

		// Check for W32 specific extensions probe function
		PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsString = 
			(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsString");
		if(!_wglGetExtensionsString)
			return;
		const char *wgl_extensions = _wglGetExtensionsString(mInitialWindow->getHDC());

		// Parse them, and add them to the main list
		StringStream ext;
        String instr;
		ext << wgl_extensions;
        while(ext >> instr)
        {
            extensionList.insert(instr);
        }
	}


	void* Win32GLSupport::getProcAddress(const String& procname)
	{
        	return (void*)wglGetProcAddress( procname.c_str() );
	}
	void Win32GLSupport::initialiseWGL()
	{
		// wglGetProcAddress does not work without an active OpenGL context,
		// but we need wglChoosePixelFormatARB's address before we can
		// create our main window.  Thank you very much, Microsoft!
		//
		// The solution is to create a dummy OpenGL window first, and then
		// test for WGL_ARB_pixel_format support.  If it is not supported,
		// we make sure to never call the ARB pixel format functions.
		//
		// If is is supported, we call the pixel format functions at least once
		// to initialise them (pointers are stored by glprocs.h).  We can also
		// take this opportunity to enumerate the valid FSAA modes.
		
		LPCSTR dummyText = "OgreWglDummy";
#ifdef CM_STATIC_LIB
		HINSTANCE hinst = GetModuleHandle( NULL );
#else
#  if CM_DEBUG_MODE == 1
		HINSTANCE hinst = GetModuleHandle("RenderSystem_GL_d.dll");
#  else
		HINSTANCE hinst = GetModuleHandle("RenderSystem_GL.dll");
#  endif
#endif
		
		WNDCLASS dummyClass;
		memset(&dummyClass, 0, sizeof(WNDCLASS));
		dummyClass.style = CS_OWNDC;
		dummyClass.hInstance = hinst;
		dummyClass.lpfnWndProc = dummyWndProc;
		dummyClass.lpszClassName = dummyText;
		RegisterClass(&dummyClass);

		HWND hwnd = CreateWindow(dummyText, dummyText,
			WS_POPUP | WS_CLIPCHILDREN,
			0, 0, 32, 32, 0, 0, hinst, 0);

		// if a simple CreateWindow fails, then boy are we in trouble...
		if (hwnd == NULL)
			CM_EXCEPT(RenderingAPIException, "CreateWindow() failed");


		// no chance of failure and no need to release thanks to CS_OWNDC
		HDC hdc = GetDC(hwnd); 

		// assign a simple OpenGL pixel format that everyone supports
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.cColorBits = 16;
		pfd.cDepthBits = 15;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		
		// if these fail, wglCreateContext will also quietly fail
		int format;
		if ((format = ChoosePixelFormat(hdc, &pfd)) != 0)
			SetPixelFormat(hdc, format, &pfd);

		HGLRC hrc = wglCreateContext(hdc);
		if (hrc)
		{
			HGLRC oldrc = wglGetCurrentContext();
			HDC oldhdc = wglGetCurrentDC();
			// if wglMakeCurrent fails, wglGetProcAddress will return null
			wglMakeCurrent(hdc, hrc);
			
			PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB =
				(PFNWGLGETEXTENSIONSSTRINGARBPROC)
				wglGetProcAddress("wglGetExtensionsStringARB");
			
			// check for pixel format and multisampling support
			if (_wglGetExtensionsStringARB)
			{
				std::istringstream wglexts(_wglGetExtensionsStringARB(hdc));
				std::string ext;
				while (wglexts >> ext)
				{
					if (ext == "WGL_ARB_pixel_format")
						mHasPixelFormatARB = true;
					else if (ext == "WGL_ARB_multisample")
						mHasMultisample = true;
					else if (ext == "WGL_EXT_framebuffer_sRGB")
						mHasHardwareGamma = true;
				}
			}

			if (mHasPixelFormatARB && mHasMultisample)
			{
				// enumerate all formats w/ multisampling
				static const int iattr[] = {
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    /* We are no matter about the colour, depth and stencil buffers here
					WGL_COLOR_BITS_ARB, 24,
					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
                    */
					WGL_SAMPLES_ARB, 2,
					0
				};
				int formats[256];
				unsigned int count;
                // cheating here.  wglChoosePixelFormatARB procc address needed later on
                // when a valid GL context does not exist and glew is not initialized yet.
                WGLEW_GET_FUN(__wglewChoosePixelFormatARB) =
                    (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
                if (WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, iattr, 0, 256, formats, &count))
                {
                    // determine what multisampling levels are offered
                    int query = WGL_SAMPLES_ARB, samples;
                    for (unsigned int i = 0; i < count; ++i)
                    {
                        PFNWGLGETPIXELFORMATATTRIBIVARBPROC _wglGetPixelFormatAttribivARB =
                            (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
                            wglGetProcAddress("wglGetPixelFormatAttribivARB");
                        if (_wglGetPixelFormatAttribivARB(hdc, formats[i], 0, 1, &query, &samples))
                        {
                            mFSAALevels.push_back(samples);
                        }
                    }
                    remove_duplicates(mFSAALevels);
                }
			}
			
			wglMakeCurrent(oldhdc, oldrc);
			wglDeleteContext(hrc);
		}

		// clean up our dummy window and class
		DestroyWindow(hwnd);
		UnregisterClass(dummyText, hinst);
	}

	LRESULT Win32GLSupport::dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
	{
		return DefWindowProc(hwnd, umsg, wp, lp);
	}

	bool Win32GLSupport::selectPixelFormat(HDC hdc, int colourDepth, int multisample, bool hwGamma)
	{
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = (colourDepth > 16)? 24 : colourDepth;
		pfd.cAlphaBits = (colourDepth > 16)? 8 : 0;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

		int format = 0;

		int useHwGamma = hwGamma? GL_TRUE : GL_FALSE;

		if (multisample && (!mHasMultisample || !mHasPixelFormatARB))
			return false;

		if (hwGamma && !mHasHardwareGamma)
			return false;
		
		if ((multisample || hwGamma) && WGLEW_GET_FUN(__wglewChoosePixelFormatARB))
		{

			// Use WGL to test extended caps (multisample, sRGB)
			vector<int>::type attribList;
			attribList.push_back(WGL_DRAW_TO_WINDOW_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_SUPPORT_OPENGL_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_DOUBLE_BUFFER_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_SAMPLE_BUFFERS_ARB); attribList.push_back(GL_TRUE);
			attribList.push_back(WGL_ACCELERATION_ARB); attribList.push_back(WGL_FULL_ACCELERATION_ARB);
			attribList.push_back(WGL_COLOR_BITS_ARB); attribList.push_back(pfd.cColorBits);
			attribList.push_back(WGL_ALPHA_BITS_ARB); attribList.push_back(pfd.cAlphaBits);
			attribList.push_back(WGL_DEPTH_BITS_ARB); attribList.push_back(24);
			attribList.push_back(WGL_STENCIL_BITS_ARB); attribList.push_back(8);
			attribList.push_back(WGL_SAMPLES_ARB); attribList.push_back(multisample);
			if (useHwGamma && checkExtension("WGL_EXT_framebuffer_sRGB"))
			{
				attribList.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT); attribList.push_back(GL_TRUE);
			}
			// terminator
			attribList.push_back(0);


			UINT nformats;
			// ChoosePixelFormatARB proc address was obtained when setting up a dummy GL context in initialiseWGL()
			// since glew hasn't been initialized yet, we have to cheat and use the previously obtained address
			if (!WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, &(attribList[0]), NULL, 1, &format, &nformats) || nformats <= 0)
				return false;
		}
		else
		{
			format = ChoosePixelFormat(hdc, &pfd);
		}


		return (format && SetPixelFormat(hdc, format, &pfd));
	}

	bool Win32GLSupport::supportsPBuffers()
	{
		return WGLEW_GET_FUN(__WGLEW_ARB_pbuffer) != GL_FALSE;
	}

	unsigned int Win32GLSupport::getDisplayMonitorCount() const
	{
		if (mMonitorInfoList.empty())		
			EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);

		return (unsigned int)mMonitorInfoList.size();
	}

	String translateWGLError()
	{
		int winError = GetLastError();
		char errDesc[255];
		int i;

		// Try windows errors first
		i = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			winError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) errDesc,
			255,
			NULL
			);

		return String(errDesc);
	}

}
