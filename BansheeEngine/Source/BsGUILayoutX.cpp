#include "BsGUILayoutX.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "CmMath.h"
#include "CmInt2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GUILayoutX::updateInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth)
	{
		// Calculate flexible space sizes
		std::vector<UINT32> flexibleSpaceSizes;
		UINT32 minimalTotalSize = 0;
		for(auto& child : mChildren)
		{
			if(child.isFlexibleSpace())
			{
				flexibleSpaceSizes.push_back(0);
			}
			else if(child.isFixedSpace())
			{
				minimalTotalSize += child.space->getSize();
			}
			else if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
					minimalTotalSize += layoutOptions.width;
				else if(layoutOptions.minWidth > 0)
					minimalTotalSize += layoutOptions.minWidth;
				else
					minimalTotalSize += child.element->_getOptimalWidth();
			}
		}

		float avgFlexibleSpaceSize = minimalTotalSize / (float)flexibleSpaceSizes.size();
		UINT32 remainingMinSize = minimalTotalSize;

		for(size_t i = 0; i < flexibleSpaceSizes.size(); i++)
		{
			UINT32 spaceSize = (UINT32)Math::CeilToInt(avgFlexibleSpaceSize);
			spaceSize = std::min(remainingMinSize, spaceSize);

			remainingMinSize -= spaceSize;
			flexibleSpaceSizes[i] = spaceSize;
		}

		// Get a basic estimate of the average width
		UINT32 totalWidth = 0;
		UINT32 numFreeElems = 0;
		float avgWidth = 0.0f;
		UINT32 flexibleSpaceIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

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
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				UINT32 elementWidth = 0;

				if(layoutOptions.fixedWidth)
				{
					elementWidth = layoutOptions.width;
				}
				else
				{
					UINT32 availableWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

					// Clamp average to min max
					if(layoutOptions.minWidth > 0 && availableWidth < layoutOptions.minWidth)
					{
						elementWidth = layoutOptions.minWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
						numFreeElems--;
					}
					else if(layoutOptions.maxWidth > 0 && availableWidth > layoutOptions.maxWidth)
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
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(!layoutOptions.fixedWidth && layoutOptions.maxWidth == 0 && layoutOptions.minWidth == 0)
				{
					UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);
					leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);

					child.element->_setWidth(elementWidth);
				}

				if(layoutOptions.fixedHeight)
					child.element->_setHeight(layoutOptions.height);
				else
				{
					UINT32 optimalHeight = child.element->_getOptimalHeight();

					if(layoutOptions.minHeight > 0)
						optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

					if(layoutOptions.maxHeight > 0)
						optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);

					child.element->_setHeight(optimalHeight);
				}

				UINT32 yOffset = (UINT32)Math::CeilToInt((height - child.element->_getHeight()) * 0.5f);

				child.element->_setOffset(Int2(x + xOffset, y + yOffset));
				child.element->_setDepth(depth);

				child.element->_setClipRect(Rect(0, 0, child.element->_getWidth(), child.element->_getHeight()));

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