#include "BsRenderWindow.h"
#include "BsCoreThread.h"
#include "BsRenderWindowManager.h"
#include "BsViewport.h"
#include "BsPlatform.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine 
{
	RenderWindowProperties::RenderWindowProperties(const RENDER_WINDOW_DESC& desc)
	{
		mWidth = desc.videoMode.getWidth();
		mHeight = desc.videoMode.getHeight();
		mHwGamma = desc.gamma;
		mVSync = desc.vsync;
		mVSyncInterval = desc.vsyncInterval;
		mMultisampleCount = desc.multisampleCount;
		mLeft = desc.left;
		mTop = desc.top;
		mIsFullScreen = desc.fullscreen;
		mHidden = desc.hidden;
		mIsModal = desc.modal;
		mIsWindow = true;
		mRequiresTextureFlipping = false;
	}

	RenderWindowCore::RenderWindowCore(const RENDER_WINDOW_DESC& desc)
		:mDesc(desc)
	{

	}

	RenderWindowCore::~RenderWindowCore()
	{
		Platform::resetNonClientAreas(*this);

		RenderWindowCoreManager::instance().windowDestroyed(this);
	}

	void RenderWindowCore::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;
	}

	void RenderWindowCore::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		props.mActive = state;
		RenderWindowManager::instance().notifyPropertiesDirty(this);
	}

	void RenderWindowCore::syncToCore(const CoreSyncData& data)
	{
		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());
		props = data.getData<RenderWindowProperties>();
	}

	void RenderWindowCore::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowManager::instance().notifyMovedOrResized(this);
	}

	void RenderWindowCore::_windowFocusReceived()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& properties = const_cast<RenderWindowProperties&>(getProperties());
		properties.mHasFocus = true;

		RenderWindowManager::instance().notifyFocusReceived(this);
	}

	void RenderWindowCore::_windowFocusLost()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& properties = const_cast<RenderWindowProperties&>(getProperties());
		properties.mHasFocus = false;

		RenderWindowManager::instance().notifyFocusLost(this);
	}

	void RenderWindowCore::_notifyMaximized()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		props.mIsMaximized = true;
		RenderWindowManager::instance().notifyPropertiesDirty(this);
	}

	void RenderWindowCore::_notifyMinimized()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		props.mIsMaximized = false;
		RenderWindowManager::instance().notifyPropertiesDirty(this);
	}

	void RenderWindowCore::_notifyRestored()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		props.mIsMaximized = false;
		RenderWindowManager::instance().notifyPropertiesDirty(this);
	}

	const RenderWindowProperties& RenderWindowCore::getProperties() const
	{
		return static_cast<const RenderWindowProperties&>(getPropertiesInternal());
	}

	void RenderWindow::destroy()
	{
		RenderWindowManager::instance().notifyWindowDestroyed(this);

		RenderTarget::destroy();
	}

	RenderWindow::RenderWindow(const RENDER_WINDOW_DESC& desc)
		:mDesc(desc)
	{

	}

	void RenderWindow::resize(CoreAccessor& accessor, UINT32 width, UINT32 height)
	{
		std::function<void(SPtr<RenderWindowCore>, UINT32, UINT32)> resizeFunc =
			[](SPtr<RenderWindowCore> renderWindow, UINT32 width, UINT32 height)
		{
			renderWindow->resize(width, height);
		};

		getMutableProperties().mWidth = width;
		getMutableProperties().mHeight = height;
		onResized();

		accessor.queueCommand(std::bind(resizeFunc, getCore(), width, height));
	}

	void RenderWindow::move(CoreAccessor& accessor, INT32 left, INT32 top)
	{
		std::function<void(SPtr<RenderWindowCore>, INT32, INT32)> moveFunc =
			[](SPtr<RenderWindowCore> renderWindow, INT32 left, INT32 top)
		{
			renderWindow->move(left, top);
		};

		getMutableProperties().mLeft = left;
		getMutableProperties().mTop = top;
		accessor.queueCommand(std::bind(moveFunc, getCore(), left, top));
	}

	void RenderWindow::hide(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> hideFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->setHidden(true);
		};

		getMutableProperties().mHidden = true;
		accessor.queueCommand(std::bind(hideFunc, getCore()));
	}

	void RenderWindow::show(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> showFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->setHidden(false);
		};

		getMutableProperties().mHidden = false;
		accessor.queueCommand(std::bind(showFunc, getCore()));
	}

	void RenderWindow::minimize(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> minimizeFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->minimize();
		};

		accessor.queueCommand(std::bind(minimizeFunc, getCore()));
	}

	void RenderWindow::maximize(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> maximizeFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->maximize();
		};

		accessor.queueCommand(std::bind(maximizeFunc, getCore()));
	}

	void RenderWindow::restore(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> restoreFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->restore();
		};

		accessor.queueCommand(std::bind(restoreFunc, getCore()));
	}

	void RenderWindow::setFullscreen(CoreAccessor& accessor, UINT32 width, UINT32 height,
		float refreshRate, UINT32 monitorIdx)
	{
		std::function<void(SPtr<RenderWindowCore>, UINT32, UINT32, float, UINT32)> fullscreenFunc =
			[](SPtr<RenderWindowCore> renderWindow, UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
		{
			renderWindow->setFullscreen(width, height, refreshRate, monitorIdx);
		};

		accessor.queueCommand(std::bind(fullscreenFunc, getCore(), width, height, refreshRate, monitorIdx));
	}

	void RenderWindow::setFullscreen(CoreAccessor& accessor, const VideoMode& mode)
	{
		std::function<void(SPtr<RenderWindowCore>, const VideoMode&)> fullscreenFunc =
			[](SPtr<RenderWindowCore> renderWindow, const VideoMode& mode)
		{
			renderWindow->setFullscreen(mode);
		};

		accessor.queueCommand(std::bind(fullscreenFunc, getCore(), std::cref(mode)));
	}

	void RenderWindow::setWindowed(CoreAccessor& accessor, UINT32 width, UINT32 height)
	{
		std::function<void(SPtr<RenderWindowCore>, UINT32, UINT32)> windowedFunc =
			[](SPtr<RenderWindowCore> renderWindow, UINT32 width, UINT32 height)
		{
			renderWindow->setWindowed(width, height);
		};

		accessor.queueCommand(std::bind(windowedFunc, getCore(), width, height));
	}

	SPtr<RenderWindowCore> RenderWindow::getCore() const
	{
		return std::static_pointer_cast<RenderWindowCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> RenderWindow::createCore() const
	{
		RENDER_WINDOW_DESC desc = mDesc;
		return RenderWindowCoreManager::instance().createInternal(desc);
	}

	RenderWindowPtr RenderWindow::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		return RenderWindowManager::instance().create(desc, parentWindow);
	}

	RenderWindowProperties& RenderWindow::getMutableProperties()
	{
		return const_cast<RenderWindowProperties&>(getProperties());
	}

	CoreSyncData RenderWindow::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderWindowProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	const RenderWindowProperties& RenderWindow::getProperties() const
	{
		return static_cast<const RenderWindowProperties&>(getPropertiesInternal());
	}
}
