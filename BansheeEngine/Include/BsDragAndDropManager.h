#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmInput.h"
#include <boost/signals.hpp>
#include <atomic>

namespace BansheeEngine
{
	class BS_EXPORT DragAndDropManager : public Module<DragAndDropManager>
	{
	public:
		DragAndDropManager();

		/**
		 * @brief	Starts a drag operation of the specified type. This means GUI elements will start receiving
		 * 			drag and drop related events and they may choose to handle them.
		 *
		 * @param	typeId					Type of the drag and drop operation that other objects may query and decide if they want
		 * 									to handle it.
		 * @param 	data					Some operation specific data that is just passed through to however needs it.
		 * @param	dropCallback			The drop callback that gets triggered whenever mouse button is released and drag operation ends.
		 * 									You should perform any cleanup here.
		 * @param	needsValidDropTarget	(optional) Determines whether the drop operation may happen anywhere or does the GUI element need to specifically
		 * 									accept the drag of this type. If false all GUI elements we mouse over will receive drag/drop events, otherwise only
		 * 									those that specifically subscribe to the specified drag operation of this typeId will.
		 * 									
		 *									Additionally this will determine the cursor displayed (whether or not it can have a "denied" state).
		 */
		void startDrag(UINT32 typeId, void* data, std::function<void(bool)> dropCallback, bool needsValidDropTarget = false);
		bool isDragInProgress() const { return mIsDragInProgress; }
		UINT32 getDragTypeId() const { return mDragTypeId; }
		void* getDragData() const { return mData; }
		bool needsValidDropTarget() const { return mNeedsValidDropTarget; }
		void addDropCallback(std::function<void(bool)> dropCallback);

		/**
		 * @brief	Called once per frame. Internal method.
		 */
		void update();

		boost::signal<bool(const PositionalInputEvent&)> onDragEnded;
	private:
		UINT32 mDragTypeId;
		void* mData;
		Vector<std::function<void(bool)>>::type mDropCallbacks;
		bool mIsDragInProgress;
		bool mNeedsValidDropTarget;

		std::atomic<bool> mCaptureChanged;
		std::atomic<int> mCaptureActive;

		void endDrag(bool processed);
		void mouseCaptureChanged();
		void cursorReleased(const PositionalInputEvent& event);
	};
}