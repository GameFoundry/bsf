#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmInput.h"
#include <boost/signals.hpp>
#include <atomic>

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

		/**
		 * @brief	Called once per frame. Internal method.
		 */
		void update();

		boost::signal<bool(const CM::ButtonEvent&)> onDragEnded;
	private:
		CM::HTexture mIcon;
		CM::UINT32 mDragTypeId;
		void* mData;
		std::function<void(bool)> mDropCallback;
		bool mIsDragInProgress;

		std::atomic<bool> mCaptureChanged;
		std::atomic<int> mCaptureActive;

		void endDrag(bool processed);
		void mouseCaptureChanged();
		void mouseUp(const CM::ButtonEvent& event);
	};
}