#include "BsDragAndDropManager.h"
#include "CmPlatform.h"
#include "CmApplication.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	DragAndDropManager::DragAndDropManager()
		:mIsDragInProgress(false), mDragTypeId(0), mData(nullptr), mCaptureChanged(false), mCaptureActive(0)
	{
		Platform::onMouseCaptureChanged.connect(boost::bind(&DragAndDropManager::mouseCaptureChanged, this));
		Input::instance().onCursorReleased.connect(boost::bind(&DragAndDropManager::cursorReleased, this, _1));
	}

	void DragAndDropManager::startDrag(CM::HTexture icon, CM::UINT32 typeId, void* data, std::function<void(bool)> dropCallback)
	{
		mIcon = icon;
		mDragTypeId = typeId;
		mData = data;
		mDropCallback = dropCallback;
		mIsDragInProgress = true;

		mCaptureActive.store(false);
		mCaptureChanged.store(false);

		Platform::captureMouse(*gApplication().getPrimaryWindow());
	}

	void DragAndDropManager::update()
	{
		if(!mIsDragInProgress)
			return;

		// This generally happens when window loses focus and capture is lost (e.g. alt+tab)
		int captureActive = mCaptureActive.load();
		if(!captureActive && mCaptureChanged.load())
		{
			endDrag(false);
		}
	}

	void DragAndDropManager::endDrag(bool processed)
	{
		if(mDropCallback != nullptr)
			mDropCallback(processed);

		mDragTypeId = 0;
		mData = nullptr;
		mDropCallback = nullptr;
		mIsDragInProgress = false;
	}

	void DragAndDropManager::mouseCaptureChanged()
	{
		mCaptureActive.fetch_xor(1); // mCaptureActive = !mCaptureActive;
		mCaptureChanged.store(true);
	}

	void DragAndDropManager::cursorReleased(const PositionalInputEvent& event)
	{
		if(!mIsDragInProgress)
			return;

		Platform::releaseMouseCapture();

		if(!onDragEnded.empty())
		{
			bool processed = onDragEnded(event);

			endDrag(processed);
		}
	}
}