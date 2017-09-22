//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Linux/BsLinuxContext.h"
#include "Unix/BsUnixPlatform.h"
#include "Linux/BsLinuxGLSupport.h"

namespace bs { namespace ct
{
		typedef int(*ErrorHandlerProc)(::Display*, XErrorEvent*);
		int contextErrorHandler(::Display* display, XErrorEvent* error)
		{
			// Do nothing
		}

		LinuxContext::LinuxContext(::Display* display, XVisualInfo& visualInfo)
		: mDisplay(display), mContext(None)
	{
		INT32 dummy;
		XVisualInfo* windowVisualInfo = XGetVisualInfo(display, VisualIDMask | VisualScreenMask, &visualInfo, &dummy);

		INT32 majorVersion, minorVersion;
		glXQueryVersion(display, &majorVersion, &minorVersion);

		GLXContext context = 0;

		// createContextAttrib was added in GLX version 1.3
		bool hasCreateContextAttrib = extGLX_ARB_create_context && (majorVersion > 1 || minorVersion >= 3);
		if(hasCreateContextAttrib)
		{
			// Find the config used to create the window's visual
			GLXFBConfig* windowConfig = nullptr;

			INT32 numConfigs;
			GLXFBConfig* configs = glXChooseFBConfig(display, DefaultScreen(display), nullptr, &numConfigs);

			for (INT32 i = 0; i < numConfigs; ++i)
			{
				XVisualInfo* configVisualInfo = glXGetVisualFromFBConfig(display, configs[i]);

				if(!configVisualInfo)
					continue;

				if(windowVisualInfo->visualid == configVisualInfo->visualid)
				{
					windowConfig = &configs[i];
					break;
				}
			}

			if(windowConfig)
			{
				int32_t attributes[] =
						{
								GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
								GLX_CONTEXT_MINOR_VERSION_ARB, 5,
								0, 0, // Core profile
								0, 0, // Debug flags
								0 // Terminator
						};

				if(extGLX_ARB_create_context_profile)
				{
					attributes[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
					attributes[5] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
				}

#if BS_DEBUG_MODE
					attributes[6] = GLX_CONTEXT_FLAGS_ARB;
					attributes[7] = GLX_CONTEXT_DEBUG_BIT_ARB;
#endif

				// Add error handler so the application doesn't crash on error
				ErrorHandlerProc oldErrorHandler = XSetErrorHandler(&contextErrorHandler);
				context = glXCreateContextAttribsARB(display, *windowConfig, 0, True, attributes);
				XSetErrorHandler(oldErrorHandler);
			}

			XFree(configs);
		}

		// If createContextAttribs failed or isn't supported, fall back to glXCreateContext
		if(!context)
			context = glXCreateContext(display, windowVisualInfo, 0, True);

		XFree(windowVisualInfo);

		mContext = context;
	}

	LinuxContext::~LinuxContext()
	{
		releaseContext();
	}

	void LinuxContext::setCurrent()
	{
		glXMakeCurrent(mDisplay, LinuxPlatform::getMainXWindow(), mContext);
	}

	void LinuxContext::endCurrent()
	{
		glXMakeCurrent(mDisplay, None, None);
	}

	void LinuxContext::releaseContext()
	{
		if (mContext)
		{
			glXDestroyContext(mDisplay, mContext);
			mContext = None;
		}
	}
}}