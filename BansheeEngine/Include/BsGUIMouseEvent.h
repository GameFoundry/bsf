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
	private:
		friend class GUIManager;

		CM::Int2 mPosition;
		CM::Int2 mDragAmount;
		GUIMouseEventType mType;
		CM::MouseButton mButton;
		bool mButtonStates[CM::MB_Count];
		GUIElement* mMouseOverElement;

		void setMouseOverData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseOutData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseMoveData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseUpData(GUIElement* mouseOverElement, const CM::Int2& position, CM::MouseButton button);
		void setMouseDownData(GUIElement* mouseOverElement, const CM::Int2& position, CM::MouseButton button);

		void setMouseDragData(GUIElement* mouseOverElement, const CM::Int2& position, const CM::Int2& dragAmount);
		void setMouseDragStartData(GUIElement* mouseOverElement, const CM::Int2& position);
		void setMouseDragEndData(GUIElement* mouseOverElement, const CM::Int2& position);
	};
}