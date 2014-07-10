//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIOptions.h"

namespace BansheeEngine
{
	GUIOption::GUIOption()
		:min(0), max(0), type(Type::FixedWidth)
	{

	}

	GUIOption GUIOption::fixedWidth(UINT32 value)
	{
		GUIOption option;

		option.min = option.max = value;
		option.type = Type::FixedWidth;

		return option;
	}

	GUIOption GUIOption::flexibleWidth(UINT32 min, UINT32 max)
	{
		GUIOption option;

		option.min = min;
		option.max = max;
		option.type = Type::FlexibleWidth;

		return option;
	}

	GUIOption GUIOption::fixedHeight(UINT32 value)
	{
		GUIOption option;

		option.min = option.max = value;
		option.type = Type::FixedHeight;

		return option;
	}

	GUIOption GUIOption::flexibleHeight(UINT32 min, UINT32 max)
	{
		GUIOption option;

		option.min = min;
		option.max = max;
		option.type = Type::FlexibleHeight;

		return option;
	}
}