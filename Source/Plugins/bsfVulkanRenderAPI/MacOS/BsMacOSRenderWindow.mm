//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#define BS_COCOA_INTERNALS
#define BS_METAL_INTERNALS

#include "MacOS/BsMacOSRenderWindow.h"
#include "MacOS/BsMacOSVideoModeInfo.h"
#include "CoreThread/BsCoreThread.h"
#include "Private/MacOS/BsMacOSPlatform.h"
#include "Private/MacOS/BsMacOSWindow.h"
#include "Math/BsMath.h"
#include "Managers/BsRenderWindowManager.h"
#include "Managers/BsVulkanCommandBufferManager.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"
#include "BsVulkanSwapChain.h"
#include "BsVulkanQueue.h"

#ifdef BS_DEBUG_MODE
#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

namespace bs
{
	MacOSRenderWindow::MacOSRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId)
		: RenderWindow(desc, windowId), mProperties(desc)
	{ }
	
	void MacOSRenderWindow::initialize()
	{
		WINDOW_DESC windowDesc;
		windowDesc.x = mDesc.left;
		windowDesc.y = mDesc.top;
		windowDesc.width = mDesc.videoMode.getWidth();
		windowDesc.height = mDesc.videoMode.getHeight();
		windowDesc.title = mDesc.title;
		windowDesc.showDecorations = mDesc.showTitleBar;
		windowDesc.allowResize = mDesc.allowResize;
		windowDesc.modal = mDesc.modal;
		windowDesc.floating = mDesc.toolWindow;
		windowDesc.enableMetal = true;

		mWindow = bs_new<CocoaWindow>(windowDesc);
		mWindow->_setUserData(this);
		
		RenderWindow::initialize();
	}

	void MacOSRenderWindow::getCustomAttribute(const String& name, void* data) const
	{
		if (name == "WINDOW" || name == "COCOA_WINDOW")
		{
			blockUntilCoreInitialized();
			getCore()->getCustomAttribute(name, data);
			return;
		}
	}

	void MacOSRenderWindow::destroy()
	{
		// Make sure to set the original desktop video mode before we exit
		if(mProperties.isFullScreen)
			setWindowed(50, 50);

		if (mWindow != nullptr)
		{
			bs_delete(mWindow);
			mWindow = nullptr;
		}

		RenderWindow::destroy();
	}

	SPtr<ct::CoreObject> MacOSRenderWindow::createCore() const
	{
		ct::VulkanRenderAPI& rapi = static_cast<ct::VulkanRenderAPI&>(ct::RenderAPI::instance());
		
		RENDER_WINDOW_DESC desc = mDesc;
		SPtr<ct::CoreObject> coreObj = bs_shared_ptr_new<ct::MacOSRenderWindow>(desc, mWindowId, mWindow, rapi);
		coreObj->_setThisPtr(coreObj);

		return coreObj;
	}

	Vector2I MacOSRenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		return mWindow->screenToWindowPos(screenPos);
	}

	Vector2I MacOSRenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		return mWindow->windowToScreenPos(windowPos);
	}

	SPtr<ct::MacOSRenderWindow> MacOSRenderWindow::getCore() const
	{
		return std::static_pointer_cast<ct::MacOSRenderWindow>(mCoreSpecific);
	}

	void MacOSRenderWindow::syncProperties()
	{
		ScopedSpinLock lock(getCore()->_getPropertiesLock());
		mProperties = getCore()->mSyncedProperties;
	}
	
	// ----------------------------------------------------------------------------------------------------------------
	
	namespace ct
	{
		MacOSRenderWindow::MacOSRenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, CocoaWindow* mWindow, VulkanRenderAPI& renderAPI)
			: RenderWindow(desc, windowId), mWindow(mWindow),  mIsChild(false), mShowOnSwap(false)
		, mRenderAPI(renderAPI), mProperties(desc),mSyncedProperties(desc), mRequiresNewBackBuffer(true)
		{
		}

		MacOSRenderWindow::~MacOSRenderWindow()
		{
			SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
			presentDevice->waitIdle();

			mSwapChain = nullptr;
			vkDestroySurfaceKHR(mRenderAPI._getInstance(), mSurface, gVulkanAllocator);
		}

		void MacOSRenderWindow::initialize()
		{
			RenderWindowProperties& props = mProperties;
			mIsChild = false;
			
			auto iter = mDesc.platformSpecific.find("parentWindowHandle");
			mIsChild = iter != mDesc.platformSpecific.end();
			
			props.isFullScreen = mDesc.fullscreen && !mIsChild;
			props.isHidden = mDesc.hidden;
			
			Rect2I area = mWindow->getArea();
			props.width = area.width;
			props.height = area.height;
			props.top = area.y;
			props.left = area.x;
			props.hasFocus = true;
			
			props.hwGamma = mDesc.gamma;
			props.multisampleCount = mDesc.multisampleCount;
			
			// Create Vulkan surface
			
			VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo;
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
			surfaceCreateInfo.pNext = nullptr;
			surfaceCreateInfo.flags = 0;
			surfaceCreateInfo.pView = (__bridge const void *)(mWindow->getMetalLayer());
			
			ct::VulkanRenderAPI& rapi = static_cast<ct::VulkanRenderAPI&>(ct::RenderAPI::instance());
			VkResult result = vkCreateMacOSSurfaceMVK(rapi._getInstance(), &surfaceCreateInfo, bs::ct::gVulkanAllocator, &mSurface);
			assert(result == VK_SUCCESS);
			
#ifdef BS_DEBUG_MODE
			//enableShaderConvertionDebugging();
#endif
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
			mSwapChain = bs_shared_ptr_new<VulkanSwapChain>();
			mSwapChain->rebuild(presentDevice, mSurface, props.width, props.height, props.vsync, mColorFormat, mColorSpace,
								mDesc.depthBuffer, mDepthFormat);
			
			if(mDesc.fullscreen && !mIsChild)
				setFullscreen(mDesc.videoMode);
			
			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties = props;
			}
			
			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
			
			RenderWindow::initialize();
		}

		void MacOSRenderWindow::acquireBackBuffer()
		{
			// We haven't presented the current back buffer yet, so just use that one
			if (!mRequiresNewBackBuffer)
				return;

			mSwapChain->acquireBackBuffer();
			mRequiresNewBackBuffer = false;
		}

		void MacOSRenderWindow::resize(UINT32 width, UINT32 height)
		{
			THROW_IF_NOT_CORE_THREAD;
			
			RenderWindowProperties& props = mProperties;
			if (!props.isFullScreen)
			{
				mWindow->resize(width, height);
				
				Rect2I area = mWindow->getArea();
				props.width = area.width;
				props.height = area.height;
				
				{
					ScopedSpinLock lock(mLock);
					getSyncedProperties().width = width;
					getSyncedProperties().height = height;
				}
				
				bs::RenderWindowManager::instance().notifySyncDataDirty(this);
			}
		}
		
		void MacOSRenderWindow::move(INT32 left, INT32 top)
		{
			THROW_IF_NOT_CORE_THREAD;
			
			RenderWindowProperties& props = mProperties;
			if (!props.isFullScreen)
			{
				mWindow->move(left, top);
				
				Rect2I area = mWindow->getArea();
				props.top = area.y;
				props.left = area.x;
				
				{
					ScopedSpinLock lock(mLock);
					getSyncedProperties().top = props.top;
					getSyncedProperties().left = props.left;
				}
				
				bs::RenderWindowManager::instance().notifySyncDataDirty(this);
			}
		}
		
		void MacOSRenderWindow::minimize()
		{
			THROW_IF_NOT_CORE_THREAD;
			mWindow->minimize();
		}
		
		void MacOSRenderWindow::maximize()
		{
			THROW_IF_NOT_CORE_THREAD;
			mWindow->maximize();
		}
		
		void MacOSRenderWindow::restore()
		{
			THROW_IF_NOT_CORE_THREAD;
			mWindow->restore();
		}
		
		void MacOSRenderWindow::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
		{
			VideoMode videoMode(width, height, refreshRate, monitorIdx);
			setFullscreen(videoMode);
		}
		
		void MacOSRenderWindow::setFullscreen(const VideoMode& videoMode)
		{
			if (mIsChild)
				return;
			
			const VideoModeInfo& videoModeInfo = ct::RenderAPI::instance().getVideoModeInfo();
			
			UINT32 outputIdx = videoMode.getOutputIdx();
			if(outputIdx >= videoModeInfo.getNumOutputs())
			{
				LOGERR("Invalid output device index.")
				return;
			}
			
			const VideoOutputInfo& outputInfo = videoModeInfo.getOutputInfo (outputIdx);
			
			if(!videoMode.isCustom())
				setDisplayMode(outputInfo, videoMode);
			else
			{
				// Look for mode matching the requested resolution
				UINT32 foundMode = (UINT32)-1;
				UINT32 numModes = outputInfo.getNumVideoModes();
				for (UINT32 i = 0; i < numModes; i++)
				{
					const VideoMode& currentMode = outputInfo.getVideoMode(i);
					
					if (currentMode.getWidth() == videoMode.getWidth() && currentMode.getHeight() == videoMode.getHeight())
					{
						foundMode = i;
						
						if (Math::approxEquals(currentMode.getRefreshRate(), videoMode.getRefreshRate()))
							break;
					}
				}
				
				if (foundMode == (UINT32)-1)
				{
					LOGERR("Unable to enter fullscreen, unsupported video mode requested.");
					return;
				}
				
				setDisplayMode(outputInfo, outputInfo.getVideoMode(foundMode));
			}
			
			mWindow->setFullscreen();
			
			RenderWindowProperties& props = mProperties;
			
			props.isFullScreen = mDesc.fullscreen;
			
			props.top = 0;
			props.left = 0;
			props.width = videoMode.getWidth();
			props.height = videoMode.getHeight();
			
			{
				ScopedSpinLock lock(mLock);
				getSyncedProperties().top = props.top;
				getSyncedProperties().left = props.left;
				getSyncedProperties().width = props.width;
				getSyncedProperties().height = props.height;
			}
			
			ct::RenderWindowManager::instance().notifySyncDataDirty(this);
			_windowMovedOrResized();
		}
		
		void MacOSRenderWindow::setDisplayMode(const VideoOutputInfo& output, const VideoMode& mode)
		{
			CGDisplayFadeReservationToken fadeToken = kCGDisplayFadeReservationInvalidToken;
			if (CGAcquireDisplayFadeReservation(5.0f, &fadeToken))
				CGDisplayFade(fadeToken, 0.3f, kCGDisplayBlendNormal, kCGDisplayBlendSolidColor, 0, 0, 0, TRUE);
			
			auto& destOutput = static_cast<const ct::MacOSVideoOutputInfo&>(output);
			auto& newMode = static_cast<const ct::MacOSVideoMode&>(mode);
			
			// Note: An alternative to changing display resolution would be to only change the back-buffer size. But that doesn't
			// account for refresh rate, so it's questionable how useful it would be.
			CGDirectDisplayID displayID = destOutput._getDisplayID();
			CGDisplaySetDisplayMode(displayID, newMode._getModeRef(), nullptr);
			
			if (fadeToken != kCGDisplayFadeReservationInvalidToken)
			{
				CGDisplayFade(fadeToken, 0.3f, kCGDisplayBlendSolidColor, kCGDisplayBlendNormal, 0, 0, 0, FALSE);
				CGReleaseDisplayFadeReservation(fadeToken);
			}
		}

		void MacOSRenderWindow::setVSync(bool enabled, UINT32 interval)
		{
			// Rebuild swap chain
			
			//// Need to make sure nothing is using the swap buffer before we re-create it
			// Note: Optionally I can detect exactly on which queues (if any) are the swap chain images used on, and only wait
			// on those
			SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
			presentDevice->waitIdle();
			
			mSwapChain->rebuild(presentDevice, mSurface, mProperties.width, mProperties.height, enabled, mColorFormat, mColorSpace,
								mDesc.depthBuffer, mDepthFormat);
			
			mProperties.vsync = enabled;
			mProperties.vsyncInterval = interval;
			
			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.vsync = enabled;
				mSyncedProperties.vsyncInterval = interval;
			}
			
			bs::RenderWindowManager::instance().notifySyncDataDirty(this);
		}

		void MacOSRenderWindow::swapBuffers(UINT32 syncMask)
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
			
			queue->present(mSwapChain.get(), mSemaphoresTemp, numSemaphores);
			mRequiresNewBackBuffer = true;
		}

		void MacOSRenderWindow::getCustomAttribute(const String& name, void* data) const
		{
			if (name == "FB")
			{
				VulkanFramebuffer** fb = (VulkanFramebuffer**)data;
				*fb = mSwapChain->getBackBuffer().framebuffer;
				return;
			}
			else if(name == "SC")
			{
				VulkanSwapChain** sc = (VulkanSwapChain**)data;
				*sc = mSwapChain.get();
				return;
			}
			else if(name == "COCOA_WINDOW")
			{
				CocoaWindow** window = (CocoaWindow**)data;
				*window = mWindow;
				return;
			}
			else if(name == "WINDOW_ID")
			{
				UINT32* windowId = (UINT32*)data;
				*windowId = mWindow->_getWindowId();
				return;
			}
			else if(name == "WINDOW")
			{
				CocoaWindow** window = (CocoaWindow**)data;
				*window = mWindow;
				return;
			}

		}

		void MacOSRenderWindow::_windowMovedOrResized()
		{
			THROW_IF_NOT_CORE_THREAD;
			
			if (!mWindow)
				return;
			
			RenderWindowProperties& props = mProperties;

			// Resize swap chain
			
			//// Need to make sure nothing is using the swap buffer before we re-create it
			// Note: Optionally I can detect exactly on which queues (if any) are the swap chain images used on, and only wait
			// on those
			SPtr<VulkanDevice> presentDevice = mRenderAPI._getPresentDevice();
			presentDevice->waitIdle();
			
			mSwapChain->rebuild(presentDevice, mSurface, props.width, props.height, props.vsync, mColorFormat, mColorSpace,
								mDesc.depthBuffer, mDepthFormat);
		}

		void MacOSRenderWindow::syncProperties()
		{
			ScopedSpinLock lock(mLock);
			mProperties = mSyncedProperties;
		}
		
		void MacOSRenderWindow::enableShaderConvertionDebugging()
		{
			ct::VulkanRenderAPI& rapi = static_cast<ct::VulkanRenderAPI&>(ct::RenderAPI::instance());
			
			MVKConfiguration mvkConfig;
			size_t appConfigSize = sizeof(mvkConfig);
			vkGetMoltenVKConfigurationMVK(rapi._getInstance(), &mvkConfig, &appConfigSize);
			mvkConfig.debugMode = true;
			vkSetMoltenVKConfigurationMVK(rapi._getInstance(), &mvkConfig, &appConfigSize);
		}

	}
}

