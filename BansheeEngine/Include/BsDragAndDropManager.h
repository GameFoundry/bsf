#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT DragAndDropManager : public CM::Module<DragAndDropManager>
	{
	public:
		DragAndDropManager();

		void startDrag(CM::HTexture icon, CM::UINT32 typeId, void* data, std::function<void(bool)> dropCallback);
		bool isDragInProgress() const { return mIsDragInProgress; }
		CM::UINT32 getDragTypeId() const { return mDragTypeId; }
		void* getDragData() const { return mData; }

		void _endDrag(bool processed);
	private:
		CM::HTexture mIcon;
		CM::UINT32 mDragTypeId;
		void* mData;
		std::function<void(bool)> mDropCallback;
		bool mIsDragInProgress;
	};
}