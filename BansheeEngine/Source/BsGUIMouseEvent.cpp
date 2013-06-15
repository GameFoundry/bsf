#include "BsGUIMouseEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIMouseEvent::GUIMouseEvent()
		:mType(GUIMouseEventType::MouseMove), mButton(GUIMouseButton::Left), mMouseOverElement(nullptr)
	{

	}

	GUIMouseEvent::GUIMouseEvent(bool buttonStates[GUIMouseButton::Count])
		:mType(GUIMouseEventType::MouseMove), mButton(GUIMouseButton::Left), mMouseOverElement(nullptr)
	{
		memcpy(mButtonStates, buttonStates, sizeof(mButtonStates));
	}

	void GUIMouseEvent::setMouseOverData(GUIElement* mouseOverElement, const Int2& position)
	{
		mType = GUIMouseEventType::MouseOver;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseOutData(GUIElement* mouseOverElement, const Int2& position)
	{
		mType = GUIMouseEventType::MouseOut;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseMoveData(GUIElement* mouseOverElement, const Int2& position)
	{
		mType = GUIMouseEventType::MouseMove;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseUpData(GUIElement* mouseOverElement, const Int2& position, GUIMouseButton button)
	{
		mType = GUIMouseEventType::MouseUp;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = button;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseDownData(GUIElement* mouseOverElement, const Int2& position, GUIMouseButton button)
	{
		mType = GUIMouseEventType::MouseDown;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = button;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseDragData(GUIElement* mouseOverElement, const Int2& position, const Int2& dragAmount)
	{
		mType = GUIMouseEventType::MouseDrag;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = dragAmount;
	}

	void GUIMouseEvent::setMouseDragStartData(GUIElement* mouseOverElement, const Int2& position)
	{
		mType = GUIMouseEventType::MouseDragStart;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Int2();
	}

	void GUIMouseEvent::setMouseDragEndData(GUIElement* mouseOverElement, const Int2& position)
	{
		mType = GUIMouseEventType::MouseDragEnd;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Int2();
	}
}