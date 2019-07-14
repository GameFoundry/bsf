//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Win32/BsWin32RenderWindow.h"
#include "Private/Win32/BsWin32Platform.h"
#include "Private/Win32/BsWin32Window.h"
#include "Win32/BsWin32VideoModeInfo.h"
#include "Corethread/BsCoreThread.h"
#include "Profiling/BsRenderStats.h"
#include "Managers/BsRenderWindowManager.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"
#include "BsVulkanSwapChain.h"
#include "BsVulkanDevice.h"
#include "BsVulkanCommandBuffer.h"
#include "Managers/BsVulkanCommandBufferManager.h"
#include "BsVulkanQueue.h"
#include "Math/BsMath.h"

namespace bs
{
	Win32RenderWindow::Win32RenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId)
		: RenderWindow(desc, windowId), mProperties(desc)
	{ }

	void Win32RenderWindow::getCustomAttribute(const String& name, void* pData) const
	{
		if (name == "WINDOW")
		{
			UINT64 *pHwnd = (UINT64*)pData;
			*pHwnd = (UINT64)getHWnd();
			return;
		}
	}

	Vector2I Win32RenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I Win32RenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	SPtr<ct::Win32RenderWindow> Win32RenderWindow::getCore() const
	{
		return std::static_pointer_cast<ct::Win32RenderWindow>(mCoreSpecific);
	}

	HWND Win32RenderWindow::getHWnd() const
	{
		blockUntilCoreInitialized();
		return getCore()->_getWindowHandle();
	}

	void Win32RenderWindow::syncProperties()
	{
		ScopedSpinLock lock(getCore()->mLock);
		mProperties = getCore()->mSyncedProperties;
	}

	SPtr<ct::CoreObject> Win32RenderWindow::createCore() const
	{
		ct::VulkanRenderAPI& rapi = static_cast<ct::VulkanRenderAPI&>(ct::RenderAPI::instance());

		RENDER_WINDOW_DESC desc = mDesc;
		SPtr<ct::CoreObject> coreObj = bs_shared_ptr_new<ct::Win32RenderWindow>(desc, mWindowId, rapi);
		coreObj->_setThisPtr(coreObj);

		return coreObj;
	}

	namespace ct
	{
		Win32RenderWindow::Win32RenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, VulkanRenderAPI& renderAPI)
		: RenderWindow(desc, windowId), mProperties(desc), mSyncedProperties(desc), mWindow(nullptr), mIsChild(false)
		, mShowOnSwap(false), mDisplayFrequency(0), mRenderAPI(renderAPI), mRequiresNewBackBuffer(true)
	{ }

		Win32RenderWindow::~Win32RenderWindow()
	{
		SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
		presentDevice->waitIdle();

		if (mWindow != nullptr)
		{
			bs_delete(mWindow);
			mWindow = nullptr;
		}

		mSwapChain->destroy();
		vkDestroySurfaceKHR(mRenderAPI._getInstance(), mSurface, gVulkanAllocator);

		Platform::resetNonClientAreas(*this);
	}

	void Win32RenderWindow::initialize()
	{
		RenderWindowProperties& props = mProperties;

		// Create a window
		WINDOW_DESC windowDesc;
		windowDesc.showTitleBar = mDesc.showTitleBar;
		windowDesc.showBorder = mDesc.showBorder;
		windowDesc.allowResize = mDesc.allowResize;
		windowDesc.enableDoubleClick = true;
		windowDesc.fullscreen = mDesc.fullscreen;
		windowDesc.width = mDesc.videoMode.width;
		windowDesc.height = mDesc.videoMode.height;
		windowDesc.hidden = mDesc.hidden || mDesc.hideUntilSwap;
		windowDesc.left = mDesc.left;
		windowDesc.top = mDesc.top;
		windowDesc.outerDimensions = false;
		windowDesc.title = mDesc.title;
		windowDesc.toolWindow = mDesc.toolWindow;
		windowDesc.creationParams = this;
		windowDesc.modal = mDesc.modal;
		windowDesc.wndProc = &Win32Platform::_win32WndProc;

#ifdef BS_STATIC_LIB
		windowDesc.module = GetModuleHandle(NULL);
#else
		windowDesc.module = GetModuleHandle("bsfVulkanRenderAPI.dll");
#endif

		auto opt = mDesc.platformSpecific.find("parentWindowHandle");
		if (opt != mDesc.platformSpecific.end())
			windowDesc.parent = (HWND)parseUINT64(opt->second);

		opt = mDesc.platformSpecific.find("externalWindowHandle");
		if (opt != mDesc.platformSpecific.end())
			windowDesc.external = (HWND)parseUINT64(opt->second);
		
		const Win32VideoModeInfo& videoModeInfo = static_cast<const Win32VideoModeInfo&>(RenderAPI::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs > 0)
		{
			UINT32 actualMonitorIdx = std::min(mDesc.videoMode.outputIdx, numOutputs - 1);
			const Win32VideoOutputInfo& outputInfo = static_cast<const Win32VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));
			windowDesc.monitor = outputInfo.getMonitorHandle();
		}

		// Must be set before creating a window, since wndProc will call ShowWindow if needed after creation
		if (!windowDesc.external)
		{
			mShowOnSwap = mDesc.hideUntilSwap && !mDesc.hidden;
			props.isHidden = mDesc.hideUntilSwap || mDesc.hidden;
		}

		mWindow = bs_new<Win32Window>(windowDesc);

		mIsChild = windowDesc.parent != nullptr;
		mDisplayFrequency = Math::roundToInt(mDesc.videoMode.refreshRate);

		// Update local properties
		props.isFullScreen = mDesc.fullscreen && !mIsChild;
		props.width = mWindow->getWidth();
		props.height = mWindow->getHeight();
		props.top = mWindow->getTop();
		props.left = mWindow->getLeft();
		props.hwGamma = mDesc.gamma;
		props.multisampleCount = 1;

		// Create Vulkan surface
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = nullptr;
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.hwnd = mWindow->getHWnd();
		surfaceCreateInfo.hinstance = windowDesc.module;

		VkInstance instance = mRenderAPI._getInstance();
		VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, gVulkanAllocator, &mSurface);
		assert(result == VK_SUCCESS);

		SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
		VkPhysicalDevice physicalDevice = presentDevice->getPhysical();

		mPresentQueueFamily = presentDevice->getQueueFamily(GQT_GRAPHICS);
		
		VkBool32 supportsPresent;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, mPresentQueueFamily, mSurface, &supportsPresent);

		if(!supportsPresent)
		{
			// Note: Not supporting present only queues at the moment
			// Note: Also present device can only return one family of graphics queue, while there could be more (some of
			// which support present)
			BS_EXCEPT(RenderingAPIException, "Cannot find a graphics queue that also supports present operations.");
		}

		SurfaceFormat format = presentDevice->getSurfaceFormat(mSurface, mDesc.gamma);
		mColorFormat = format.colorFormat;
		mColorSpace = format.colorSpace;
		mDepthFormat = format.depthFormat;

		// Create swap chain
		mSwapChain = presentDevice->getResourceManager().create<VulkanSwapChain>(mSurface, props.width, props.height,
			props.vsync, mColorFormat, mColorSpace, mDesc.depthBuffer, mDepthFormat);

		// Make the window full screen if required
		if (!windowDesc.external)
		{
			if (props.isFullScreen)
			{
				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = 32;
				displayDeviceMode.dmPelsWidth = props.width;
				displayDeviceMode.dmPelsHeight = props.height;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

					if (ChangeDisplaySettingsEx(NULL, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings with user display frequency failed.");
					}
				}

				if (ChangeDisplaySettingsEx(NULL, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings failed.");
				}
			}
		}

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties = props;
		}

		bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		RenderWindow::initialize();
	}

	void Win32RenderWindow::acquireBackBuffer()
	{
		// We haven't presented the current back buffer yet, so just use that one
		if (!mRequiresNewBackBuffer)
			return;

		VkResult acquireResult = mSwapChain->acquireBackBuffer();
		if(acquireResult == VK_SUBOPTIMAL_KHR || acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			rebuildSwapChain();
			mSwapChain->acquireBackBuffer();
		}

		mRequiresNewBackBuffer = false;
	}

	void Win32RenderWindow::swapBuffers(UINT32 syncMask)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mShowOnSwap)
			setHidden(false);

		// Get a command buffer on which we'll submit
		SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();

		// Assuming present queue is always graphics
		assert(presentDevice->getQueueFamily(GQT_GRAPHICS) == mPresentQueueFamily);

		// Find an appropriate queue to execute on
		VulkanQueue* queue = presentDevice->getQueue(GQT_GRAPHICS, 0);
		UINT32 queueMask = presentDevice->getQueueMask(GQT_GRAPHICS, 0);

		// Ignore myself
		syncMask &= ~queueMask;

		UINT32 deviceIdx = presentDevice->getIndex();
		VulkanCommandBufferManager& cbm = static_cast<VulkanCommandBufferManager&>(CommandBufferManager::instance());

		UINT32 numSemaphores;
		cbm.getSyncSemaphores(deviceIdx, syncMask, mSemaphoresTemp, numSemaphores);

		// Wait on present (i.e. until the back buffer becomes available), if we haven't already done so
		const SwapChainSurface& surface = mSwapChain->getBackBuffer();
		if(surface.needsWait)
		{
			mSemaphoresTemp[numSemaphores] = mSwapChain->getBackBuffer().sync;
			numSemaphores++;

			mSwapChain->notifyBackBufferWaitIssued();
		}

		VkResult presentResult = queue->present(mSwapChain, mSemaphoresTemp, numSemaphores);
		if(presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR)
			rebuildSwapChain();

		mRequiresNewBackBuffer = true;
	}

	void Win32RenderWindow::move(INT32 left, INT32 top)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = mProperties;

		if (!props.isFullScreen)
		{
			mWindow->move(left, top);

			props.top = mWindow->getTop();
			props.left = mWindow->getLeft();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.top = props.top;
				mSyncedProperties.left = props.left;
			}

			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void Win32RenderWindow::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = mProperties;

		if (!props.isFullScreen)
		{
			mWindow->resize(width, height);

			props.width = mWindow->getWidth();
			props.height = mWindow->getHeight();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.width = props.width;
				mSyncedProperties.height = props.height;
			}

			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void Win32RenderWindow::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->setActive(state);

		RenderWindow::setActive(state);
	}

	void Win32RenderWindow::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		mShowOnSwap = false;
		mWindow->setHidden(hidden);

		RenderWindow::setHidden(hidden);
	}

	void Win32RenderWindow::minimize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->minimize();
	}

	void Win32RenderWindow::maximize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->maximize();
	}

	void Win32RenderWindow::restore()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->restore();
	}

	void Win32RenderWindow::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const Win32VideoModeInfo& videoModeInfo = static_cast<const Win32VideoModeInfo&>(RenderAPI::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		RenderWindowProperties& props = mProperties;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const Win32VideoOutputInfo& outputInfo = static_cast<const Win32VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		mDisplayFrequency = Math::roundToInt(refreshRate);
		props.isFullScreen = true;

		DEVMODE displayDeviceMode;

		memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
		displayDeviceMode.dmSize = sizeof(DEVMODE);
		displayDeviceMode.dmBitsPerPel = 32;
		displayDeviceMode.dmPelsWidth = width;
		displayDeviceMode.dmPelsHeight = height;
		displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
		displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		HMONITOR hMonitor = outputInfo.getMonitorHandle();
		MONITORINFOEX monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		if (ChangeDisplaySettingsEx(monitorInfo.szDevice, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
		{
			BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings failed");
		}

		props.top = monitorInfo.rcMonitor.top;
		props.left = monitorInfo.rcMonitor.left;
		props.width = width;
		props.height = height;

		SetWindowLong(mWindow->getHWnd(), GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		SetWindowLong(mWindow->getHWnd(), GWL_EXSTYLE, 0);

		SetWindowPos(mWindow->getHWnd(), HWND_TOP, props.left, props.top, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	void Win32RenderWindow::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		setFullscreen(mode.width, mode.height, mode.refreshRate, mode.outputIdx);
	}

	void Win32RenderWindow::setWindowed(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = mProperties;

		if (!props.isFullScreen)
			return;

		props.isFullScreen = false;
		props.width = width;
		props.height = height;

		// Drop out of fullscreen
		ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);

		UINT32 winWidth = width;
		UINT32 winHeight = height;

		RECT rect;
		SetRect(&rect, 0, 0, winWidth, winHeight);

		AdjustWindowRect(&rect, mWindow->getStyle(), false);
		winWidth = rect.right - rect.left;
		winHeight = rect.bottom - rect.top;

		// Deal with centering when switching down to smaller resolution
		HMONITOR hMonitor = MonitorFromWindow(mWindow->getHWnd(), MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		INT32 left = screenw > INT32(winWidth) ? ((screenw - INT32(winWidth)) / 2) : 0;
		INT32 top = screenh > INT32(winHeight) ? ((screenh - INT32(winHeight)) / 2) : 0;

		SetWindowLong(mWindow->getHWnd(), GWL_STYLE, mWindow->getStyle() | WS_VISIBLE);
		SetWindowLong(mWindow->getHWnd(), GWL_EXSTYLE, mWindow->getStyleEx());

		SetWindowPos(mWindow->getHWnd(), HWND_NOTOPMOST, left, top, winWidth, winHeight,
			SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties.width = props.width;
			mSyncedProperties.height = props.height;
		}

		bs::RenderWindowManager::instance().notifySyncDataDirty(this);
	}

	void Win32RenderWindow::setVSync(bool enabled, UINT32 interval)
	{
		mProperties.vsync = enabled;
		mProperties.vsyncInterval = interval;

		rebuildSwapChain();

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties.vsync = enabled;
			mSyncedProperties.vsyncInterval = interval;
		}

		bs::RenderWindowManager::instance().notifySyncDataDirty(this);
	}

	HWND Win32RenderWindow::_getWindowHandle() const
	{
		return mWindow->getHWnd();
	}

	void Win32RenderWindow::getCustomAttribute(const String& name, void* data) const
	{
		if (name == "FB")
		{
			VulkanFramebuffer** fb = (VulkanFramebuffer**)data;
			*fb = mSwapChain->getBackBuffer().framebuffer;
			return;
		}

		if(name == "SC")
		{
			VulkanSwapChain** sc = (VulkanSwapChain**)data;
			*sc = mSwapChain;
			return;
		}

		if(name == "WINDOW")
		{
			UINT64 *pWnd = (UINT64*)data;
			*pWnd = (UINT64)mWindow->getHWnd();
			return;
		}

		RenderWindow::getCustomAttribute(name, data);
	}

	void Win32RenderWindow::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mWindow)
			return;

		mWindow->_windowMovedOrResized();

		RenderWindowProperties& props = mProperties;
		if (!props.isFullScreen) // Fullscreen is handled directly by this object
		{
			props.top = mWindow->getTop();
			props.left = mWindow->getLeft();
			props.width = mWindow->getWidth();
			props.height = mWindow->getHeight();
		}

		rebuildSwapChain();
	}

	void Win32RenderWindow::syncProperties()
	{
		ScopedSpinLock lock(mLock);
		mProperties = mSyncedProperties;
	}

	void Win32RenderWindow::rebuildSwapChain()
	{
		//// Need to make sure nothing is using the swap buffer before we re-create it
		// Note: Optionally I can detect exactly on which queues (if any) are the swap chain images used on, and only wait
		// on those
		SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
		presentDevice->waitIdle();

		VulkanSwapChain* oldSwapChain = mSwapChain;

		mSwapChain = presentDevice->getResourceManager().create<VulkanSwapChain>(mSurface, mProperties.width,
			mProperties.height, mProperties.vsync, mColorFormat, mColorSpace, mDesc.depthBuffer, mDepthFormat,
			oldSwapChain);

		oldSwapChain->destroy();
	}

	}
}
