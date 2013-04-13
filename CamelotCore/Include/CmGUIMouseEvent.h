#pragma once

#include "CmPrerequisites.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	class CM_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent(const Vector2& position)
			:mPosition(position)
		{ }

	private:
		Vector2 mPosition;
	};
}