#pragma once

#include "BsPrerequisites.h"
#include "CmInputHandler.h"
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
		MouseDrag,
		MouseDragStart,
		MouseDragEnd
	};

	class BS_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent();
		GUIMouseEvent(bool buttonStates[CM::MB_Count]);

		const CM::Int2& getPosition() const { return mPosition; }
		GUIMouseEventType getType() const { return mType; }
		CM::MouseButton getButton() const { return mButton; }
		CM::Int2 getDragAmount() const { return mDragAmount; }
		bool isButtonDown(CM::MouseButton button) const { return mButtonStates[(int)button]; }
		GUIElement* getMouseOverElement() const { return mMouseOverElement; }

		void _setMouseOverData(GUIElement* mouseOverElement, const CM::Int2& position);
		void _setMouseOutData(GUIElement* mouseOverElement, const CM::Int2& position);
		void _setMouseMoveData(GUIElement* mouseOverElement, const CM::Int2& position);
		void _setMouseUpData(GUIElement* mouseOverElement, const CM::Int2& position, CM::MouseButton button);
		void _setMouseDownData(GUIElement* mouseOverElement, const CM::Int2& position, CM::MouseButton button);

		void _setMouseDragData(GUIElement* mouseOverElement, const CM::Int2& position, const CM::Int2& dragAmount);
		void _setMouseDragStartData(GUIElement* mouseOverElement, const CM::Int2& position);
		void _setMouseDragEndData(GUIElement* mouseOverElement, const CM::Int2& position);
	private:
		CM::Int2 mPosition;
		CM::Int2 mDragAmount;
		GUIMouseEventType mType;
		CM::MouseButton mButton;
		bool mButtonStates[CM::MB_Count];
		GUIElement* mMouseOverElement;
	};
}