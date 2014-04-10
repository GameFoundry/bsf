#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	enum class GUICommandEventType
	{
		Redraw, FocusLost, FocusGained, MoveLeft, MoveRight, MoveUp, MoveDown, 
		SelectLeft, SelectRight, SelectUp, SelectDown, Escape, Delete, Backspace, Return
	};

	class BS_EXPORT GUICommandEvent
	{
	public:
		GUICommandEvent()
			:mType(GUICommandEventType::Redraw)
		{ }

		GUICommandEventType getType() const { return mType; }
	private:
		friend class GUIManager;

		GUICommandEventType mType;

		void setType(GUICommandEventType type) { mType = type; }
	};
}