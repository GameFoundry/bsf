#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	enum class GUICommandEventType
	{
		Redraw, FocusLost, FocusGained, CursorMoveLeft, CursorMoveRight, CursorMoveUp, CursorMoveDown, 
		SelectLeft, SelectRight, SelectUp, SelectDown, Undo, Redo, Rename,
		Escape, Delete, Backspace, Return, SelectAll, Copy, Cut, Paste, Tab
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