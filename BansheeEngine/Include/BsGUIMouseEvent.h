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
		MouseDrag
	};

	class BS_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent(GUIMouseEventType type, const CM::Int2& position, CM::MouseButton button = CM::MB_Left)
			:mType(type), mPosition(position), mButton(button)
		{ }

		const CM::Int2& getPosition() const { return mPosition; }
		GUIMouseEventType getType() const { return mType; }
		CM::MouseButton getButton() const { return mButton; }
		CM::Int2 getDragAmount() const { return mDragAmount; }

		void _setDragAmount(CM::Int2 dragAmount) { mDragAmount = dragAmount; }
	private:
		CM::Int2 mPosition;
		CM::Int2 mDragAmount;
		GUIMouseEventType mType;
		CM::MouseButton mButton;
	};
}