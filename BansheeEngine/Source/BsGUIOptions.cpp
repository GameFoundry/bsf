#include "BsGUIOptions.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIOption::GUIOption()
		:min(0), max(0), type(Type::FixedWidth)
	{

	}

	GUIOption GUIOption::fixedWidth(CM::UINT32 value)
	{
		GUIOption option;

		option.min = option.max = value;
		option.type = Type::FixedWidth;

		return option;
	}

	GUIOption GUIOption::flexibleWidth(CM::UINT32 min, CM::UINT32 max)
	{
		GUIOption option;

		option.min = min;
		option.max = max;
		option.type = Type::FlexibleWidth;

		return option;
	}

	GUIOption GUIOption::fixedHeight(CM::UINT32 value)
	{
		GUIOption option;

		option.min = option.max = value;
		option.type = Type::FixedHeight;

		return option;
	}

	GUIOption GUIOption::flexibleHeight(CM::UINT32 min, CM::UINT32 max)
	{
		GUIOption option;

		option.min = min;
		option.max = max;
		option.type = Type::FlexibleHeight;

		return option;
	}
}