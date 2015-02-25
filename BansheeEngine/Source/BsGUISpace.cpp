#include "BsGUISpace.h"

namespace BansheeEngine
{
	LayoutSizeRange GUIFixedSpace::_calculateLayoutSizeRange() const
	{ 
		LayoutSizeRange range;
		range.optimal = _getOptimalSize();
		range.min = range.optimal;
		range.max = range.optimal;

		return range;
	}

	LayoutSizeRange GUIFlexibleSpace::_calculateLayoutSizeRange() const
	{
		LayoutSizeRange range;
		range.optimal = _getOptimalSize();
		range.min = range.optimal;
		range.max = range.optimal;

		return range;
	}
}