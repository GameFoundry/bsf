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
		GUIElement* getMouseOverElement() const { return mMouseOverElement; }
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
		GUIElement* mMouseOverElement;
		CM::UINT32 mDragTypeId;
		void* mDragData;

		bool mShift;
		bool mCtrl;
		bool mAlt;

		void setMouseOverData(GUIElement* mouseOverElement, const CM::Vector2I& position);
		void setMouseOutData(GUIElement* mouseOverElement, const CM::Vector2I& position);
		void setMouseMoveData(GUIElement* mouseOverElement, const CM::Vector2I& position);
		void setMouseWheelScrollData(GUIElement* mouseOverElement, float scrollAmount);
		void setMouseUpData(GUIElement* mouseOverElement, const CM::Vector2I& position, GUIMouseButton button);
		void setMouseDownData(GUIElement* mouseOverElement, const CM::Vector2I& position, GUIMouseButton button);
		void setMouseDoubleClickData(GUIElement* mouseOverElement, const CM::Vector2I& position, GUIMouseButton button);

		void setMouseDragData(GUIElement* mouseOverElement, const CM::Vector2I& position, const CM::Vector2I& dragAmount);
		void setMouseDragStartData(GUIElement* mouseOverElement, const CM::Vector2I& position);
		void setMouseDragEndData(GUIElement* mouseOverElement, const CM::Vector2I& position);

		void setDragAndDropDroppedData(GUIElement* mouseOverElement, const CM::Vector2I& position, CM::UINT32 dragTypeId, void* dragData);
		void setDragAndDropDraggedData(GUIElement* mouseOverElement, const CM::Vector2I& position, CM::UINT32 dragTypeId, void* dragData);
	};
}