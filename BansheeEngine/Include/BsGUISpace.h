#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIFixedSpace
	{
	public:
		GUIFixedSpace(UINT32 size)
			:mSize(size)
		{

		}

		UINT32 getSize() const { return mSize; }

	protected:
		UINT32 mSize;
	};

	class BS_EXPORT GUIFlexibleSpace
	{
	public:
		GUIFlexibleSpace() {}
	};
}