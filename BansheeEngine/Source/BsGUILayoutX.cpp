#include "BsGUILayoutX.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "CmMath.h"
#include "CmInt2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GUILayoutX::_update(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth)
	{
		std::vector<UINT32> flexibleSpaceSizes;
		for(auto& child : mChildren)
		{
			if(child.isFlexibleSpace())
			{
				flexibleSpaceSizes.push_back(0);
			}
		}

		// TODO - Calculate flexible space sizes

		// Get a basic estimate of the average width
		UINT32 totalWidth = 0;
		UINT32 numFreeElems = 0;
		float avgWidth = 0.0f;
		UINT32 flexibleSpaceIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUI_LAYOUT_OPTIONS& layoutOptions = child.element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
				{
					totalWidth += layoutOptions.width;
				}
				else
				{
					numFreeElems++;
				}
			}
			else if(child.isFixedSpace())
			{
				totalWidth += child.space->getSize();
			}
			else if(child.isFlexibleSpace())
			{
				totalWidth += flexibleSpaceSizes[flexibleSpaceIdx];
				flexibleSpaceIdx++;
			}
			else
			{
				numFreeElems++;
			}
		}

		UINT32 leftoverWidth = (UINT32)std::max(0, (INT32)width - (INT32)totalWidth);
		float averageWidth = leftoverWidth / (float) numFreeElems;

		// Only assign elements with fixed, or clamped width
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUI_LAYOUT_OPTIONS& layoutOptions = child.element->_getLayoutOptions();

				UINT32 elementWidth = 0;

				if(layoutOptions.fixedWidth)
				{
					elementWidth = layoutOptions.width;
				}
				else
				{
					UINT32 availableWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

					// Clamp average to min max
					if(availableWidth < layoutOptions.minWidth)
					{
						elementWidth = layoutOptions.minWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
						numFreeElems--;
					}
					else if(availableWidth > layoutOptions.maxWidth)
					{
						elementWidth = layoutOptions.maxWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
						numFreeElems--;
					}
					else if(layoutOptions.maxWidth > 0 || layoutOptions.minWidth > 0)
					{
						elementWidth = availableWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)availableWidth);
						numFreeElems--;
					}
				}

				child.element->_setWidth(elementWidth);

				if(layoutOptions.fixedHeight)
					child.element->_setHeight(layoutOptions.fixedHeight);
				else
				{
					UINT32 optimalHeight = child.element->_getOptimalHeight();
					
					if(layoutOptions.minHeight > 0)
						optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

					if(layoutOptions.maxHeight > 0)
						optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);

					child.element->_setHeight(optimalHeight);
				}
			}
		}

		averageWidth = leftoverWidth / (float) numFreeElems;

		// Assign free scaling elements now that we have a good estimate on average width
		// Also calculate offset
		UINT32 xOffset = 0;
		flexibleSpaceIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUI_LAYOUT_OPTIONS& layoutOptions = child.element->_getLayoutOptions();

				if(!layoutOptions.fixedWidth && layoutOptions.maxWidth == 0 && layoutOptions.minWidth == 0)
				{
					UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);
					leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);

					child.element->_setWidth(elementWidth);

					if(layoutOptions.fixedHeight)
						child.element->_setHeight(layoutOptions.fixedHeight);
					else
					{
						UINT32 optimalHeight = child.element->_getOptimalHeight();

						if(layoutOptions.minHeight > 0)
							optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

						if(layoutOptions.maxHeight > 0)
							optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);

						child.element->_setHeight(optimalHeight);
					}
				}

				child.element->_setOffset(Int2(x + xOffset, y));
				child.element->_setDepth(depth);

				// TODO - Set clip rect

				xOffset += child.element->_getWidth();
			}
			else if(child.isLayout())
			{
				UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);
				leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);

				child.layout->_update(x + xOffset, y, elementWidth, height, depth);

				xOffset += elementWidth;
			}
			else if(child.isFixedSpace())
			{
				xOffset += child.space->getSize();
			}
			else if(child.isFlexibleSpace())
			{
				xOffset += flexibleSpaceSizes[flexibleSpaceIdx];
				flexibleSpaceIdx++;
			}
		}
	}
}