#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	enum class GUICommandEventType
	{
		Redraw
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

		void setRedrawData() { mType = GUICommandEventType::Redraw; }
	};
}