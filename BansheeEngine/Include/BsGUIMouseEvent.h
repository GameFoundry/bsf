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
		GUIMouseEvent(bool buttonStates[(int)GUIMouseButton::Count], bool shift, bool ctrl, bool alt);

		const Vector2I& getPosition() const { return mPosition; }
		GUIMouseEventType getType() const { return mType; }
		GUIMouseButton getButton() const { return mButton; }
		Vector2I getDragAmount() const { return mDragAmount; }
		float getWheelScrollAmount() const { return mWheelScrollAmount; }
		bool isButtonDown(GUIMouseButton button) const { return mButtonStates[(int)button]; }
		UINT32 getDragAndDropTypeId() const { return mDragTypeId; }
		void* getDragAndDropData() const { return mDragData; }

		bool isShiftDown() const { return mShift; }
		bool isCtrlDown() const { return mCtrl; }
		bool isAltDown() const { return mAlt; }
	private:
		friend class GUIManager;

		bool mButtonStates[(int)GUIMouseButton::Count];
		Vector2I mPosition;
		Vector2I mDragAmount;
		float mWheelScrollAmount;
		GUIMouseEventType mType;
		GUIMouseButton mButton;
		UINT32 mDragTypeId;
		void* mDragData;

		bool mShift;
		bool mCtrl;
		bool mAlt;

		void setMouseOverData(const Vector2I& position);
		void setMouseOutData(const Vector2I& position);
		void setMouseMoveData(const Vector2I& position);
		void setMouseWheelScrollData(float scrollAmount);
		void setMouseUpData(const Vector2I& position, GUIMouseButton button);
		void setMouseDownData(const Vector2I& position, GUIMouseButton button);
		void setMouseDoubleClickData(const Vector2I& position, GUIMouseButton button);

		void setMouseDragData(const Vector2I& position, const Vector2I& dragAmount);
		void setMouseDragStartData(const Vector2I& position);
		void setMouseDragEndData(const Vector2I& position);

		void setDragAndDropDroppedData(const Vector2I& position, UINT32 dragTypeId, void* dragData);
		void setDragAndDropDraggedData(const Vector2I& position, UINT32 dragTypeId, void* dragData);
	};
}