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
				else
				{
					UINT32 optimalWidth = child.element->_getOptimalWidth();

					if(layoutOptions.minHeight > 0)
						optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

					if(layoutOptions.maxHeight > 0)
						optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);

					minimalTotalSize += optimalWidth;
				}
			}
		}

		UINT32 remainingFlexSpaceSize = (UINT32)std::max(0, (INT32)width - (INT32)minimalTotalSize);
		float avgFlexibleSpaceSize = remainingFlexSpaceSize / (float)flexibleSpaceSizes.size();
		
		for(size_t i = 0; i < flexibleSpaceSizes.size(); i++)
		{
			UINT32 spaceSize = (UINT32)Math::CeilToInt(avgFlexibleSpaceSize);
			spaceSize = std::min(remainingFlexSpaceSize, spaceSize);

			remainingFlexSpaceSize -= spaceSize;
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
		bool* processedElements = CM_NEW_ARRAY(bool, (UINT32)mChildren.size(), ScratchAlloc);
		memset(processedElements, 0, mChildren.size() * sizeof(bool));

		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				UINT32 elementWidth = 0;

				if(layoutOptions.fixedWidth)
				{
					elementWidth = layoutOptions.width;
					processedElements[childIdx] = true;
				}
				else
				{
					UINT32 availableWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

					// Clamp average to min max
					if(layoutOptions.minWidth > 0 && availableWidth < layoutOptions.minWidth)
					{
						elementWidth = layoutOptions.minWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
						
						processedElements[childIdx] = true;
						numFreeElems--;
					}
					else if(layoutOptions.maxWidth > 0 && availableWidth > layoutOptions.maxWidth)
					{
						elementWidth = layoutOptions.maxWidth;
						leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
						
						processedElements[childIdx] = true;
						numFreeElems--;
					}
				}

				child.element->_setWidth(elementWidth);
			}

			childIdx++;
		}

		averageWidth = leftoverWidth / (float) numFreeElems;

		// Assign free scaling elements now that we have a good estimate on average width
		// Note: Our average value is JUST an estimate. It's hard to predict the actual value because
		// of min/max constraints and would probably require a few iterations to get a somewhat accurate value,
		// but I don't think that is worth the trouble. For that reason min/max clamping that might happen in 
		// the next block might cause elements to either be a bit too small or too large for the layout.
		// It is the assumption that such clamping won't happen often.
		// 
		// Also calculate offset and element height.
		UINT32 xOffset = 0;
		flexibleSpaceIdx = 0;
		childIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(!processedElements[childIdx])
				{
					UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

					if(layoutOptions.minWidth > 0 && elementWidth < layoutOptions.minWidth)
						elementWidth = layoutOptions.minWidth;
					else if(layoutOptions.maxWidth > 0 && elementWidth > layoutOptions.maxWidth)
						elementWidth = layoutOptions.maxWidth;

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

				Int2 offset(x + xOffset, y + yOffset);
				child.element->_setOffset(offset);
				child.element->_setDepth(depth);

				UINT32 clippedWidth = (UINT32)std::min((INT32)child.element->_getWidth(), (INT32)width - offset.x);
				UINT32 clippedHeight = (UINT32)std::min((INT32)child.element->_getHeight(), (INT32)height - offset.y);

				child.element->_setClipRect(Rect(0, 0, clippedWidth, clippedHeight));

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

			childIdx++;
		}

		CM_DELETE_ARRAY(processedElements, bool, (UINT32)mChildren.size(), ScratchAlloc);
	}
}