#include "BsGUIMouseEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIMouseEvent::GUIMouseEvent()
		:mType(GUIMouseEventType::MouseMove), mButton(GUIMouseButton::Left), mMouseOverElement(nullptr),
		mShift(false), mCtrl(false), mAlt(false), mWheelScrollAmount(0.0f)
	{

	}

	GUIMouseEvent::GUIMouseEvent(bool buttonStates[GUIMouseButton::Count], bool shift, bool ctrl, bool alt)
		:mType(GUIMouseEventType::MouseMove), mButton(GUIMouseButton::Left), mMouseOverElement(nullptr),
		mShift(shift), mCtrl(ctrl), mAlt(alt)
	{
		memcpy(mButtonStates, buttonStates, sizeof(mButtonStates));
	}

	void GUIMouseEvent::setMouseOverData(GUIElement* mouseOverElement, const Vector2I& position)
	{
		mType = GUIMouseEventType::MouseOver;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseOutData(GUIElement* mouseOverElement, const Vector2I& position)
	{
		mType = GUIMouseEventType::MouseOut;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseMoveData(GUIElement* mouseOverElement, const Vector2I& position)
	{
		mType = GUIMouseEventType::MouseMove;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseWheelScrollData(GUIElement* mouseOverElement, float scrollAmount)
	{
		mType = GUIMouseEventType::MouseWheelScroll;
		mPosition = Vector2I();
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = scrollAmount;
	}

	void GUIMouseEvent::setMouseUpData(GUIElement* mouseOverElement, const Vector2I& position, GUIMouseButton button)
	{
		mType = GUIMouseEventType::MouseUp;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = button;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseDownData(GUIElement* mouseOverElement, const Vector2I& position, GUIMouseButton button)
	{
		mType = GUIMouseEventType::MouseDown;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = button;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseDoubleClickData(GUIElement* mouseOverElement, const Vector2I& position, GUIMouseButton button)
	{
		mType = GUIMouseEventType::MouseDoubleClick;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = button;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseDragData(GUIElement* mouseOverElement, const Vector2I& position, const Vector2I& dragAmount)
	{
		mType = GUIMouseEventType::MouseDrag;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = dragAmount;
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseDragStartData(GUIElement* mouseOverElement, const Vector2I& position)
	{
		mType = GUIMouseEventType::MouseDragStart;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setMouseDragEndData(GUIElement* mouseOverElement, const Vector2I& position)
	{
		mType = GUIMouseEventType::MouseDragEnd;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
	}

	void GUIMouseEvent::setDragAndDropDroppedData(GUIElement* mouseOverElement, const Vector2I& position, CM::UINT32 dragTypeId, void* dragData)
	{
		mType = GUIMouseEventType::MouseDragAndDropDropped;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
		mDragTypeId = dragTypeId;
		mDragData = dragData;
	}

	void GUIMouseEvent::setDragAndDropDraggedData(GUIElement* mouseOverElement, const Vector2I& position, CM::UINT32 dragTypeId, void* dragData)
	{
		mType = GUIMouseEventType::MouseDragAndDropDragged;
		mPosition = position;
		mMouseOverElement = mouseOverElement;
		mButton = GUIMouseButton::Left;
		mDragAmount = Vector2I();
		mWheelScrollAmount = 0.0f;
		mDragTypeId = dragTypeId;
		mDragData = dragData;
	}
}