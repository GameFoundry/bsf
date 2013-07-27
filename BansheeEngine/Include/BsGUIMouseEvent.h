#pragma once

#include "BsPrerequisites.h"
#include "CmInt2.h"

namespace BansheeEngine
{
	enum class GUIMouseEventType
	{
		MouseOver,
		MouseOut,
		MouseDown,
		MouseUp,
		MouseMove,
		MouseWheelScroll,
		MouseDrag,
		MouseDragStart,
		MouseDragEnd
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

		const CM::Int2& getPosition() const { return mPosition; }
		GUIMouseEventType getType() const { return mType; }
		GUIMouseButton getButton() const { return mButton; }
		CM::Int2 getDragAmount() const { return mDragAmount; }
		float getWheelScrollAmount() const { return mWheelScrollAmount; }
		bool isButtonDown(GUIMouseButton button) const { return mButtonStates[(int)button]; }
		GUIElement* getMouseOverElement() const { return mMouseOverElement; }

		bool isShiftDown() const { return mShift; }
		bool isCtrlDown() const { return mCtrl; }
		bool isAltDown() const { return mAlt; }
	private:
		friend class GUIManager;

		bool mButtonStates[GUIMouseButton::Count];
		CM::Int2 mPosition;
		CM::Int2 mDragAmount;
		float mWheelScrollAmount;
		GUIMouseEventType mType;
		GUIMouseButton mButton;
		GUIElement* mMouseOverElement;

		bool mShift;
		bool mCtrl;
		bool mAlt;

		void setMouseOverData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseOutData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseMoveData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseWheelScrollData(GUIElement* mouseOverElement, float scrollAmount);
		void setMouseUpData(GUIElement* mouseOverElement, const CM::Int2& position, GUIMouseButton button);
		void setMouseDownData(GUIElement* mouseOverElement, const CM::Int2& position, GUIMouseButton button);

		void setMouseDragData(GUIElement* mouseOverElement, const CM::Int2& position, const CM::Int2& dragAmount);
		void setMouseDragStartData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseDragEndData(GUIElement* mouseOverElement, const CM::Int2& position);
	};
}