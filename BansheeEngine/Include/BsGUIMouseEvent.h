#pragma once

#include "BsPrerequisites.h"
#include "CmVector2.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent(const CM::Vector2& position)
			:mPosition(position)
		{ }

	private:
		CM::Vector2 mPosition;
	};
}