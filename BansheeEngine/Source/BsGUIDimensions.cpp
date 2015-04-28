#include "BsGUIDimensions.h"
#include "BsGUIElementStyle.h"
#include "BsGUIOptions.h"

namespace BansheeEngine
{
	GUIDimensions GUIDimensions::create()
	{
		return GUIDimensions();
	}

	GUIDimensions GUIDimensions::create(const GUIOptions& options)
	{
		GUIDimensions dimensions;

		for(auto& option : options.mOptions)
		{
			switch(option.type)
			{
			case GUIOption::Type::Position:
				dimensions.x = (INT32)option.min;
				dimensions.y = (INT32)option.max;
				break;
			case GUIOption::Type::FixedWidth:
				dimensions.flags |= GUIDF_FixedWidth | GUIDF_OverWidth;
				dimensions.minWidth = dimensions.maxWidth = option.min;
				break;
			case GUIOption::Type::FixedHeight:
				dimensions.flags |= GUIDF_FixedHeight | GUIDF_OverHeight;
				dimensions.minHeight = dimensions.maxHeight = option.min;
				break;
			case GUIOption::Type::FlexibleWidth:
				dimensions.flags |= GUIDF_OverWidth;
				dimensions.flags &= ~GUIDF_FixedWidth;
				dimensions.minWidth = option.min;
				dimensions.maxWidth = option.max;
				break;
			case GUIOption::Type::FlexibleHeight:
				dimensions.flags |= GUIDF_OverHeight;
				dimensions.flags &= ~GUIDF_FixedHeight;
				dimensions.minHeight = option.min;
				dimensions.maxHeight = option.max;
				break;
			}
		}

		return dimensions;
	}

	void GUIDimensions::updateWithStyle(const GUIElementStyle* style)
	{
		if(!overridenWidth())
		{
			if (style->fixedWidth)
			{
				flags |= GUIDF_FixedWidth;
				minWidth = maxWidth = style->width;
			}
			else
			{
				flags &= ~GUIDF_FixedWidth;
				minWidth = style->minWidth;
				maxWidth = style->maxWidth;
			}
		}

		if(!overridenHeight())
		{
			if (style->fixedHeight)
			{
				flags |= GUIDF_FixedHeight;
				minHeight = maxHeight = style->height;
			}
			else
			{
				flags &= ~GUIDF_FixedHeight;
				minHeight = style->minHeight;
				maxHeight = style->maxHeight;
			}
		}
	}

	GUIDimensions::GUIDimensions()
		:x(0), y(0), minWidth(0), maxWidth(0),
		 minHeight(0), maxHeight(0), flags(0)
	{

	}

	LayoutSizeRange GUIDimensions::calculateSizeRange(const Vector2I& optimal) const
	{
		LayoutSizeRange sizeRange;

		if (fixedHeight())
		{
			sizeRange.optimal.y = minHeight;
			sizeRange.min.y = minHeight;
			sizeRange.max.y = minHeight;
		}
		else
		{
			sizeRange.optimal.y = optimal.y;

			if (minHeight > 0)
			{
				sizeRange.optimal.y = std::max((INT32)minHeight, sizeRange.optimal.y);
				sizeRange.min.y = minHeight;
			}

			if (maxHeight > 0)
			{
				sizeRange.optimal.y = std::min((INT32)maxHeight, sizeRange.optimal.y);
				sizeRange.max.y = maxHeight;
			}
		}

		if (fixedWidth())
		{
			sizeRange.optimal.x = minWidth;
			sizeRange.min.x = minWidth;
			sizeRange.max.x = minWidth;
		}
		else
		{
			sizeRange.optimal.x = optimal.x;

			if (minWidth > 0)
			{
				sizeRange.optimal.x = std::max((INT32)minWidth, sizeRange.optimal.x);
				sizeRange.min.x = minWidth;
			}

			if (maxWidth > 0)
			{
				sizeRange.optimal.x = std::min((INT32)maxWidth, sizeRange.optimal.x);
				sizeRange.max.x = maxWidth;
			}
		}

		return sizeRange;
	}
}