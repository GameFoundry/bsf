#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIFixedSpace
	{
	public:
		GUIFixedSpace(CM::UINT32 size)
			:mSize(size)
		{

		}

		CM::UINT32 getSize() const { return mSize; }

	protected:
		CM::UINT32 mSize;
	};

	class BS_EXPORT GUIFlexibleSpace
	{
	public:
		GUIFlexibleSpace() {}
	};
}