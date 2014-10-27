#include "BsRenderWindow.h"
#include "BsCoreThread.h"
#include "BsRenderWindowManager.h"
#include "BsViewport.h"
#include "BsPlatform.h"

namespace BansheeEngine 
{
	void RenderWindowProperties::copyFrom(const RenderTargetProperties& other)
	{
		const RenderWindowProperties& windowProps = static_cast<const RenderWindowProperties&>(other);

		*this = windowProps;
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
		properties->mMultisampleHint = desc.multisampleHint;
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

	RenderWindowCore* RenderWindow::getCore() const
	{
		return static_cast<RenderWindowCore*>(mCore);
	}

	RenderTargetCore* RenderWindow::createCore()
	{
		RenderWindowProperties* coreProperties = bs_new<RenderWindowProperties>();
		RenderWindowProperties* myProperties = static_cast<RenderWindowProperties*>(mProperties);

		*coreProperties = *myProperties;

		return createCore(coreProperties, mDesc);
	}

	RenderWindowPtr RenderWindow::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		return RenderWindowManager::instance().create(desc, parentWindow);
	}
}
