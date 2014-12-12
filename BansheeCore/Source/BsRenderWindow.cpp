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

	CoreSyncData RenderWindowCore::syncFromCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderWindowProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void RenderWindowCore::syncToCore(const CoreSyncData& data)
	{
		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());
		props = data.getData<RenderWindowProperties>();
	}

	void RenderWindowCore::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;
	}

	void RenderWindowCore::_windowFocusReceived()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& properties = const_cast<RenderWindowProperties&>(getProperties());
		properties.mHasFocus = true;

		markCoreDirty();
	}

	void RenderWindowCore::_windowFocusLost()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties& properties = const_cast<RenderWindowProperties&>(getProperties());
		properties.mHasFocus = false;

		markCoreDirty();
	}

	const RenderWindowProperties& RenderWindowCore::getProperties() const
	{
		return static_cast<const RenderWindowProperties&>(getPropertiesInternal());
	}

	void RenderWindow::destroy()
	{
		RenderWindowManager::instance().windowDestroyed(this);

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

		accessor.queueCommand(std::bind(resizeFunc, getCore(), width, height));
	}

	void RenderWindow::move(CoreAccessor& accessor, INT32 left, INT32 top)
	{
		std::function<void(SPtr<RenderWindowCore>, INT32, INT32)> moveFunc =
			[](SPtr<RenderWindowCore> renderWindow, INT32 left, INT32 top)
		{
			renderWindow->move(left, top);
		};

		accessor.queueCommand(std::bind(moveFunc, getCore(), left, top));
	}

	void RenderWindow::hide(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> hideFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->setHidden(true);
		};

		accessor.queueCommand(std::bind(hideFunc, getCore()));
	}

	void RenderWindow::show(CoreAccessor& accessor)
	{
		std::function<void(SPtr<RenderWindowCore>)> showFunc =
			[](SPtr<RenderWindowCore> renderWindow)
		{
			renderWindow->setHidden(false);
		};

		accessor.queueCommand(std::bind(showFunc, getCore()));
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

	CoreSyncData RenderWindow::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderWindowProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void RenderWindow::syncFromCore(const CoreSyncData& data)
	{
		RenderWindowProperties& props = const_cast<RenderWindowProperties&>(getProperties());

		const RenderWindowProperties& newProps = data.getData<RenderWindowProperties>();

		bool movedOrResized = props.getHeight() != newProps.getHeight() || props.getWidth() != newProps.getWidth() || props.getLeft() != newProps.getLeft()
			|| props.getTop() != newProps.getTop() || props.isFullScreen() != newProps.isFullScreen();

		props = newProps;
		if (movedOrResized)
			RenderWindowManager::instance().windowMovedOrResized(this);
	}

	const RenderWindowProperties& RenderWindow::getProperties() const
	{
		return static_cast<const RenderWindowProperties&>(getPropertiesInternal());
	}
}
