#include "BsGUILayoutOptions.h"
#include "BsGUIElementStyle.h"
#include "BsGUIOptions.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayoutOptions GUILayoutOptions::create(const GUIOptions& options, const GUIElementStyle* style)
	{
		GUILayoutOptions layoutOptions;

		layoutOptions.fixedWidth = style->fixedWidth;
		layoutOptions.fixedHeight = style->fixedHeight;
		layoutOptions.width = style->width;
		layoutOptions.height = style->height;
		layoutOptions.minWidth = style->minWidth;
		layoutOptions.maxWidth = style->maxWidth;
		layoutOptions.minHeight = style->minHeight;
		layoutOptions.maxHeight = style->maxHeight;

		for(auto& option : options.mOptions)
		{
			switch(option.type)
			{
			case GUIOption::Type::FixedWidth:
				layoutOptions.fixedWidth = true;
				layoutOptions.width = option.min;
				break;
			case GUIOption::Type::FixedHeight:
				layoutOptions.fixedHeight = true;
				layoutOptions.height = option.min;
				break;
			case GUIOption::Type::FlexibleWidth:
				layoutOptions.fixedWidth = false;
				layoutOptions.minWidth = option.min;
				layoutOptions.maxWidth = option.max;
				break;
			case GUIOption::Type::FlexibleHeight:
				layoutOptions.fixedHeight = false;
				layoutOptions.minHeight = option.min;
				layoutOptions.maxHeight = option.max;
				break;
			}
		}

		return layoutOptions;
	}

	GUILayoutOptions GUILayoutOptions::create(const GUIElementStyle* style)
	{
		GUILayoutOptions layoutOptions;

		layoutOptions.fixedWidth = style->fixedWidth;
		layoutOptions.fixedHeight = style->fixedHeight;
		layoutOptions.width = style->width;
		layoutOptions.height = style->height;
		layoutOptions.minWidth = style->minWidth;
		layoutOptions.maxWidth = style->maxWidth;
		layoutOptions.minHeight = style->minHeight;
		layoutOptions.maxHeight = style->maxHeight;

		return layoutOptions;
	}

	GUILayoutOptions::GUILayoutOptions()
		:width(0), height(0), minWidth(0), maxWidth(0),
		minHeight(0), maxHeight(0), fixedWidth(false), fixedHeight(false)
	{

	}
}