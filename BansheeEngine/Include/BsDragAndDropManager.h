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
		void addDropCallback(std::function<void(bool)> dropCallback);

		/**
		 * @brief	Called once per frame. Internal method.
		 */
		void update();

		boost::signal<bool(const CM::PositionalInputEvent&)> onDragEnded;
	private:
		CM::HTexture mIcon;
		CM::UINT32 mDragTypeId;
		void* mData;
		CM::Vector<std::function<void(bool)>>::type mDropCallbacks;
		bool mIsDragInProgress;

		std::atomic<bool> mCaptureChanged;
		std::atomic<int> mCaptureActive;

		void endDrag(bool processed);
		void mouseCaptureChanged();
		void cursorReleased(const CM::PositionalInputEvent& event);
	};
}