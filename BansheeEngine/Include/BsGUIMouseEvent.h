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
		MouseMove
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
	private:
		CM::Int2 mPosition;
		GUIMouseEventType mType;
		CM::MouseButton mButton;
	};
}