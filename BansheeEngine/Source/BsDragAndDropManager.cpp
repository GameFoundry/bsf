#include "BsDragAndDropManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	DragAndDropManager::DragAndDropManager()
		:mIsDragInProgress(false), mDragTypeId(0), mData(nullptr)
	{

	}

	void DragAndDropManager::startDrag(CM::HTexture icon, CM::UINT32 typeId, void* data, std::function<void(bool)> dropCallback)
	{
		mIcon = icon;
		mDragTypeId = typeId;
		mData = data;
		mDropCallback = dropCallback;
		mIsDragInProgress = true;
	}

	void DragAndDropManager::_endDrag(bool processed)
	{
		if(mDropCallback != nullptr)
			mDropCallback(processed);

		mDragTypeId = 0;
		mData = nullptr;
		mDropCallback = nullptr;
		mIsDragInProgress = false;
	}
}