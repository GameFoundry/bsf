#include "BsRenderWindow.h"
#include "BsCoreThread.h"
#include "BsRenderWindowManager.h"
#include "BsViewport.h"
#include "BsPlatform.h"

namespace BansheeEngine 
{
	void RenderWindowProperties::copyToBuffer(UINT8* buffer) const
	{
		*(RenderWindowProperties*)buffer = *this;
	}

	void RenderWindowProperties::copyFromBuffer(UINT8* buffer)
	{
		*this = *(RenderWindowProperties*)buffer;
	}

	UINT32 RenderWindowProperties::getSize() const
	{
		return sizeof(RenderWindowProperties);
	}

	RenderWindowCore::RenderWindowCore(RenderWindow* parent, RenderWindowProperties* properties)
		:RenderTargetCore(parent, properties)
	{

	}

	void RenderWindowCore::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;
	}

	void RenderWindowCore::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;
	}

	void RenderWindowCore::_windowFocusReceived()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties* properties = static_cast<RenderWindowProperties*>(mProperties);
		properties->mHasFocus = true;

		markCoreDirty();
	}

	void RenderWindowCore::_windowFocusLost()
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderWindowProperties* properties = static_cast<RenderWindowProperties*>(mProperties);
		properties->mHasFocus = false;

		markCoreDirty();
	}

	RenderWindow* RenderWindowCore::getNonCore() const 
	{ 
		return static_cast<RenderWindow*>(mParent); 
	}

	void RenderWindow::initialize(const RENDER_WINDOW_DESC& desc)
	{
		mDesc = desc;

		mProperties = createProperties();
		RenderWindowProperties* properties = static_cast<RenderWindowProperties*>(mProperties);

		properties->mWidth = desc.videoMode.getWidth();
		properties->mHeight = desc.videoMode.getHeight();
		properties->mHwGamma = desc.gamma;
		properties->mVSync = desc.vsync;
		properties->mVSyncInterval = desc.vsyncInterval;
		properties->mMultisampleCount = desc.multisampleCount;
		properties->mLeft = desc.left;
		properties->mTop = desc.top;
		properties->mIsFullScreen = desc.fullscreen;
		properties->mHidden = desc.hidden;
		properties->mIsModal = desc.modal;
		properties->mIsWindow = true;
		properties->mRequiresTextureFlipping = requiresTextureFlipping();

		RenderTarget::initialize();
	}

	void RenderWindow::destroy()
	{
		Platform::resetNonClientAreas(*this);

		RenderWindowManager::instance().windowDestroyed(this);

		RenderTarget::destroy();
	}

	const RenderWindowProperties& RenderWindow::getProperties() const
	{
		THROW_IF_CORE_THREAD;

		return static_cast<const RenderWindowProperties&>(RenderTarget::getProperties());
	}

	SPtr<RenderWindowCore> RenderWindow::getCore() const
	{
		return std::static_pointer_cast<RenderWindowCore>(mCoreSpecific);
	}

	RenderWindowPtr RenderWindow::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		return RenderWindowManager::instance().create(desc, parentWindow);
	}
}
