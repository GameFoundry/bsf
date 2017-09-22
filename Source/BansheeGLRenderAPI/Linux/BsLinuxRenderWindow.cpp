//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "CoreThread/BsCoreThread.h"
#include "BsGLPixelFormat.h"
#include "Unix/BsUnixPlatform.h"
#include "Linux/BsLinuxRenderWindow.h"
#include "Unix/BsUnixWindow.h"
#include "Linux/BsLinuxVideoModeInfo.h"
#include "Linux/BsLinuxGLSupport.h"
#include "Math/BsMath.h"
#include "RenderAPI/BsRenderAPI.h"
#include "BsGLRenderWindowManager.h"
#include <GL/glxew.h>
#include "Math/BsVector2I.h"

namespace bs
{
	LinuxRenderWindowProperties::LinuxRenderWindowProperties(const RENDER_WINDOW_DESC& desc)
		:RenderWindowProperties(desc)
	{ }

	LinuxRenderWindow::LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, ct::LinuxGLSupport& glSupport)
		:RenderWindow(desc, windowId), mGLSupport(glSupport), mProperties(desc)
	{ }

	void LinuxRenderWindow::getCustomAttribute(const String& name, void* data) const
	{
		if (name == "WINDOW")
		{
			::Window *x11Window = (::Window*)data;
			// TODOPORT
			//*x11Window = (UINT64)getHWnd();
			return;
		}
	}

	Vector2I LinuxRenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		// TODOPORT
	}

	Vector2I LinuxRenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		// TODOPORT
	}

	SPtr<ct::LinuxRenderWindow> LinuxRenderWindow::getCore() const
	{
		return std::static_pointer_cast<ct::LinuxRenderWindow>(mCoreSpecific);
	}

	void LinuxRenderWindow::syncProperties()
	{
		ScopedSpinLock lock(getCore()->mLock);
		mProperties = getCore()->mSyncedProperties;
	}

	namespace ct
	{
	LinuxRenderWindow::LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, LinuxGLSupport& glsupport)
			: RenderWindow(desc, windowId), mWindow(nullptr), mGLSupport(glsupport), mContext(nullptr), mProperties(desc)
			, mSyncedProperties(desc), mIsChild(false), mShowOnSwap(false)
	{ }

	LinuxRenderWindow::~LinuxRenderWindow()
	{
		LinuxRenderWindowProperties& props = mProperties;

		if (mWindow != nullptr)
		{
			mWindow->close();

			bs_delete(mWindow);
			mWindow = nullptr;
		}

		props.mActive = false;
	}

	void LinuxRenderWindow::initialize()
	{
		LinuxRenderWindowProperties& props = mProperties;

		props.mIsFullScreen = mDesc.fullscreen;
		mIsChild = false;
		mDisplayFrequency = Math::roundToInt(mDesc.videoMode.getRefreshRate());
		props.mColorDepth = 32;

		WINDOW_DESC windowDesc;
		windowDesc.fullscreen = mDesc.fullscreen;
		windowDesc.x = mDesc.left;
		windowDesc.y = mDesc.top;
		windowDesc.width = mDesc.videoMode.getWidth();
		windowDesc.height = mDesc.videoMode.getHeight();
		windowDesc.hidden = mDesc.hidden || mDesc.hideUntilSwap;
		windowDesc.title = mDesc.title;
		windowDesc.showDecorations = !mDesc.toolWindow;
		windowDesc.modal = mDesc.modal;
		windowDesc.visualInfo = mGLSupport.findBestVisual(LinuxPlatform::getXDisplay(), mDesc.depthBuffer,
				mDesc.multisampleCount, mDesc.gamma);

		NameValuePairList::const_iterator opt;
		opt = mDesc.platformSpecific.find("parentWindowHandle");
		if (opt != mDesc.platformSpecific.end())
			windowDesc.parent = (::Window)parseUINT64(opt->second);

		const LinuxVideoModeInfo& videoModeInfo = static_cast<const LinuxVideoModeInfo&>(RenderAPI::instance()
				.getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs > 0)
		{
			UINT32 actualMonitorIdx = std::min(mDesc.videoMode.getOutputIdx(), numOutputs - 1);
			const LinuxVideoOutputInfo& outputInfo = static_cast<const LinuxVideoOutputInfo&>(videoModeInfo
					.getOutputInfo(actualMonitorIdx));
			windowDesc.monitor = outputInfo.getMonitorHandle();
		}

		mIsChild = windowDesc.parent != nullptr;
		props.mIsFullScreen = mDesc.fullscreen && !mIsChild;
		props.mColorDepth = 32;
		props.mActive = true;

		if (!windowDesc.external)
		{
			mShowOnSwap = mDesc.hideUntilSwap;
			props.mHidden = mDesc.hideUntilSwap || mDesc.hidden;
		}

		mWindow = bs_new<LinuxWindow>(windowDesc);

		props.mWidth = mWindow->getWidth();
		props.mHeight = mWindow->getHeight();
		props.mTop = mWindow->getTop();
		props.mLeft = mWindow->getLeft();

		props.mHwGamma = testHwGamma;
		props.mMultisampleCount = testMultisample;

		XWindowAttributes windowAttributes;
		XGetWindowAttributes(LinuxPlatform::getXDisplay(), mWindow->_getXWindow(), &windowAttributes);

		XVisualInfo requestVI;
		requestVI.screen = DefaultScreen(LinuxPlatform::getXDisplay());
		requestVI.visualid = XVisualIDFromVisual(windowAttributes.visual);

		mContext = mGLSupport.createContext(LinuxPlatform::getXDisplay(), requestVI);

		if(mDesc.fullscreen && !mIsChild)
			setFullscreen(mDesc.videoMode);

		if(mDesc.hideUntilSwap || mDesc.hidden)
			setHidden(true);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties = props;
		}

		bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		RenderWindow::initialize();
	}

	void LinuxRenderWindow::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const LinuxVideoModeInfo& videoModeInfo = static_cast<const LinuxVideoModeInfo&>(RenderAPI::instance() .getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		LinuxRenderWindowProperties& props = mProperties;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const LinuxVideoOutputInfo& outputInfo = static_cast<const LinuxVideoOutputInfo&>(videoModeInfo.getOutputInfo (actualMonitorIdx));

		// TODO - Change screen mode



		mWindow->_setFullscreen(true);

		props.mIsFullScreen = true;

		props.mTop = monitorInfo.rcMonitor.top;
		props.mLeft = monitorInfo.rcMonitor.left;
		props.mWidth = width;
		props.mHeight = height;

		_windowMovedOrResized();
	}

	void LinuxRenderWindow::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		setFullscreen(mode.getWidth(), mode.getHeight(), mode.getRefreshRate(), mode.getOutputIdx());
	}

	void LinuxRenderWindow::setWindowed(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		LinuxRenderWindowProperties& props = mProperties;

		if (!props.mIsFullScreen)
			return;

		props.mIsFullScreen = false;
		props.mWidth = width;
		props.mHeight = height;

		// TODO - Restore old screen config

		mWindow->_setFullscreen(false);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties.mWidth = props.mWidth;
			mSyncedProperties.mHeight = props.mHeight;
		}

		bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		_windowMovedOrResized();
	}

	void LinuxRenderWindow::move(INT32 left, INT32 top)
	{
		THROW_IF_NOT_CORE_THREAD;

		LinuxRenderWindowProperties& props = mProperties;
		if (!props.mIsFullScreen)
		{
			mWindow->move(left, top);

			props.mTop = mWindow->getTop();
			props.mLeft = mWindow->getLeft();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mTop = props.mTop;
				mSyncedProperties.mLeft = props.mLeft;
			}

			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void LinuxRenderWindow::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		LinuxRenderWindowProperties& props = mProperties;
		if (!props.mIsFullScreen)
		{
			mWindow->resize(width, height);

			props.mWidth = mWindow->getWidth();
			props.mHeight = mWindow->getHeight();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mWidth = props.mWidth;
				mSyncedProperties.mHeight = props.mHeight;
			}

			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void LinuxRenderWindow::minimize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->minimize();
	}

	void LinuxRenderWindow::maximize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->maximize();
	}

	void LinuxRenderWindow::restore()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->restore();
	}

	void LinuxRenderWindow::swapBuffers(UINT32 syncMask)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mShowOnSwap)
			setHidden(false);

		glXSwapBuffers(LinuxPlatform::getXDisplay(), mWindow->_getXWindow());
	}

	void LinuxRenderWindow::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if ((dst.getRight() > getProperties().getWidth()) ||
			(dst.getBottom() > getProperties().getHeight()) ||
			(dst.getFront() != 0) || (dst.getBack() != 1))
		{
			BS_EXCEPT(InvalidParametersException, "Invalid box.");
		}

		if (buffer == FB_AUTO)
		{
			buffer = mProperties.isFullScreen() ? FB_FRONT : FB_BACK;
		}

		GLenum format = GLPixelUtil::getGLOriginFormat(dst.getFormat());
		GLenum type = GLPixelUtil::getGLOriginDataType(dst.getFormat());

		if ((format == GL_NONE) || (type == 0))
		{
			BS_EXCEPT(InvalidParametersException, "Unsupported format.");
		}

		// Must change the packing to ensure no overruns!
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glReadBuffer((buffer == FB_FRONT)? GL_FRONT : GL_BACK);
		glReadPixels((GLint)dst.getLeft(), (GLint)dst.getTop(),
				(GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
				format, type, dst.getData());

		// restore default alignment
		glPixelStorei(GL_PACK_ALIGNMENT, 4);

		//vertical flip
		{
			size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.getFormat());
			size_t height = dst.getHeight();
			UINT8* tmpData = (UINT8*)bs_alloc((UINT32)(rowSpan * height));
			UINT8* srcRow = (UINT8 *)dst.getData(), *tmpRow = tmpData + (height - 1) * rowSpan;

			while (tmpRow >= tmpData)
			{
				memcpy(tmpRow, srcRow, rowSpan);
				srcRow += rowSpan;
				tmpRow -= rowSpan;
			}
			memcpy(dst.getData(), tmpData, rowSpan * height);

			bs_free(tmpData);
		}
	}

	void LinuxRenderWindow::getCustomAttribute(const String& name, void* data) const
	{
		if(name == "GLCONTEXT")
		{
			SPtr<GLContext>* contextPtr = static_cast<SPtr<GLContext>*>(data);
			*contextPtr = mContext;
			return;
		}
		else if(name == "WINDOW")
		{
			::Window* window = (::Window*)data;
			*window = mWindow->_getXWindow();
			return;
		}
	}

	void LinuxRenderWindow::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(state)
			mWindow->restore();
		else
			mWindow->minimize();

		RenderWindow::setActive(state);
	}

	void LinuxRenderWindow::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		mShowOnSwap = false;

		if(hidden)
			mWindow->hide();
		else
			mWindow->show();

		RenderWindow::setHidden(hidden);
	}

	void LinuxRenderWindow::_windowMovedOrResized()
	{
		if (!mWindow)
			return;

		mWindow->_windowMovedOrResized();

		LinuxRenderWindowProperties& props = mProperties;
		if (!props.mIsFullScreen) // Fullscreen is handled directly by this object
		{
			props.mTop = mWindow->getTop();
			props.mLeft = mWindow->getLeft();
			props.mWidth = mWindow->getWidth();
			props.mHeight = mWindow->getHeight();
		}

		RenderWindow::_windowMovedOrResized();
	}

	void LinuxRenderWindow::syncProperties()
	{
		ScopedSpinLock lock(mLock);
		mProperties = mSyncedProperties;
	}
	}
}

