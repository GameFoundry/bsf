#include "BsGUILayoutOptions.h"
#include "BsGUIElementStyle.h"
#include "BsGUIOptions.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayoutOptions GUILayoutOptions::create()
	{
		return GUILayoutOptions();
	}

	GUILayoutOptions GUILayoutOptions::create(const GUIOptions& options)
	{
		GUILayoutOptions layoutOptions;

		for(auto& option : options.mOptions)
		{
			switch(option.type)
			{
			case GUIOption::Type::FixedWidth:
				layoutOptions.fixedWidth = true;
				layoutOptions.width = option.min;
				layoutOptions.overridenWidth = true;
				break;
			case GUIOption::Type::FixedHeight:
				layoutOptions.fixedHeight = true;
				layoutOptions.height = option.min;
				layoutOptions.overridenHeight = true;
				break;
			case GUIOption::Type::FlexibleWidth:
				layoutOptions.fixedWidth = false;
				layoutOptions.minWidth = option.min;
				layoutOptions.maxWidth = option.max;
				layoutOptions.overridenWidth = true;
				break;
			case GUIOption::Type::FlexibleHeight:
				layoutOptions.fixedHeight = false;
				layoutOptions.minHeight = option.min;
				layoutOptions.maxHeight = option.max;
				layoutOptions.overridenHeight = true;
				break;
			}
		}

		return layoutOptions;
	}

	void GUILayoutOptions::updateWithStyle(const GUIElementStyle* style)
	{
		if(!overridenWidth)
		{
			fixedWidth = style->fixedWidth;
			width = style->width;
			minWidth = style->minWidth;
			maxWidth = style->maxWidth;
		}

		if(!overridenHeight)
		{
			fixedHeight = style->fixedHeight;
			height = style->height;
			minHeight = style->minHeight;
			maxHeight = style->maxHeight;
		}
	}

	GUILayoutOptions::GUILayoutOptions()
		:width(0), height(0), minWidth(0), maxWidth(0),
		minHeight(0), maxHeight(0), fixedWidth(false), fixedHeight(false),
		overridenWidth(false), overridenHeight(false)
	{

	}
}