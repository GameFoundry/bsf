#include "CmWin32GLSupport.h"
#include "CmGLTexture.h"
#include "CmWin32Window.h"
#include "CmGLRenderSystem.h"
#include "CmWin32Context.h"
#include "CmException.h"
#include "GL/wglew.h"
#include <algorithm>

GLenum __stdcall wglewContextInit (BansheeEngine::GLSupport *glSupport);

namespace BansheeEngine 
{
	template<class C> void remove_duplicates(C& c)
	{
		std::sort(c.begin(), c.end());
		typename C::iterator p = std::unique(c.begin(), c.end());
		c.erase(p, c.end());
	}

	Win32GLSupport::Win32GLSupport()
        : mInitialWindow(0), mHasPixelFormatARB(false), mHasMultisample(false), 
		mHasHardwareGamma(false), mHasAdvancedContext(false)
    {
		initialiseWGL();
    } 

	RenderWindowPtr Win32GLSupport::newWindow(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{		
		if(parentWindow != nullptr)
		{
			HWND hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString((UINT64)hWnd);
		}

		Win32Window* window = new (cm_alloc<Win32Window, PoolAlloc>()) Win32Window(desc, *this);
		
		if(!mInitialWindow)
			mInitialWindow = window;

		return RenderWindowPtr(window, &CoreObject::_deleteDelayed<Win32Window, PoolAlloc>);
	}

	void Win32GLSupport::start()
	{
	}

	void Win32GLSupport::stop()
	{
		mInitialWindow = nullptr;
	}

	void Win32GLSupport::initializeExtensions()
	{
		assert(mInitialWindow != nullptr);
		
		GLSupport::initializeExtensions();

		wglewContextInit(this);

		// Check for W32 specific extensions probe function
		PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsString = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsString");
		if(_wglGetExtensionsString == nullptr)
			return;

		const char *wgl_extensions = _wglGetExtensionsString(mInitialWindow->_getHDC());

		// Parse them, and add them to the main list
		StringStream ext;
        String instr;
		ext << wgl_extensions;
        while(ext >> instr)
        {
            extensionList.insert(instr);
        }
	}

	Win32Context* Win32GLSupport::createContext(HDC hdc, HGLRC externalGlrc)
	{
		GLRenderSystem* rs = static_cast<GLRenderSystem*>(RenderSystem::instancePtr());

		// If RenderSystem has initialized a context use that, otherwise we create our own
		HGLRC glrc = externalGlrc;
		bool createdNew = false;
		if (!rs->_isContextInitialized())
		{
			if (externalGlrc == 0)
			{
				if (mHasAdvancedContext)
				{
					int attribs[40];
					int i = 0;

					attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
					attribs[i++] = 4;
					attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
					attribs[i++] = 3;
					attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
					attribs[i++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

#if CM_DEBUG_MODE
					attribs[i++] = WGL_CONTEXT_FLAGS_ARB;
					attribs[i++] = WGL_CONTEXT_DEBUG_BIT_ARB;				
#endif

					attribs[i++] = 0;
					glrc = wglCreateContextAttribsARB(hdc, 0, attribs);
				}
				else
					glrc = wglCreateContext(hdc);

				if (glrc == 0)
					CM_EXCEPT(RenderingAPIException, "wglCreateContext failed: " + translateWGLError());

				createdNew = true;
			}
		}
		else
		{
			rs->getMainContext()->setCurrent();
			glrc = wglGetCurrentContext();
		}

		return cm_new<Win32Context>(hdc, glrc, createdNew);
	}

	void* Win32GLSupport::getProcAddress(const String& procname)
	{
        return (void*)wglGetProcAddress(procname.c_str());
	}

	void Win32GLSupport::initialiseWGL()
	{
		// We need to create a dummy context in order to get functions
		// that allow us to create a more advanced context. It seems
		// hacky but that's the only way to do it.
		
		LPCSTR dummyText = "Dummy";
#ifdef CM_STATIC_LIB
		HINSTANCE hinst = GetModuleHandle(NULL);
#else
		HINSTANCE hinst = GetModuleHandle(MODULE_NAME.c_str());
#endif
		
		WNDCLASS dummyClass;
		memset(&dummyClass, 0, sizeof(WNDCLASS));
		dummyClass.style = CS_OWNDC;
		dummyClass.hInstance = hinst;
		dummyClass.lpfnWndProc = dummyWndProc;
		dummyClass.lpszClassName = dummyText;
		RegisterClass(&dummyClass);

		HWND hwnd = CreateWindow(dummyText, dummyText, WS_POPUP | WS_CLIPCHILDREN,
			0, 0, 32, 32, 0, 0, hinst, 0);

		if (hwnd == nullptr)
			CM_EXCEPT(RenderingAPIException, "CreateWindow() failed");

		HDC hdc = GetDC(hwnd); 

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.cColorBits = 16;
		pfd.cDepthBits = 15;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		
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
				(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
			
			PFNWGLCREATECONTEXTATTRIBSARBPROC _wglCreateContextAttribsARB = 
				(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

			if (_wglCreateContextAttribsARB != nullptr)
			{
				WGLEW_GET_FUN(__wglewCreateContextAttribsARB) = _wglCreateContextAttribsARB;
				mHasAdvancedContext = true;
			}

			// Check for pixel format and multisampling support
			if (_wglGetExtensionsStringARB != nullptr)
			{
				std::istringstream wglexts(_wglGetExtensionsStringARB(hdc));
				String ext;
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
				// Enumerate all formats w/ multisampling
				static const int iattr[] = {
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLES_ARB, 2,
					0
				};

				int formats[256];
				unsigned int count;
                WGLEW_GET_FUN(__wglewChoosePixelFormatARB) = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
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
			Vector<int> attribList;
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
			if (!WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, &(attribList[0]), NULL, 1, &format, &nformats) || nformats <= 0)
				return false;
		}
		else
		{
			format = ChoosePixelFormat(hdc, &pfd);
		}

		return (format && SetPixelFormat(hdc, format, &pfd));
	}

	String translateWGLError()
	{
		int winError = GetLastError();
		char errDesc[255];
		int i;

		// Try windows errors first
		i = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, winError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) errDesc, 255, NULL);

		return String(errDesc);
	}
}
