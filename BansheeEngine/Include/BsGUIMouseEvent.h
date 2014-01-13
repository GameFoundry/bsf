#pragma once

#include "BsPrerequisites.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	enum class GUIMouseEventType
	{
		MouseOver,
		MouseOut,
		MouseDown,
		MouseUp,
		MouseDoubleClick,
		MouseMove,
		MouseWheelScroll,
		MouseDrag,
		MouseDragStart,
		MouseDragEnd,
		MouseDragAndDropDragged,
		MouseDragAndDropDropped,
	};

	enum class GUIMouseButton
	{
		Left, Right, Middle, Count
	};

	class BS_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent();
		GUIMouseEvent(bool buttonStates[GUIMouseButton::Count], bool shift, bool ctrl, bool alt);

		const CM::Vector2I& getPosition() const { return mPosition; }
		GUIMouseEventType getType() const { return mType; }
		GUIMouseButton getButton() const { return mButton; }
		CM::Vector2I getDragAmount() const { return mDragAmount; }
		float getWheelScrollAmount() const { return mWheelScrollAmount; }
		bool isButtonDown(GUIMouseButton button) const { return mButtonStates[(int)button]; }
		CM::UINT32 getDragAndDropTypeId() const { return mDragTypeId; }
		void* getDragAndDropData() const { return mDragData; }

		bool isShiftDown() const { return mShift; }
		bool isCtrlDown() const { return mCtrl; }
		bool isAltDown() const { return mAlt; }
	private:
		friend class GUIManager;

		bool mButtonStates[GUIMouseButton::Count];
		CM::Vector2I mPosition;
		CM::Vector2I mDragAmount;
		float mWheelScrollAmount;
		GUIMouseEventType mType;
		GUIMouseButton mButton;
		CM::UINT32 mDragTypeId;
		void* mDragData;

		bool mShift;
		bool mCtrl;
		bool mAlt;

		void setMouseOverData(const CM::Vector2I& position);
		void setMouseOutData(const CM::Vector2I& position);
		void setMouseMoveData(const CM::Vector2I& position);
		void setMouseWheelScrollData(float scrollAmount);
		void setMouseUpData(const CM::Vector2I& position, GUIMouseButton button);
		void setMouseDownData(const CM::Vector2I& position, GUIMouseButton button);
		void setMouseDoubleClickData(const CM::Vector2I& position, GUIMouseButton button);

		void setMouseDragData(const CM::Vector2I& position, const CM::Vector2I& dragAmount);
		void setMouseDragStartData(const CM::Vector2I& position);
		void setMouseDragEndData(const CM::Vector2I& position);

		void setDragAndDropDroppedData(const CM::Vector2I& position, CM::UINT32 dragTypeId, void* dragData);
		void setDragAndDropDraggedData(const CM::Vector2I& position, CM::UINT32 dragTypeId, void* dragData);
	};
}