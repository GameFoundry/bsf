#include "BsRenderWindowManager.h"
#include "BsPlatform.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	RenderWindowManager::RenderWindowManager()
		:mWindowInFocus(nullptr), mNewWindowInFocus(nullptr)
	{
		Platform::onMouseLeftWindow.connect(std::bind(&RenderWindowManager::windowMouseLeft, this, _1));
	}

	RenderWindowManager::~RenderWindowManager()
	{

	}

	RenderWindowPtr RenderWindowManager::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		UINT32 id = RenderWindowCoreManager::instance().mNextWindowId.fetch_add(1, std::memory_order_relaxed);

		RenderWindowPtr renderWindow = createImpl(desc, id, parentWindow);
		renderWindow->_setThisPtr(renderWindow);
		
		{
			BS_LOCK_MUTEX(mWindowMutex);

			mWindows[renderWindow->mWindowId] = renderWindow.get();
		}

		renderWindow->initialize();
		
		return renderWindow;
	}

	void RenderWindowManager::notifyWindowDestroyed(RenderWindow* window)
	{
		{
			BS_LOCK_MUTEX(mWindowMutex);

			auto iterFind = std::find_if(begin(mMovedOrResizedWindows), end(mMovedOrResizedWindows), 
				[&](const MoveOrResizeData& x) { return x.window == window; });

			if(iterFind != mMovedOrResizedWindows.end())
				mMovedOrResizedWindows.erase(iterFind);

			if (mNewWindowInFocus == window)
				mNewWindowInFocus = nullptr;

			mWindows.erase(window->mWindowId);
			mDirtyProperties.erase(window);
		}
	}

	void RenderWindowManager::notifyFocusReceived(RenderWindowCore* coreWindow)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		RenderWindow* window = getNonCore(coreWindow);
		mNewWindowInFocus = window;
	}

	void RenderWindowManager::notifyFocusLost(RenderWindowCore* coreWindow)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		mNewWindowInFocus = nullptr;
	}

	void RenderWindowManager::notifyMovedOrResized(RenderWindowCore* coreWindow)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		RenderWindow* window = getNonCore(coreWindow);
		if (window == nullptr)
			return;

		auto iterFind = std::find_if(begin(mMovedOrResizedWindows), end(mMovedOrResizedWindows), 
			[&](const MoveOrResizeData& x) { return x.window == window; });

		const RenderWindowProperties& props = coreWindow->getProperties();
		MoveOrResizeData* moveResizeData = nullptr;

		if (iterFind != end(mMovedOrResizedWindows))
		{
			moveResizeData = &*iterFind;
		}
		else
		{
			MoveOrResizeData newEntry;
			newEntry.window = window;

			mMovedOrResizedWindows.push_back(newEntry);
			moveResizeData = &mMovedOrResizedWindows.back();
		}
		
		moveResizeData->x = props.getLeft();
		moveResizeData->y = props.getTop();
		moveResizeData->width = props.getWidth();
		moveResizeData->height = props.getHeight();
	}

	void RenderWindowManager::notifySyncDataDirty(RenderWindowCore* coreWindow)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		RenderWindow* window = getNonCore(coreWindow);

		if (window != nullptr)
			mDirtyProperties.insert(window);
	}

	void RenderWindowManager::windowMouseLeft(RenderWindowCore* coreWindow)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		RenderWindow* window = getNonCore(coreWindow);
		auto iterFind = std::find(begin(mMouseLeftWindows), end(mMouseLeftWindows), window);

		if (iterFind == end(mMouseLeftWindows))
			mMouseLeftWindows.push_back(window);
	}

	void RenderWindowManager::_update()
	{
		RenderWindow* newWinInFocus = nullptr;
		Vector<MoveOrResizeData> movedOrResizedWindows;
		Vector<RenderWindow*> mouseLeftWindows;

		{
			BS_LOCK_MUTEX(mWindowMutex);
			newWinInFocus = mNewWindowInFocus;

			for (auto& moveResizeData : mMovedOrResizedWindows)
			{
				RenderWindow* window = moveResizeData.window;
				const RenderWindowProperties& props = window->getProperties();

				// Need to eliminate non-dirty ones because it's possible we already triggered the resize event
				// if the resize call originated from the sim thread, so we don't trigger it twice.

				bool isDirty = moveResizeData.x != props.getLeft() || moveResizeData.y != props.getTop()
					|| moveResizeData.width != props.getWidth() || moveResizeData.height != props.getHeight();

				if (isDirty)
					movedOrResizedWindows.push_back(moveResizeData);
			}

			mMovedOrResizedWindows.clear();

			mouseLeftWindows = mMouseLeftWindows;
			mMouseLeftWindows.clear();

			for (auto& dirtyPropertyWindow : mDirtyProperties)
				dirtyPropertyWindow->syncProperties();

			mDirtyProperties.clear();
		}

		if(mWindowInFocus != newWinInFocus)
		{
			if(mWindowInFocus != nullptr)
				onFocusLost(*mWindowInFocus);

			if(newWinInFocus != nullptr)
				onFocusGained(*newWinInFocus);

			mWindowInFocus = newWinInFocus;
		}

		for (auto& moveResizeData : movedOrResizedWindows)
		{
			moveResizeData.window->onResized();
		}

		if (!onMouseLeftWindow.empty())
		{
			for (auto& window : mouseLeftWindows)
				onMouseLeftWindow(*window);
		}			
	}

	Vector<RenderWindow*> RenderWindowManager::getRenderWindows() const
	{
		BS_LOCK_MUTEX(mWindowMutex);

		Vector<RenderWindow*> windows;
		for (auto& windowPair : mWindows)
			windows.push_back(windowPair.second);

		return windows;
	}

	RenderWindow* RenderWindowManager::getNonCore(const RenderWindowCore* window) const
	{
		auto iterFind = mWindows.find(window->mWindowId);

		if (iterFind != mWindows.end())
			return iterFind->second;

		return nullptr;
	}

	RenderWindowCoreManager::RenderWindowCoreManager()
	{
		mNextWindowId = 0;
	}

	SPtr<RenderWindowCore> RenderWindowCoreManager::create(RENDER_WINDOW_DESC& desc)
	{
		UINT32 id = mNextWindowId.fetch_add(1, std::memory_order_relaxed);

		SPtr<RenderWindowCore> renderWindow = createInternal(desc, id);
		renderWindow->initialize();

		return renderWindow;
	}

	void RenderWindowCoreManager::_update()
	{
		BS_LOCK_MUTEX(mWindowMutex);

		for (auto& dirtyPropertyWindow : mDirtyProperties)
			dirtyPropertyWindow->syncProperties();

		mDirtyProperties.clear();
	}

	void RenderWindowCoreManager::windowCreated(RenderWindowCore* window)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		mCreatedWindows.push_back(window);
	}

	void RenderWindowCoreManager::windowDestroyed(RenderWindowCore* window)
	{
		{
			BS_LOCK_MUTEX(mWindowMutex);

			auto iterFind = std::find(begin(mCreatedWindows), end(mCreatedWindows), window);

			if (iterFind == mCreatedWindows.end())
				BS_EXCEPT(InternalErrorException, "Trying to destroy a window that is not in the created windows list.");

			mCreatedWindows.erase(iterFind);
			mDirtyProperties.erase(window);
		}
	}

	Vector<RenderWindowCore*> RenderWindowCoreManager::getRenderWindows() const
	{
		BS_LOCK_MUTEX(mWindowMutex);

		return mCreatedWindows;
	}

	void RenderWindowCoreManager::notifySyncDataDirty(RenderWindowCore* window)
	{
		BS_LOCK_MUTEX(mWindowMutex);

		mDirtyProperties.insert(window);
	}
}