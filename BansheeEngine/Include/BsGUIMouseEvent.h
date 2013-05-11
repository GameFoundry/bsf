#pragma once

#include "BsPrerequisites.h"
#include "CmInt2.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMouseEvent
	{
	public:
		GUIMouseEvent(const CM::Int2& position)
			:mPosition(position)
		{ }

		const CM::Int2& getPosition() const { return mPosition; }
	private:
		CM::Int2 mPosition;
	};
}