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

	void RenderWindowCore::destroy()
	{
		Platform::resetNonClientAreas(*this);

		RenderWindowCoreManager::instance().windowDestroyed(this);

		RenderTargetCore::destroy();
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
		props = data.getData<RenderWindowProperties>();
	}

	const RenderWindowProperties& RenderWindow::getProperties() const
	{
		return static_cast<const RenderWindowProperties&>(getPropertiesInternal());
	}
}
